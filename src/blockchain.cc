
#include "blockchain.h"

#include "peer.h"

int Blockchain::Txn::counter = 1;
int Blockchain::Block::counter = 0;  // 0 for genesis block
bool is_genesis_block_initialized = false;
BlockPtr Blockchain::genesis_block = std::make_shared<Block>(-1);

//-----------------------------------------------------------------------------

Blockchain::Blockchain(PeerPtr owner) : owner_(owner) {
    if (!is_genesis_block_initialized) {
        is_genesis_block_initialized = true;
        genesis_block->arrival_time = 0;
        GET_SHARED_PTR(owner, owner_);
        int N = owner->GetNumPeersInNetwork();
        genesis_block->balances = std::vector<int64>(N, 0);
    }
    assert(genesis_block->id == 0);
    all_blocks_[0] = genesis_block;
    leaf_blocks_.emplace(genesis_block, 1, 0);
}

//-----------------------------------------------------------------------------

BlockPtr Blockchain::CreateValidBlock() {}

//-----------------------------------------------------------------------------

bool Blockchain::IsLongest(BlockPtr block) { auto it = leaf_blocks_.begin(); }

//-----------------------------------------------------------------------------

void Blockchain::AddBlock(BlockPtr block) {}

//-----------------------------------------------------------------------------
