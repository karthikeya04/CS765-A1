
#include <algorithm>
#include <random>
#include <boost/graph/random.hpp>

#include "network.h"
#include "types.h"
#include "constants.h"
#include "simulator.h"
#include "debug.h"

Network::Network(std::shared_ptr<NetworkParams> params,
                 SimulatorPtr sim)
    : params_(params),
      net_(Network_t(params_->num_peers)), sim_(sim)
{
    auto graph = GenerateRandomGraph(params_->num_peers);
}

Network::Graph Network::GenerateRandomGraph(int N)
{
    Graph graph(N);

    // used for sampling degree of each node
    IntUniformDistr deg_distr(MIN_NEIGHBOURS, MAX_NEIGHBOURS);
    std::vector<int> degree(N);

    auto add_edge = [&graph](int u, int v)
    {
        graph[u].push_back(v);
        graph[v].push_back(u);
    };
    auto is_valid_deg_seq = [](std::vector<int> &degree)
    {
        int64 pref = 0;
        int N = degree.size();
        std::sort(allr(degree));
        for (int i = 1; i <= N; i++)
        {
            pref += degree[i - 1];
            int64 suff = 0;
            for (int j = i + 1; j <= N; j++)
                suff += std::min(degree[j - 1], i);
            if (pref > i * (i - 1) + suff)
                return false;
        }
        return true;
    };
    do
    {
        for (int i = 0; i < N; i++)
        {
            degree[i] = deg_distr(Simulator::rng);
        }
    } while (!is_valid_deg_seq(degree));

    random_shuffle(degree);
    debug(degree);

    // fast enough for 5k #peers
    while (true)
    {
        for (int i = 0; i < N; i++)
            graph[i].clear();
        std::vector<int> stubs;
        stubs.reserve(MAX_NEIGHBOURS * N);
        for (int u = 0; u < N; u++)
        {
            int d = degree[u];
            while (d--)
                stubs.push_back(u);
        }
        random_shuffle(stubs);
        int ssize = stubs.size();
        std::vector<bool> visited(ssize, false);
        std::vector<hash_set<int>> connected(N);
        int total_visited = 0;
        for (int i = 0; i < ssize - 1; i++)
        {
            if (visited[i])
                continue;
            total_visited++;
            int u = stubs[i];
            for (int j = i + 1; j < ssize; j++)
            {
                int v = stubs[j];
                if (u != v /* no self loops */ &&
                    connected[u].count(v) == 0 /* no multi-edges */)
                {
                    connected[u].insert(v);
                    add_edge(u, v);
                    visited[j] = 1;
                    total_visited++;
                    break;
                }
            }
        }
        if (total_visited == ssize)
            break;
    }
    debug_graph(graph);
    return graph;
}

bool Network::IsConnected(const Graph &graph)
{
    const int n = graph.size();
    std::vector<bool> visited(n, false);
    std::function<void(int)> dfs = [&](int u)
    {
        visited[u] = true;
        for (int v : graph[u])
        {
            if (!visited[v])
                dfs(v);
        }
    };
    dfs(0);
    bool is_connected = true;
    for (int i = 0; i < n; i++)
    {
        is_connected &= visited[i];
    }
    return is_connected;
}