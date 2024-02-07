
#ifndef _SRC_PEER_H_
#define _SRC_PEER_H_

#include <memory>

#include "types.h"
#include "blockchain.h"

class Simulator;

class Peer
{
public:
   typedef std::shared_ptr<Peer> Ptr;

   Peer(int id, bool is_low_cpu, bool is_slow_peer, SimulatorPtr sim);
   DISALLOW_COPY_AND_ASSIGN(Peer)

   friend class GenAndBroadcastTxn;
   friend class ReceiveAndForwardTxn;
   friend class BroadcastMinedBlk;
   friend class ReceiveAndForwardBlk;

private:
   // Event Ops
   void GenAndBroadcastTxnOp();
   void ReceiveAndForwardTxnOp(std::shared_ptr<Blockchain::Txn> txn);
   void BroadcastMinedBlkOp();
   void ReceiveAndForwardBlkOp(std::shared_ptr<Blockchain::Block> blk);

   bool IsLowCpu() const;
   bool isSlowPeer() const;

private:
   int id_;
   SimulatorPtr sim_;
   RealExpDistr txn_gen_distr_;
   bool is_low_cpu_;
   bool is_slow_peer_;
};

#endif // _SRC_PEER_H_