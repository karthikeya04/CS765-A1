
#ifndef _SRC_PEER_H_
#define _SRC_PEER_H_

#include <memory>

#include "blockchain.h"
#include "events.h"
#include "network.h"
#include "types.h"

using Link = Network::Link;
using LinkPtr = std::shared_ptr<Link>;
using Links = std::vector<LinkPtr>;

class Peer : public std::enable_shared_from_this<Peer> {
   public:
    typedef std::shared_ptr<Peer> Ptr;

    Peer(int id, bool is_low_cpu, bool is_slow_peer, double hashing_power,
         SimulatorPtr sim);

    DISALLOW_COPY_AND_ASSIGN(Peer)

    friend class GenAndBroadcastTxn;
    friend class ReceiveAndForwardTxn;
    friend class BroadcastMinedBlk;
    friend class ReceiveAndForwardBlk;
    friend class Blockchain;
    friend class Network;

    // called once the peer is set up in the network
    void Init();
    void AddLink(LinkPtr link);
    bool IsLowCpu() const;
    bool IsSlow() const;

   protected:
    void BroadcastToNeighbours(std::shared_ptr<Block> blk); // helper fn
    // Event Ops
    void GenAndBroadcastTxnOp();
    void ReceiveAndForwardTxnOp(std::shared_ptr<Txn> txn, int sender_id);
    virtual void BroadcastMinedBlkOp(std::shared_ptr<Block> blk);
    virtual void ReceiveAndForwardBlkOp(std::shared_ptr<Block> blk, int sender_id);

    void ScheduleTxnGen();
    virtual void StartMining();
    int GetNumPeersInNetwork();

   protected:
    int id_;
    Links links_;
    bool is_low_cpu_;
    bool is_slow_peer_;
    double hashing_power_;
    bool is_initialized_;
    int64 balance_;
    hash_table<int64, std::shared_ptr<Txn>> received_txns_;
    BlockchainPtr blockchain_;
    std::shared_ptr<BroadcastMinedBlk> current_mining_event_;
    // simulator handler to schedule events
    SimulatorWeakPtr sim_;
    // Network handler
    NetworkWeakPtr net_;
    RealExpDistr txn_gen_distr_;
};

class SelfishPeer : public Peer {
   public: 
    using Peer::Peer;
    friend class GenAndBroadcastTxn;
    friend class ReceiveAndForwardTxn;
    friend class BroadcastMinedBlk;
    friend class ReceiveAndForwardBlk;
    friend class Blockchain;
    friend class Network;

   private: 
    void BroadcastMinedBlkOp(std::shared_ptr<Block> blk);
    void ReceiveAndForwardBlkOp(std::shared_ptr<Block> blk, int sender_id);
    void StartMining();
    
   private: 
    int state_ = 0; // (0' is -1)
    deque<BlockPtr> secret_chain_;
   public: 
    const deque<BlockPtr>& ShowSecretChain(){return secret_chain_;} // for visualisation
};

#endif  // _SRC_PEER_H_