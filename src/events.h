
#ifndef _SRC_EVENTS_H_
#define _SRC_EVENTS_H_

#include <memory>
#include <functional>

#include "types.h"
#include "simulator.h"
#include "peer.h"
#include "blockchain.h"

// Abstract Event class
class Event
{
public:
   typedef std::shared_ptr<Event> Ptr;

   enum class State
   {
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

   Event(PeerPtr peer);

   virtual ~Event() {}

   virtual void Process() = 0;

protected:
   PeerWeakPtr owner_;
   State state = State::Pending;
   // Type type;
};

class GenAndBroadcastTxn : public Event
{
public:
   GenAndBroadcastTxn(PeerPtr peer);
   void Process() override;
};

class ReceiveAndForwardTxn : public Event
{
public:
   ReceiveAndForwardTxn(PeerPtr peer, std::shared_ptr<Blockchain::Txn> txn);
   void Process() override;

private:
   std::shared_ptr<Blockchain::Txn> txn_;
};

class BroadcastMinedBlk : public Event
{
public:
   BroadcastMinedBlk(PeerPtr peer);
   void Process() override;
};

class ReceiveAndForwardBlk : public Event
{
public:
   ReceiveAndForwardBlk(PeerPtr peer, std::shared_ptr<Blockchain::Block> blk);
   void Process() override;

private:
   std::shared_ptr<Blockchain::Block> block_;
};

#endif // _SRC_EVENTS_H_
