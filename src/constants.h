
#ifndef _SRC_CONSTANTS_H_
#define _SRC_CONSTANTS_H_

// in bits
#define _1Kb 1000
#define _1KB 8 * _1Kb
#define _1Mb 1000 * _1Kb
#define _1MB 8 * _1Mb

// txn, blk related
#define MAX_BLK_SIZE _1MB
#define EMPTY_BLK_SIZE _1KB
#define TXN_SIZE _1KB
#define MAX_TXNS_IN_A_BLK (MAX_BLK_SIZE / TXN_SIZE)
#define MINING_FEE 50

// network related
#define MIN_NEIGHBOURS 3
#define MAX_NEIGHBOURS 6

// in seconds
#define MIN_PROPAGATION_DELAY 0.01
#define MAX_PROPAGATION_DELAY 0.5

#define FAST_LINK_SPEED (100 * _1Mb)  // in Mbps
#define SLOW_LINK_SPEED (5 * _1Mb)    // in Mbps

#endif  // _SRC_CONSTANTS_H_
