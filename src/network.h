
#ifndef _SRC_NETWORK_H_
#define _SRC_NETWORK_H_

#include <memory>
#include <vector>

#include "types.h"
#include "macros.h"
#include "peer.h"

class Network
{
public:
    typedef std::shared_ptr<Network> Ptr;

    struct NetworkParams
    {

        // No of. peers in the network
        int num_peers;

        // No of. slow peers in the network
        int num_slow_peers;

        // No of. peers with low processing power
        int num_low_cpu;
    };

    struct Link
    {
        Peer::Ptr p;
        double latency;
    };

    Network(std::shared_ptr<NetworkParams> params,
            SimulatorPtr sim);

    typedef std::vector<PeerPtr> Peers;
    typedef std::vector<Link> Links;
    typedef std::vector<Links> Network_t;
    typedef std::vector<std::vector<int>> Graph;

private:
    Graph GenerateRandomGraph(int N);
    bool IsConnected(const Graph &graph);
    void AddEventsAtPeer(PeerPtr peer);

private:
    std::shared_ptr<NetworkParams> params_;

    // list of peers (i.e nodes) in the network
    Peers peers_;

    // net_[u] contains all the links originated from u
    Network_t net_;

    SimulatorWeakPtr sim_;

private:
    DISALLOW_COPY_AND_ASSIGN(Network)
};

#endif // _SRC_NETWORK_H_