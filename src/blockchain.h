
#ifndef _SRC_BLOCKCHAIN_H_
#define _SRC_BLOCKCHAIN_H_

#include <climits>
#include <functional>
#include <memory>
#include <vector>

#include "constants.h"
#include "debug.h"
#include "macros.h"
#include "types.h"

class Blockchain {
   public:
    struct Txn {
        static int counter;
        int id;
        int payer_id;
        int payee_id;
        int64 amount;
        // For coinbase txn, the same struct is used
        // with payer_id = -1 (all peer IDs start
        // from 1) and payee_id corresponds to the miner
        Txn(int payer_id, int payee_id, int64 amount)
            : id(counter++),
              payer_id(payer_id),
              payee_id(payee_id),
              amount(amount) {}
        void display() {
            cout << "TxnID(" << id << "): " << payer_id << " pays " << payee_id
                 << " " << amount << " coins\n";
        }
    };
    // used to transfer blocks in the network
    // So don't store any peer related metadata here
    struct Block {
        static int counter;
        int id;
        // parent block id
        int par_block_id;
        // list of all transactions included in this block
        std::vector<Txn> txns;

        Block(int par_block_id) : id(counter++), par_block_id(par_block_id) {}
        // returns size of the block in bits
        int64 Size() { return txns.size() * _1KB; }
    };
    struct BlockMetaData {  // private to the blockchain

        /* block specific fields */
        int block_id;
        // time of arrival at the current peer.
        // If its an own block, then the time after which PoW is done
        // so It is essentially the time at which it is added to the chain
        double arrival_time;

        /* chain related fields: Filled only after including the block in a
         * chain */
        bool is_in_a_chain;
        int chain_length;
        // time at which the chain is generated
        // i.e max arrival time over all the blocks in the chain
        double chain_gen_time;
        // balances until this block
        std::vector<int64> balances;

        BlockMetaData(int block_id, double arrival_time,
                      bool is_in_a_chain = false, int chain_length = 0,
                      double chain_gen_time = INT_MAX)
            : block_id(block_id),
              arrival_time(arrival_time),
              is_in_a_chain(is_in_a_chain),
              chain_length(chain_length),
              chain_gen_time(chain_gen_time) {}
        bool operator<(const BlockMetaData& other) {
            if (chain_length == other.chain_length)
                return chain_gen_time < other.chain_gen_time;
            return chain_length > other.chain_length;
        }
    };
    typedef std::shared_ptr<Block> BlockPtr;
    typedef std::shared_ptr<BlockMetaData> BlockMetaDataPtr;

    static BlockPtr genesis_block;
    static BlockMetaDataPtr genesis_block_metadata_;
    static bool is_genesis_block_initialized;

    Blockchain(PeerPtr owner);
    DISALLOW_COPY_AND_ASSIGN(Blockchain)

    BlockPtr CreateValidBlock();

    // longest visible chain's latest block_id / length
    int GetLVC(); // blk_id
    int GetLVClength();

    // return true if the longest chain in the blockchain ends at the given
    // block. Longest chain ties are broken using chain_gen_time
    bool IsLongest(int block_id) const;

    // returns true if the blockchain has recevied a block with the given id
    // Note: true doesn't necessarily mean it is in a chain
    bool Contains(int block_id) const;

    // If parent is present add it to the blockchain(and add waiting blocks from
    // the buffer) else adds it to the block buffer
    // Assumes blockchain is already notified about the block
    // returns false if the block is invalid for sure
    bool AddBlock(BlockPtr block);

    // Returns false if the block is invalid 
    bool IsValid(BlockPtr block);

    // Peer is supposed to call this to notify the blockchain about the newly
    // received block(generated block), so that it can set up metadata (like
    // arrival time etc)
    void Notify(BlockPtr block);

    typedef std::vector<BlockPtr> Blocks;
    int Size();
    void ExportToFile(int num_peers, const deque<BlockPtr>& secret_chain);

   private:
    // callback to be called after adding a block to any chain (to update chain
    // specific block metadata)
    void BlockAddedToAChain(BlockPtr block);

   private:
    PeerWeakPtr owner_;
    // contains all the blocks received
    // Indexed by block id
    hash_table<int, BlockPtr> all_blocks_;
    hash_table<int, BlockMetaDataPtr> blocks_metadata_;
    // blocks_buffer_[id] contains all the blocks whose parent(id) isn't
    // included in the blockchain yet
    // Indexed by parent id
    hash_table<int, Blocks> blocks_buffer_;

    // comparator to impose strict weak ordering in leaf_blocks_ set
    std::function<bool(int, int)> LeafBlocksComparator = [this](int blk_id1,
                                                                int blk_id2) {
        auto metadata1 = blocks_metadata_[blk_id1];
        auto metadata2 = blocks_metadata_[blk_id2];
        return (*metadata1 < *metadata2);
    };

    std::set<int, decltype(LeafBlocksComparator)> leaf_blocks_;
};

typedef Blockchain::Txn Txn;
typedef Blockchain::Block Block;
typedef Blockchain::BlockMetaData BlockMetaData;
typedef std::shared_ptr<Block> BlockPtr;
typedef std::weak_ptr<Block> BlockWeakPtr;
typedef std::shared_ptr<BlockMetaData> BlockMetaDataPtr;

#endif  // _SRC_BLOCKCHAIN_H_