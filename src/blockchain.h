
#ifndef _SRC_BLOCKCHAIN_H_
#define _SRC_BLOCKCHAIN_H_

#include <memory>
#include <vector>

#include "constants.h"
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
        // with payer_id = 0 (all peer IDs start
        // from 1) and payee_id corresponds to the miner
        Txn(int payer_id, int payee_id, int64 amount)
            : id(counter++),
              payer_id(payer_id),
              payee_id(payee_id),
              amount(amount) {}
    };
    struct Block {
        static int counter;
        int id;
        // parent block id
        int par_block_id;
        // Note: parent is initialized after construction of the object
        std::weak_ptr<Block> parent;
        // time of arrival at the current peer.
        // If its an own block, then the time after which PoW is done
        // so It is essentially the time at which it is added to the chain
        double arrival_time;
        // list of all transactions included in this block
        std::vector<Txn> txns;
        // Filled after including the block in blockchain
        std::vector<int64> balances;
        bool is_in_a_chain;
        Block(int par_block_id)
            : id(counter++), par_block_id(par_block_id), is_in_a_chain(false) {}
        // returns size of the block in bits
        int64 Size() { return txns.size() * _1KB; }
    };
    struct LeafBlockMetaData {
        int block_id;
        int chain_length;
        // time at which the chain is generated
        // i.e max arrival time over all the blocks in the chain
        double chain_gen_time;
        LeafBlockMetaData(int block_id, int chain_length, double gen_time)
            : block_id(block_id),
              chain_length(chain_length),
              chain_gen_time(gen_time) {}
        bool operator<(const LeafBlockMetaData& other) {
            if (chain_length == other.chain_length)
                return chain_gen_time < other.chain_gen_time;
            return chain_length > other.chain_length;
        }
    };
    typedef std::shared_ptr<Block> BlockPtr;

    static BlockPtr genesis_block;
    static bool is_genesis_block_initialized;

    Blockchain(PeerPtr owner);
    DISALLOW_COPY_AND_ASSIGN(Blockchain)

    BlockPtr CreateValidBlock();
    // return true if the longest chain in the blockchain ends at the given
    // block. Longest chain ties are broken using arrival_time
    bool IsLongest(BlockPtr block);
    bool Contains(int block_id);
    // If parent is present:
    //  adds it to the blockchain if parent is present else adds it to the
    // block buffer
    void AddBlock(BlockPtr block);

    typedef std::vector<BlockPtr> Blocks;

   private:
    PeerWeakPtr owner_;
    // contains all the blocks received
    // Indexed by block id
    hash_table<int, BlockPtr> all_blocks_;
    // blocks_buffer_[id] contains all the blocks whose parent(id) isn't
    // included in the blockchain yet
    // Indexed by parent id
    hash_table<int, Blocks> blocks_buffer_;
    std::set<LeafBlockMetaData> leaf_blocks_;
};

typedef Blockchain::Txn Txn;
typedef Blockchain::Block Block;
typedef std::shared_ptr<Block> BlockPtr;

#endif  // _SRC_BLOCKCHAIN_H_