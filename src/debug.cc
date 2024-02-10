
#include "debug.h"

void __print(int x) { cerr << x; }
void __print(int64 x) { cerr << x; }
void __print(size_t x) { cerr << x; }
void __print(double x) { cerr << x; }
void __print(char x) { cerr << '\'' << x << '\''; }
void __print(const char *x) { cerr << '\"' << x << '\"'; }
void __print(const string &x) { cerr << '\"' << x << '\"'; }
void __print(bool x) { cerr << (x ? "true" : "false"); }

void debug_out() { cerr << endl; }
