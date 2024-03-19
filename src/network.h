
#ifndef _SRC_NETWORK_H_
#define _SRC_NETWORK_H_

#include <memory>
#include <vector>

#include "macros.h"
#include "types.h"

class Network {
   public:
    typedef std::shared_ptr<Network> Ptr;

    struct NetworkParams {
        // No of. peers in the network
        int num_peers;

        // No of. slow peers in the network
        int num_slow_peers;

        // No of. peers with low processing power
        int num_low_cpu;

        // fracition of hashing power with each selfish miner
        double hashPower_self1;
        double hashPower_self2;
        
        // dij is randomly chosen from an exponential distribution
        // with mean (T_dij kbits)/c_ij
        double T_dij;
    };

    struct Link {
        PeerWeakPtr to;

        // latency related fields
        static RealUniformDistr prop_delay_distr;
        static double propagation_delay;
        static double T_dij;
        // in bits/sec
        double link_speed;
        Link(PeerPtr p, bool is_fast);
        // link latency (in seconds) - to be called for every message that's put
        // on the link
        // @param message_length: in bits
        double latency(int64 message_length /* in bits */);
    };

    Network(std::shared_ptr<NetworkParams> params, SimulatorPtr sim);
    DISALLOW_COPY_AND_ASSIGN(Network)

    void Init();
    void End();

    typedef std::vector<PeerPtr> Peers;
    typedef std::vector<Link> Links;
    typedef std::vector<std::vector<int>> Graph;

    friend class Peer;
    friend class SelfishPeer;

   private:
    Graph GenerateRandomGraph(int N);
    bool IsConnected(const Graph &graph);

   private:
    std::shared_ptr<NetworkParams> params_;

    // list of peers (i.e nodes) in the network
    Peers peers_;

    SimulatorWeakPtr sim_;
};

#endif  // _SRC_NETWORK_H_