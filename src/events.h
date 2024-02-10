
#ifndef _SRC_EVENTS_H_
#define _SRC_EVENTS_H_

#include <functional>
#include <memory>

#include "blockchain.h"
#include "peer.h"
#include "simulator.h"
#include "types.h"

// Abstract Event class
class Event {
   public:
    typedef std::shared_ptr<Event> Ptr;

    enum class State {
        // Event has not been triggered or aborted.
        Pending,
        // Event has been aborted.
        Aborted
    };

    Event(PeerPtr peer);
    virtual ~Event() {}

    virtual void Process() = 0;
    void Abort();
    bool isAborted();

   protected:
    // peer at which the event is supposed to take place
    PeerWeakPtr owner_;
    State state = State::Pending;
};

class GenAndBroadcastTxn : public Event {
   public:
    GenAndBroadcastTxn(PeerPtr owner);
    void Process() override;
};

class ReceiveAndForwardTxn : public Event {
   public:
    ReceiveAndForwardTxn(PeerPtr owner, std::shared_ptr<Blockchain::Txn> txn,
                         int sender_id);
    void Process() override;

   private:
    std::shared_ptr<Blockchain::Txn> txn_;
    // peer id
    int sender_id_;
};

class BroadcastMinedBlk : public Event {
   public:
    BroadcastMinedBlk(PeerPtr owner, std::shared_ptr<Block> blk);
    void Process() override;

   private:
    std::shared_ptr<Block> block_;
};

class ReceiveAndForwardBlk : public Event {
   public:
    ReceiveAndForwardBlk(PeerPtr owner, std::shared_ptr<Block> blk,
                         int sender_id);
    void Process() override;

   private:
    std::shared_ptr<Blockchain::Block> block_;
    // peer id
    int sender_id_;
};

#endif  // _SRC_EVENTS_H_
