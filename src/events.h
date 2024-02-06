
#ifndef _SRC_EVENTS_H_
#define _SRC_EVENTS_H_

#include <memory>
#include <functional>

#include "simulator.h"
#include "peer.h"
#include "blockchain.h"

// Abstract Event class
class Event {
 public:
    typedef std::shared_ptr<Event> Ptr;
    typedef std::weak_ptr<Event> WeakPtr;

    enum class State {
        // Event has not been triggered or aborted.
        Pending,
        // Event has been triggered.
        Triggered,
        // Event has been triggered and processed.
        Processed,
        // Event has been aborted.
        Aborted
    };

    // enum class Type {
    //     GenAndBroadcastTxn,
    //     ReceiveAndForwardTxn,
    //     BroadcastMinedBlk,
    //     ReceiveAndForwardBlk
    // };

    Event(Peer::Ptr peer);

    virtual ~Event() {}

    virtual void Process() = 0;

 protected:
    Peer::WeakPtr owner_;
    State state = State::Pending;
    // Type type;
};

class GenAndBroadcastTxn : public Event {
 public:
    GenAndBroadcastTxn(Peer::Ptr peer);
    void Process() override;
};

class ReceiveAndForwardTxn : public Event {
 public:
    ReceiveAndForwardTxn(Peer::Ptr peer, std::shared_ptr<Blockchain::Txn> txn);
    void Process() override;
 private:
    std::shared_ptr<Blockchain::Txn> txn_;
};

class BroadcastMinedBlk : public Event {
 public:
    BroadcastMinedBlk(Peer::Ptr peer);
    void Process() override;
};

class ReceiveAndForwardBlk : public Event {
 public:
    ReceiveAndForwardBlk(Peer::Ptr peer, std::shared_ptr<Blockchain::Block> blk);
    void Process() override;
 private:
    std::shared_ptr<Blockchain::Block> block_;
};

#endif // _SRC_EVENTS_H_
