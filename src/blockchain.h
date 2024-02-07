
#ifndef _SRC_BLOCKCHAIN_H_
#define _SRC_BLOCKCHAIN_H_

#include <memory>

#include "types.h"
#include "macros.h"

class Blockchain
{
public:
    struct Txn
    {
        int64 id;
        int payer_id;
        int payee_id;
    };

    struct Block
    {
        // parent block id
        int par_block_id;

        // time of arrival at the current peer
        double arrival_time;
    };

    static std::shared_ptr<Block> genesis_block;

    Blockchain(PeerPtr owner);

private:
private:
    DISALLOW_COPY_AND_ASSIGN(Blockchain)
};

#endif // _SRC_BLOCKCHAIN_H_