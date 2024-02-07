#ifdef DEBUG_MODE
#ifndef DEBUG_H
#define DEBUG_H

#include <bits/stdc++.h>
using namespace std;

#define debug(...) \
    cerr << endl;  \
    cerr << "[ " << #__VA_ARGS__ << " ]:", debug_out(__VA_ARGS__);

void __print(int x) { cerr << x; }
void __print(size_t x) { cerr << x; }
#ifdef LEETCODE
void __print(long long x) { cout << x; }
#endif
void __print(float x) { cerr << x; }
void __print(double x) { cerr << x; }
void __print(long double x) { cerr << x; }
void __print(char x) { cerr << '\'' << x << '\''; }
void __print(const char *x) { cerr << '\"' << x << '\"'; }
void __print(const string &x) { cerr << '\"' << x << '\"'; }
void __print(bool x) { cerr << (x ? "true" : "false"); }

template <class T>
void __print(vector<T>);
// template<typename T, typename V> void __print(const pair<T, V>);
template <class T>
void __print(set<T>);
template <class T>
void __print(multiset<T>);
template <class T, class V>
void __print(map<T, V>);

template <typename T, typename V>
void __print(const pair<T, V> &x)
{
    cerr << '{';
    __print(x.first);
    cerr << ',';
    __print(x.second);
    cerr << '}';
}
template <class T>
void __print(deque<T> v)
{
    cerr << "[ ";
    for (T i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]";
}
template <class T>
void __print(vector<T> v)
{
    cerr << "[ ";
    for (T i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]\n";
}
template <class T>
void __print(set<T> v)
{
    cerr << " [ ";
    for (T i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]";
}
template <class T>
void __print(multiset<T> v)
{
    cerr << " [ ";
    for (T i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]";
}
template <class T, class V>
void __print(map<T, V> v)
{
    cerr << " [ ";
    for (auto i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]";
}
template <class T, class V, typename H>
void __print(unordered_map<T, V, H> v)
{
    cerr << " [ ";
    for (auto i : v)
    {
        __print(i);
        cerr << " ";
    }
    cerr << "]";
}

void debug_out() { cerr << endl; }
template <typename Head, typename... Tail>
void debug_out(Head H, Tail... T)
{
    __print(H);
    cerr << " ";
    debug_out(T...);
}
template <typename T>
void debug_graph(const T &g)
{
    map<pair<int, int>, bool> m;
    std::cout << "-------graph begin--------" << std::endl;
    for (int i = 0; i < (int)g.size(); i++)
    {
        for (auto j : g[i])
        {
            int mn = min(i, j), mx = max(i, j);
            if (m[{mn, mx}])
                continue;
            m[{mn, mx}] = 1;
            std::cout << i << " " << j << std::endl;
        }
    }
    std::cout << "-------graph end---------" << std::endl;
}

#endif
#else
#define debug(...) ;
#define debug_graph(...) ;
#endif