
#include <stdio.h>
#include <stdint.h>
#include "internal_random.h"
#include <random.h>

uint32_t xorshift(const uint32_t seed)
{
    uint32_t s = seed;
    s = s ^ (s << 13);
    s = s ^ (s >> 17);
    return s ^ (s << 15);
}

uint64_t xorshift64(const uint64_t seed)
{
    uint64_t s = seed;
    s = s ^ (s << 13);
    s = s ^ (s >> 7);
    return s ^ (s << 17);
}

uint32_t xorshift96(const uint32_t seed1, const uint32_t seed2, const uint32_t seed3)
{
    uint32_t t = (seed1 ^ (seed1 << 3)) ^ (seed2 ^ (seed2 >> 19)) ^ (seed3 ^ (seed3 << 6));
    return 0;
}

uint32_t xorshift128(const uint32_t seed1, const uint32_t seed2, const uint32_t seed3, const uint32_t seed4)
{
    uint32_t t = seed1 ^ (seed1 << 11);
    return 0;
}

double nextDoubleXor()
{
    static uint32_t y = 2463534242;
    uint32_t z1 = y = xorshift(y);
    uint32_t z2 = y = xorshift(y);
    return (((int64_t)(z1 & 0x3ffffff) << 27) + (z2 & 0x7ffffff)) * DOUBLE_UNIT;
}
