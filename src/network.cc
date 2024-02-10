
#include "network.h"

#include <algorithm>
#include <boost/graph/random.hpp>
#include <random>

#include "constants.h"
#include "debug.h"
#include "peer.h"
#include "simulator.h"
#include "types.h"

//-----------------------------------------------------------------------------

Network::Network(std::shared_ptr<NetworkParams> params, SimulatorPtr sim)
    : params_(std::move(params)), sim_(sim) {}

//-----------------------------------------------------------------------------

void Network::Init() {
    const int N = params_->num_peers;
    GET_SHARED_PTR(sim, sim_);
    // randomly assign peers to
    // low_cpu/slow_peer
    std::vector<bool> is_low_cpu(N, false), is_slow_peer(N, false);
    std::vector<int> permutation(N);
    std::iota(ALL(permutation), 0);
    random_shuffle(permutation);
    assert(params_->num_slow_peers <= N);
    assert(params_->num_low_cpu <= N);
    for (int i = 0; i < params_->num_low_cpu; i++) {
        is_low_cpu[permutation[i]] = true;
    }
    random_shuffle(permutation);
    for (int i = 0; i < params_->num_slow_peers; i++) {
        is_slow_peer[permutation[i]] = true;
    }
    auto num_low_cpu = params_->num_low_cpu;
    auto num_high_cpu = N - num_low_cpu;
    double slow_peer_hashing_power = 1.0 / (10 * num_high_cpu + num_low_cpu);

    // initialize peers
    peers_.resize(N);
    for (int i = 0; i < N; i++) {
        double hashing_power = slow_peer_hashing_power;
        if (!is_low_cpu[i]) hashing_power *= 10;
        peers_[i] =
            make_shared<Peer>(i + 1 /* IDs start from 1*/, is_low_cpu[i],
                              is_slow_peer[i], hashing_power, sim);
    }

    // generate random simple graph with
    // each peer having neighbours
    // ranging from 3 to 6
    auto graph = GenerateRandomGraph(N);

    for (int u = 0; u < N; u++) {
        for (int v : graph[u]) {
            bool is_fast = (!peers_[u]->IsSlow() && !peers_[v]->IsSlow());
            auto link = std::make_shared<Link>(peers_[v], is_fast);
            peers_[u]->AddLink(link);
        }
    }
    // start events at peers
    for (int i = 0; i < N; i++) {
        peers_[i]->Init();
    }
}

//-----------------------------------------------------------------------------

Network::Graph Network::GenerateRandomGraph(int N) {
    Graph graph(N);

    // used for sampling degree of each
    // node
    IntUniformDistr deg_distr(MIN_NEIGHBOURS, MAX_NEIGHBOURS);
    std::vector<int> degree(N);

    auto add_edge = [&graph](int u, int v) {
        graph[u].push_back(v);
        graph[v].push_back(u);
    };
    auto is_valid_deg_seq = [](std::vector<int> &degree) {
        int64 pref = 0;
        int N = degree.size();
        std::sort(ALLR(degree));
        for (int i = 1; i <= N; i++) {
            pref += degree[i - 1];
            int64 suff = 0;
            for (int j = i + 1; j <= N; j++) suff += std::min(degree[j - 1], i);
            if (pref > i * (i - 1) + suff) return false;
        }
        return true;
    };
    do {
        for (int i = 0; i < N; i++) {
            degree[i] = deg_distr(Simulator::rng);
        }
    } while (!is_valid_deg_seq(degree));

    random_shuffle(degree);
    debug(degree);

    // fast enough for 5k #peers
    while (true) {
        for (int i = 0; i < N; i++) graph[i].clear();
        std::vector<int> stubs;
        stubs.reserve(MAX_NEIGHBOURS * N);
        for (int u = 0; u < N; u++) {
            int d = degree[u];
            while (d--) stubs.push_back(u);
        }
        random_shuffle(stubs);
        int ssize = stubs.size();
        std::vector<bool> visited(ssize, false);
        std::vector<hash_set<int>> connected(N);
        int total_visited = 0;
        for (int i = 0; i < ssize - 1; i++) {
            if (visited[i]) continue;
            total_visited++;
            int u = stubs[i];
            for (int j = i + 1; j < ssize; j++) {
                int v = stubs[j];
                if (u != v /* no self
                              loops */
                    && connected[u].count(v) == 0 /* no multi-edges */) {
                    connected[u].insert(v);
                    add_edge(u, v);
                    visited[j] = 1;
                    total_visited++;
                    break;
                }
            }
        }
        if (total_visited == ssize) break;
    }
    debug_graph(graph);
    return graph;
}

//-----------------------------------------------------------------------------

bool Network::IsConnected(const Graph &graph) {
    const int n = graph.size();
    std::vector<bool> visited(n, false);
    std::function<void(int)> dfs = [&](int u) {
        visited[u] = true;
        for (int v : graph[u]) {
            if (!visited[v]) dfs(v);
        }
    };
    dfs(0);
    bool is_connected = true;
    for (int i = 0; i < n; i++) {
        is_connected &= visited[i];
    }
    return is_connected;
}

//-----------------------------------------------------------------------------

// Network::Link

RealUniformDistr Network::Link::prop_delay_distr(MIN_PROPAGATION_DELAY,
                                                 MAX_PROPAGATION_DELAY);
double Network::Link::propagation_delay =
    Network::Link::prop_delay_distr(Simulator::rng);

Network::Link::Link(PeerPtr to, bool is_fast)
    : to(to), link_speed(is_fast ? FAST_LINK_SPEED : SLOW_LINK_SPEED) {}

double Network::Link::latency(int64 message_length) {
    RealExpDistr queuing_delay_distr(link_speed / 96000);
    double queuing_delay = queuing_delay_distr(Simulator::rng);
    return (Network::Link::propagation_delay + message_length / link_speed +
            queuing_delay);
}

//-----------------------------------------------------------------------------

void Network::End() {
    // blockchains at all the peers must be same (maybe do a check?)
    auto blockchain = peers_[0]->blockchain_;
    blockchain->ExportToFile();
    // TODO: export network and blockchain data to files for analysis
}

//-----------------------------------------------------------------------------
