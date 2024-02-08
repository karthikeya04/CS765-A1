
#include "peer.h"

Peer::Peer(int id, bool is_low_cpu, bool is_slow_peer,
           SimulatorPtr sim) : id_(id), sim_(std::move(sim)),
                               is_low_cpu_(is_low_cpu), is_slow_peer_(is_slow_peer)
{
}

void Peer::AddLink(LinkPtr link) {

}

bool Peer::IsLowCpu() const { return is_low_cpu_; }
bool Peer::IsSlow() const { return is_slow_peer_; }

void Peer::GenAndBroadcastTxnOp(){

};
void Peer::ReceiveAndForwardTxnOp(std::shared_ptr<Blockchain::Txn> txn)
{
}
void Peer::BroadcastMinedBlkOp()
{
}
void Peer::ReceiveAndForwardBlkOp(std::shared_ptr<Blockchain::Block> blk)
{
}
