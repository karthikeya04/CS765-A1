
#include "peer.h"

#include <vector>

#include "blockchain.h"
#include "constants.h"
#include "simulator.h"
#include "types.h"

//-----------------------------------------------------------------------------

Peer::Peer(int id, bool is_low_cpu, bool is_slow_peer, double hashing_power,
           SimulatorPtr sim)
    : id_(id),
      is_low_cpu_(is_low_cpu),
      is_slow_peer_(is_slow_peer),
      hashing_power_(hashing_power),
      balance_(0),
      sim_(sim),
      net_(sim->network_),
      txn_gen_distr_(1 / (sim->GetTtx())) {}

//-----------------------------------------------------------------------------

void Peer::Init() {
    if (!is_initialized_) {
        is_initialized_ = true;
        blockchain_ = std::make_shared<Blockchain>(shared_from_this());
        ScheduleTxnGen();
        // start mining
        StartMining();
    }
}

//-----------------------------------------------------------------------------

void Peer::AddLink(LinkPtr link) { links_.emplace_back(std::move(link)); }

bool Peer::IsLowCpu() const { return is_low_cpu_; }
bool Peer::IsSlow() const { return is_slow_peer_; }

//-----------------------------------------------------------------------------

void Peer::ScheduleTxnGen() {
    GET_SHARED_PTR(sim, sim_);
    EventPtr event = std::make_shared<GenAndBroadcastTxn>(shared_from_this());
    double delay = txn_gen_distr_(Simulator::rng);
    sim->Schedule(event, delay);
}

//-----------------------------------------------------------------------------

void Peer::StartMining() {
    auto block = blockchain_->CreateValidBlock();
    EventPtr event =
        std::make_shared<BroadcastMinedBlk>(shared_from_this(), block);
    GET_SHARED_PTR(sim, sim_);
    double I = sim->params_->avg_blks_interarrival;
    double distr_mean = I / hashing_power_;
    RealExpDistr pow_distr(1 / distr_mean);
    double pow_time = pow_distr(Simulator::rng);
    sim->Schedule(event, pow_time /* delay */);
    current_mining_event_ = event;
}

//-----------------------------------------------------------------------------

void Peer::GenAndBroadcastTxnOp() {
    if (balance_ > 0) {
        // generate the txn
        GET_SHARED_PTR(net, net_);
        const int N = net->params_->num_peers;
        IntUniformDistr p_distr(0, N - 2);
        auto payee = p_distr(Simulator::rng);
        if (payee >= id_) payee++;
        IntUniformDistr a_distr(1, balance_);
        int64 amount = a_distr(Simulator::rng);
        auto txn = std::make_shared<Txn>(id_, payee, amount);

        // broadcast
        GET_SHARED_PTR(sim, sim_);
        for (auto link : links_) {
            GET_SHARED_PTR(to, link->to);
            EventPtr event =
                std::make_shared<ReceiveAndForwardTxn>(to, txn, id_);
            sim->Schedule(event, link->latency(TXN_SIZE));
        }
    }
    // schedule next txn generation
    ScheduleTxnGen();
};

//-----------------------------------------------------------------------------

void Peer::ReceiveAndForwardTxnOp(std::shared_ptr<Txn> txn, int sender_id) {
    if (received_txns_.find(txn->id) !=
        received_txns_.end()) {  // already has the txn, i.e must have already
                                 // forwarded it to neighbours
        return;
    }
    received_txns_[txn->id] = txn;

    // forward to neighbours
    GET_SHARED_PTR(sim, sim_);
    for (auto link : links_) {
        GET_SHARED_PTR(to, link->to);
        if (to->id_ == sender_id) continue;
        EventPtr event = std::make_shared<ReceiveAndForwardTxn>(to, txn, id_);
        sim->Schedule(event, link->latency(TXN_SIZE));
    }
}

//-----------------------------------------------------------------------------

void Peer::BroadcastMinedBlkOp(BlockPtr block) {
    GET_SHARED_PTR(sim, sim_);
    block->arrival_time = sim->now_;
    GET_SHARED_PTR(parent, block->parent);
    assert(blockchain_->IsLongest(parent));
    blockchain_->AddBlock(block);
    for (auto link : links_) {
        GET_SHARED_PTR(to, link->to);
        EventPtr event = std::make_shared<ReceiveAndForwardBlk>(to, block, id_);
        sim->Schedule(event, link->latency(block->Size()));
    }
}

//-----------------------------------------------------------------------------

void Peer::ReceiveAndForwardBlkOp(BlockPtr block, int sender_id) {
    GET_SHARED_PTR(sim, sim_);
    block->arrival_time = sim->now_;
    if (blockchain_->Contains(block->id)) return;
    blockchain_->AddBlock(block);
    for (auto link : links_) {
        GET_SHARED_PTR(to, link->to);
        if (to->id_ == sender_id) continue;
        EventPtr event = std::make_shared<ReceiveAndForwardBlk>(to, block, id_);
        sim->Schedule(event, link->latency(block->Size()));
    }
    // what if parent block hasn't arrived yet??
}

//-----------------------------------------------------------------------------

int Peer::GetNumPeersInNetwork() {
    GET_SHARED_PTR(sim, sim_);
    return sim->params_->network_params->num_peers;
}

//-----------------------------------------------------------------------------
