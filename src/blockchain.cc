
#include "blockchain.h"

#include "constants.h"
#include "debug.h"
#include "peer.h"
#include "simulator.h"

int Blockchain::Txn::counter = 1;
int Blockchain::Block::counter = 0;  // 0 for genesis block
bool Blockchain::is_genesis_block_initialized = false;
BlockPtr Blockchain::genesis_block = std::make_shared<Block>(-1);
BlockMetaDataPtr Blockchain::genesis_block_metadata_ =
    std::make_shared<BlockMetaData>(
        0 /* id */, 0 /* arrival_time */, true /* is_in_a_chain */,
        1 /* chain length */, 0 /* chain_gen_time */);
//-----------------------------------------------------------------------------

Blockchain::Blockchain(PeerPtr owner)
    : owner_(owner), leaf_blocks_(LeafBlocksComparator) {
    if (!is_genesis_block_initialized) {
        is_genesis_block_initialized = true;
        GET_SHARED_PTR(owner, owner_);
        int N = owner->GetNumPeersInNetwork();
        genesis_block_metadata_->balances = std::vector<int64>(N + 1, 0);
    }
    assert(genesis_block->id == 0);
    all_blocks_[0] = genesis_block;
    blocks_metadata_[0] = genesis_block_metadata_;
    leaf_blocks_.insert(0);
}

//-----------------------------------------------------------------------------

BlockPtr Blockchain::CreateValidBlock() {
    GET_SHARED_PTR(owner, owner_);
    assert(!leaf_blocks_.empty());
    auto par_block_id = *leaf_blocks_.begin();
    auto block = std::make_shared<Block>(par_block_id);
    auto &txns = block->txns;
    // add coinbase txn
    txns.emplace_back(-1, owner->id_, MINING_FEE);

    // add a subset of the transactions received so far and
    // not included in any blocks in the longest chain
    auto ok_txns = owner->received_txns_;
    assert(all_blocks_.find(par_block_id) != all_blocks_.end());
    auto parent = all_blocks_[par_block_id];
    auto current = parent;
    while (current->id != 0) {
        for (auto txn : current->txns) {
            if (ok_txns.find(txn.id) != ok_txns.end()) ok_txns.erase(txn.id);
        }
        assert(all_blocks_.find(current->par_block_id) != all_blocks_.end());
        current = all_blocks_[current->par_block_id];
    }

    // everytime you start mining, it is an oppurtunity to update balance
    // of the owner
    auto metadata = blocks_metadata_[par_block_id];
    assert((int)metadata->balances.size() >= owner->id_);
    owner->balance_ = metadata->balances[owner->id_];

    auto would_be_balances = metadata->balances;
    for (auto tt : ok_txns) {
        auto txn = tt.second;
        if (
            txn->payer_id != -1 && 
            would_be_balances[txn->payer_id] < txn->amount
        ) continue;
        would_be_balances[txn->payer_id] -= txn->amount;
        would_be_balances[txn->payee_id] += txn->amount;
        txns.push_back(*txn);
        if (block->Size() > MAX_BLK_SIZE) {
            txns.pop_back();
            break;
        }
    }

    return block;
}

//-----------------------------------------------------------------------------

bool Blockchain::IsLongest(int block_id) const {
    auto it = leaf_blocks_.begin();
    return *it == block_id;
}

//-----------------------------------------------------------------------------

bool Blockchain::Contains(int block_id) const {
    return all_blocks_.find(block_id) != all_blocks_.end();
}

//-----------------------------------------------------------------------------

void Blockchain::Notify(BlockPtr block) {
    assert(!Contains(block->id));
    all_blocks_[block->id] = block;

    // add metadata
    GET_SHARED_PTR(owner, owner_);
    GET_SHARED_PTR(sim, owner->sim_);
    auto now = sim->Now();
    blocks_metadata_[block->id] =
        std::make_shared<BlockMetaData>(block->id, now /* arrival time */);
}

