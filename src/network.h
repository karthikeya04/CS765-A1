
#ifndef _SRC_NETWORK_H_
#define _SRC_NETWORK_H_

#include <memory>
#include <vector>

#include "types.h"
#include "macros.h"

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
        PeerWeakPtr to;

        // latency related fields

        static RealUniformDistr prop_delay_distr;
        static double propagation_delay;
        // in bits/sec
        double link_speed;
        RealExpDistr queuing_delay_distr;

        Link(PeerPtr p, bool is_fast);
        // link latency (in seconds) - to be called for every message
        double latency(int64 message_length /* in bits */);
    };

    Network(std::shared_ptr<NetworkParams> params,
            SimulatorPtr sim);

    DISALLOW_COPY_AND_ASSIGN(Network)

    typedef std::vector<PeerPtr> Peers;
    typedef std::vector<Link> Links;
    typedef std::vector<std::vector<int>> Graph;

private:
    Graph GenerateRandomGraph(int N);
    bool IsConnected(const Graph &graph);
    void AddEventsAtPeer(PeerPtr peer);

private:
    std::shared_ptr<NetworkParams> params_;

    // list of peers (i.e nodes) in the network
    Peers peers_;

    SimulatorWeakPtr sim_;
};

#endif // _SRC_NETWORK_H_