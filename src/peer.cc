
#include "peer.h"

#include <vector>

#include "blockchain.h"
#include "constants.h"
#include "debug.h"
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
        // start mining
        StartMining();
        ScheduleTxnGen();
    }
}

//-----------------------------------------------------------------------------

void Peer::AddLink(LinkPtr link) { links_.emplace_back(std::move(link)); }

bool Peer::IsLowCpu() const { return is_low_cpu_; }
bool Peer::IsSlow() const { return is_slow_peer_; }

//-----------------------------------------------------------------------------

void Peer::ScheduleTxnGen() {
    GET_SHARED_PTR(sim, sim_);
    if (sim->is_over_) return;
    EventPtr event = std::make_shared<GenAndBroadcastTxn>(shared_from_this());
    double delay = txn_gen_distr_(Simulator::rng);
    sim->Schedule(event, delay);
}

//-----------------------------------------------------------------------------

void Peer::StartMining() {
    GET_SHARED_PTR(sim, sim_);
    if (blockchain_->Size() >= 
        sim->params_->max_blocks) {  // stop mining (END of the simulation)
        sim->is_over_ = true;
        return;
    }
    auto block = blockchain_->CreateValidBlock();
    current_mining_event_ =
        std::make_shared<BroadcastMinedBlk>(shared_from_this(), block);
    EventPtr event = current_mining_event_;

    double I = sim->params_->avg_blks_interarrival;
    double distr_mean = I / hashing_power_;
    RealExpDistr pow_distr(1 / distr_mean);
    double pow_time = pow_distr(Simulator::rng);
    sim->Schedule(event, pow_time /* delay */);
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

void Peer::BroadcastToNeighbours(BlockPtr block){
    GET_SHARED_PTR(sim, sim_);
    blockchain_->Notify(block);
    if (blockchain_->AddBlock(block)) {
        for (auto link : links_) {
            GET_SHARED_PTR(to, link->to);
            EventPtr event =
                std::make_shared<ReceiveAndForwardBlk>(to, block, id_);
            sim->Schedule(event, link->latency(block->Size()));
        }
    }
}

void Peer::BroadcastMinedBlkOp(BlockPtr block) {
    assert(blockchain_->IsLongest(block->par_block_id));
    BroadcastToNeighbours(block);
    // start mining again
    StartMining();
}

//-----------------------------------------------------------------------------

void Peer::ReceiveAndForwardBlkOp(BlockPtr block, int sender_id) {
    GET_SHARED_PTR(sim, sim_);
    if (blockchain_->Contains(block->id)) return;
    blockchain_->Notify(block);
    if (!blockchain_->AddBlock(block)) {  // invalid block
        return;
    }
    // abort the current mining event if the longest chain has changed
    if (!blockchain_->IsLongest(current_mining_event_->GetParentId())) {
        current_mining_event_->Abort();
        StartMining();
    }
    for (auto link : links_) {
        GET_SHARED_PTR(to, link->to);
        if (to->id_ == sender_id) continue;
        EventPtr event = std::make_shared<ReceiveAndForwardBlk>(to, block, id_);
        sim->Schedule(event, link->latency(block->Size()));
    }
}

//-----------------------------------------------------------------------------

int Peer::GetNumPeersInNetwork() {
    GET_SHARED_PTR(sim, sim_);
    return sim->params_->network_params->num_peers;
}

//-----------------------------------------------------------------------------

void SelfishPeer::StartMining() {
    GET_SHARED_PTR(sim, sim_);
    if (blockchain_->Size() >= 
        sim->params_->max_blocks) {  // stop mining (END of the simulation)
        sim->is_over_ = true;
        return;
    }
    
    int par_block_id;
    if(state_ == 0){ // secret_chain.empty()
        par_block_id = blockchain_->GetLVC();
    }else{
        assert(!secret_chain_.empty());
        par_block_id = secret_chain_.back()->id;
    }
    // lightweight blocks with no (non-coinbase) txns travel faster
    auto block = std::make_shared<Block>(par_block_id);
    block->txns.emplace_back(-1, id_, MINING_FEE);
    
    current_mining_event_ =
        std::make_shared<BroadcastMinedBlk>(shared_from_this(), block);
    EventPtr event = current_mining_event_;

    double I = sim->params_->avg_blks_interarrival;
    double distr_mean = I / hashing_power_;
    RealExpDistr pow_distr(1 / distr_mean);
    double pow_time = pow_distr(Simulator::rng);
    sim->Schedule(event, pow_time /* delay */);
}

//-----------------------------------------------------------------------------

void SelfishPeer::BroadcastMinedBlkOp(BlockPtr block) {
    if(state_ == -1){
        BroadcastToNeighbours(block);
        state_ = 0;
        secret_chain_.clear();
    }else{
        secret_chain_.push_back(block);
        ++ state_;   
    }
    StartMining();
}

//-----------------------------------------------------------------------------

void SelfishPeer::ReceiveAndForwardBlkOp(BlockPtr block, int /*sender_id*/) {
    int chain_length = blockchain_->GetLVClength();
    GET_SHARED_PTR(sim, sim_);
    if (blockchain_->Contains(block->id)) return;
    blockchain_->Notify(block);
    if (!blockchain_->AddBlock(block)) {  // invalid block
        return;
    }
    // increase might be >1 if blocks recvd out of order are finally added to tree just now
    int increase = blockchain_->GetLVClength() - chain_length;

    while(increase--) 
    switch (state_){
        case -1:
        case 0:
            state_ = 0;
            secret_chain_.clear();
            current_mining_event_->Abort();
            StartMining();
            break;
        case 1:
            state_ = -1;
            BroadcastToNeighbours(secret_chain_.back());
            secret_chain_.pop_front();
            break;
        case 2:
            state_ = 0;
            for(auto blk : secret_chain_){
                BroadcastToNeighbours(blk);
            }
            secret_chain_.clear();
            // problem statement says to start a new attack, but why would the adversary pause his current mining event to start a new one on the same block?
            current_mining_event_->Abort();
            StartMining();
            break;
        default:
            -- state_;
            BroadcastToNeighbours(secret_chain_.front());
            secret_chain_.pop_front();
    }
}

//-----------------------------------------------------------------------------