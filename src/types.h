
#ifndef _SRC_TYPES_H_
#define _SRC_TYPES_H_

#include <stdint.h>

#include <chrono>
#include <ext/pb_ds/assoc_container.hpp>  // Common file
#include <ext/pb_ds/tree_policy.hpp>  // Including tree_order_statistics_node_update
#include <memory>
#include <random>
#include <unordered_set>

#include "macros.h"

typedef int64_t int64;
typedef int32_t int32;
typedef int8_t int8;
typedef std::exponential_distribution<double> RealExpDistr;
typedef std::uniform_real_distribution<double> RealUniformDistr;
typedef std::uniform_int_distribution<int> IntUniformDistr;

class Simulator;
class Network;
class Peer;
class Event;
class Blockchain;

DEFINE_PTR_TYPES(Simulator)
DEFINE_PTR_TYPES(Network)
DEFINE_PTR_TYPES(Peer)
DEFINE_PTR_TYPES(Event)
DEFINE_PTR_TYPES(Blockchain)

struct chash {
    static uint64_t splitmix64(uint64_t x) {
        x += 0x9e3779b97f4a7c15;
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
        x = (x ^ (x >> 27)) * 0x94d049bb133111eb;
        return x ^ (x >> 31);
    }

    size_t operator()(uint64_t x) const {
        static const uint64_t FIXED_RANDOM =
            std::chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x + FIXED_RANDOM);
    }
    size_t operator()(std::pair<uint64_t, uint64_t> x) const {
        static const uint64_t FIXED_RANDOM =
            std::chrono::steady_clock::now().time_since_epoch().count();
        return splitmix64(x.first + FIXED_RANDOM) ^
               (splitmix64(x.second + FIXED_RANDOM) >>
                1);  // (or can use 3*F + S as well)
    }
};

template <typename T>
using hash_set = std::unordered_set<T, chash>;

template <typename K, typename V>
using hash_table = __gnu_pbds::gp_hash_table<K, V, chash>;

template <typename T>
using hash_multiset = std::unordered_multiset<T, chash>;

#define ALL(a) a.begin(), a.end()
#define ALLR(a) a.rbegin(), a.rend()
#define random_shuffle(V) std::shuffle(ALL(V), Simulator::rng)

#endif  // _SRC_TYPES_H_
