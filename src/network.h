
#ifndef _SRC_NETWORK_H_
#define _SRC_NETWORK_H_

#include <memory>
#include <vector>

#include "types.h"
#include "macros.h"

// Forward declarations
class Peer;

class Network {
 public:
    typedef std::shared_ptr<Network> Ptr;
    typedef std::weak_ptr<Network> WeakPtr;

    struct NetworkParams {

        // No of. peers in the network
        int num_peers;

        // No of. slow peers in the network
        int num_slow_peers;

        // No of. peers with low processing power
        int num_low_cpu;
    };

    struct Link {
        uint32 u;
        double latency;
    };

    Network(NetworkParams params);

    void GenerateRandomGraph();

    typedef std::vector<Peer> Peers;
    typedef std::vector<Link> Graph;

 private:
    NetworkParams params_;

    // list of peers (i.e nodes) in the network
    Peers peers_;

    // graph_[u] contains all the links originated from u
    Graph graph_;

    // static variables
 private:
    static std::mt19937_64 rng;

 private:
    DISALLOW_COPY_AND_ASSIGN(Network)
};

#endif // _SRC_NETWORK_H_