
#ifndef _SRC_BLOCKCHAIN_H_
#define _SRC_BLOCKCHAIN_H_

#include <memory>

#include "types.h"
#include "peer.h"
#include "macros.h"

class Blockchain {
 public:
    struct Txn {
        uint64 id;
        uint64 payer_id;
        uint64 payee_id;
    };

    struct Block {
        // parent block id
        uint32 par_block_id;

        // time of arrival at the current peer
        double arrival_time;
    };

    static std::shared_ptr<Block> genesis_block;
    
    Blockchain(Peer::Ptr owner);
 private:

 private:
    DISALLOW_COPY_AND_ASSIGN(Blockchain)
};

#endif // _SRC_BLOCKCHAIN_H_