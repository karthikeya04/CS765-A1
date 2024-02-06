
#ifndef _SRC_PEER_H_
#define _SRC_PEER_H_

#include <memory>

#include "types.h"
#include "simulator.h"

// Forward declarations
class BlockChain;

class Peer {
 public:
    typedef std::shared_ptr<Peer> Ptr;
    typedef std::weak_ptr<Peer> WeakPtr;

    typedef uint32 Id;

    Peer(Simulator::Ptr sim);
    DISALLOW_COPY_AND_ASSIGN(Peer)

    friend GenAndBroadcastTxn;
    friend ReceiveAndForwardTxn;
    friend BroadcastMinedBlk;
    friend ReceiveAndForwardBlk;
 private:
    // Event Ops
    void GenAndBroadcastTxnOp();
    void ReceiveAndForwardTxnOp(std::shared_ptr<Blockchain::Txn> txn);
    void BroadcastMinedBlkOp();
    void ReceiveAndForwardBlkOp(std::shared_ptr<Blockchain::Block> blk);

 private:
    Id id_;
    Simulator::Ptr sim_;
    RealExpDistr txn_gen_distr_;

};

#endif // _SRC_PEER_H_