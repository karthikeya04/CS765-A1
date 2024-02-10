
#include "events.h"

Event::Event(PeerPtr owner) : owner_(owner) {}
void Event::Abort() { state = State::Aborted; }
bool Event::isAborted() { return (state == State::Aborted); }
//-----------------------------------------------------------------------------

GenAndBroadcastTxn::GenAndBroadcastTxn(PeerPtr owner)
    : Event(std::move(owner)) {}

//-----------------------------------------------------------------------------

void GenAndBroadcastTxn::Process() {
    GET_SHARED_PTR(owner, owner_);
    owner->GenAndBroadcastTxnOp();
}

//-----------------------------------------------------------------------------

ReceiveAndForwardTxn::ReceiveAndForwardTxn(PeerPtr owner,
                                           std::shared_ptr<Blockchain::Txn> txn,
                                           int sender_id_)
    : Event(owner), txn_(txn), sender_id_(sender_id_) {}

//-----------------------------------------------------------------------------

void ReceiveAndForwardTxn::Process() {
    GET_SHARED_PTR(owner, owner_);
    owner->ReceiveAndForwardTxnOp(txn_, sender_id_);
}

//-----------------------------------------------------------------------------

BroadcastMinedBlk::BroadcastMinedBlk(PeerPtr owner,
                                     std::shared_ptr<Block> block)
    : Event(owner), block_(block) {}

//-----------------------------------------------------------------------------

void BroadcastMinedBlk::Process() {
    GET_SHARED_PTR(owner, owner_);
    owner->BroadcastMinedBlkOp(block_);
}

//-----------------------------------------------------------------------------

ReceiveAndForwardBlk::ReceiveAndForwardBlk(
    PeerPtr owner, std::shared_ptr<Blockchain::Block> blk, int sender_id)
    : Event(owner), block_(blk), sender_id_(sender_id) {}

//-----------------------------------------------------------------------------

void ReceiveAndForwardBlk::Process() {
    GET_SHARED_PTR(owner, owner_);
    owner->ReceiveAndForwardBlkOp(block_, sender_id_);
}

//-----------------------------------------------------------------------------
