
#ifndef _SRC_TYPES_H_
#define _SRC_TYPES_H_

#include <stdint.h>
#include <random>

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef std::exponential_distribution<double> RealExpDistr;
typedef std::uniform_real_distribution<double> RealUniformDistr;
typedef std::uniform_int_distribution<uint32_t> uintUniformDistr;


#endif // _SRC_TYPES_H_
