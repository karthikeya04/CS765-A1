
#include "events.h"

Event::Event(PeerPtr owner) : owner_(owner) {}

GenAndBroadcastTxn::GenAndBroadcastTxn(PeerPtr peer)
    : Event(std::move(peer)) {}

void GenAndBroadcastTxn::Process()
{
    auto owner = owner_.lock();
    owner->GenAndBroadcastTxnOp();
}

ReceiveAndForwardTxn::ReceiveAndForwardTxn(PeerPtr peer, std::shared_ptr<Blockchain::Txn> txn)
    : Event(std::move(peer)), txn_(std::move(txn)) {}

void ReceiveAndForwardTxn::Process()
{
    auto owner = owner_.lock();
    owner->ReceiveAndForwardTxnOp(txn_);
}

BroadcastMinedBlk::BroadcastMinedBlk(PeerPtr peer)
    : Event(std::move(peer)) {}

void BroadcastMinedBlk::Process()
{
    auto owner = owner_.lock();
    owner->BroadcastMinedBlkOp();
}

ReceiveAndForwardBlk::ReceiveAndForwardBlk(PeerPtr peer, std::shared_ptr<Blockchain::Block> blk)
    : Event(std::move(peer)), block_(std::move(blk)) {}

void ReceiveAndForwardBlk::Process()
{
    auto owner = owner_.lock();
    owner->ReceiveAndForwardBlkOp(block_);
}