//-----------------------------------------------------------------------------

void Blockchain::BlockAddedToAChain(BlockPtr block) {
    auto metadata = blocks_metadata_[block->id];
    auto parent_metadata = blocks_metadata_[block->par_block_id];
    metadata->is_in_a_chain = true;
    metadata->chain_length = parent_metadata->chain_length + 1;
    metadata->chain_gen_time =
        std::max(metadata->arrival_time, parent_metadata->chain_gen_time);
    metadata->balances = parent_metadata->balances;
    auto &balances = metadata->balances;
    for (auto txn : block->txns) {
        if (txn.payer_id != -1)  // coinbase txn
            assert((balances[txn.payer_id] -= txn.amount) >= 0);
        assert((balances[txn.payee_id] += txn.amount) >= 0);
    }
}

//-----------------------------------------------------------------------------

bool Blockchain::AddBlock(BlockPtr block) {
    auto par_block_id = block->par_block_id;
    if (!Contains(par_block_id)) {  // Parent block hasn't arrived yet
        blocks_buffer_[par_block_id].push_back(block);
        // can't really validate the block
        return true;  // benefit of doubt
    }
    auto par_block = all_blocks_[par_block_id];
    auto par_block_metadata = blocks_metadata_[par_block_id];
    if (!par_block_metadata->is_in_a_chain) {
        blocks_buffer_[par_block_id].push_back(block);
        return true;
    }
    if (!IsValid(block)) {  // ignore invalid blocks
        return false;
    }
    assert(!blocks_metadata_[block->id]->is_in_a_chain);
    // remove parent from leaf_blocks_ if present
    if (leaf_blocks_.find(par_block_id) != leaf_blocks_.end()) {
        leaf_blocks_.erase(par_block_id);
    }
    // add current block to leaf_blocks_
    BlockAddedToAChain(block);
    leaf_blocks_.insert(block->id);
    // add other relevant blocks to the chain (if present)
    for (auto ch_block : blocks_buffer_[block->id]) {
        AddBlock(ch_block);
    }
    return true;
}

//-----------------------------------------------------------------------------

bool Blockchain::IsValid(BlockPtr block) {
    auto par_metadata = blocks_metadata_[block->par_block_id];
    auto balances = par_metadata->balances;
    for (auto txn : block->txns) {
        if (txn.payer_id != -1)  // coinbase txn
            if ((balances[txn.payer_id] -= txn.amount) < 0) return false;
        if ((balances[txn.payee_id] += txn.amount) < 0) return false;
    }
    return true;
}

//-----------------------------------------------------------------------------

int Blockchain::Size() { return all_blocks_.size(); }

//-----------------------------------------------------------------------------

void Blockchain::ExportToFile() {
    // TODO
    GET_SHARED_PTR(owner, owner_)
    string fname = "blockchains/" + std::to_string(owner->id_) + ".dot"; 
    ofstream file(fname);
    file << "digraph D { node [ordering=out] rankdir=\"LR\";\n";
    
    auto latest_block_id = *leaf_blocks_.begin();
    assert(all_blocks_.find(latest_block_id) != all_blocks_.end());
    auto current = all_blocks_[latest_block_id];
    while (current->id != 0) {
        file << current->id << " [fillcolor=lightgreen, style=filled]\n";
        assert(all_blocks_.find(current->par_block_id) != all_blocks_.end());
        current = all_blocks_[current->par_block_id];
    }
    file << "0 [fillcolor=lightgreen, style=filled]\n";

    for(auto& p : all_blocks_){
        auto& block = p.second;
        if(block->par_block_id == -1) continue;
        if(!blocks_metadata_[block->id]->is_in_a_chain) continue;
        file << block->id << " [shape=box]\n";
        file << block->par_block_id << " -> " << block->id << "\n";
    }

    file << "\n}";
    file.close();
}

//-----------------------------------------------------------------------------