
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "internal_random.h"

int64_t initialScramble(int64_t seed)
{
    return (seed ^ MULTIPLIER) & MASK;
}

int64_t *setSeed(int64_t *rnd, int64_t seed)
{
    *rnd = initialScramble(seed);
    return rnd;
}

int32_t next(int64_t *rnd, int32_t bits)
{
    int64_t oldseed;
    oldseed = *rnd;
    return (int32_t)((*rnd = (oldseed * MULTIPLIER + ADDEND) & MASK) >> (48 - bits));
}

int64_t nextLong(int64_t *rnd)
{
    return ((int64_t)(next(rnd, 32)) << 32) + next(rnd, 32);
}

int32_t nextInt(int64_t *rnd)
{
    return next(rnd, 32);
}

int32_t nextIntWithBounds(int64_t *rnd, int32_t bound)
{
    if (bound <= 0)
    {
        // err!
        return 0;
    }
    int32_t r = next(rnd, 31);
    int32_t m = bound - 1;
    if ((bound & m) == 0)
    {
        r = (int32_t)((bound * (int64_t)r) >> 31);
    }
    else
    {
        int32_t u;
        for (u = r; u - (r = u % bound) + m < 0; u = next(rnd, 31))
            ;
    }
    return r;
}

double nextDouble(int64_t *rnd)
{
  return (((int64_t)(next(rnd, 26)) << 27) + next(rnd, 27)) * DOUBLE_UNIT;
}

float nextFloat(int64_t *rnd)
{
    return next(rnd, 24) / ((float)(1 << 24));
}

// 
// nextもrandomも使われているので適当にpとする
// rかn関数のほうがいいかな？
// next関数の略としてn
int64_t n(int64_t seed) {
  return (seed * MULTIPLIER + ADDEND) & MASK;
}

int64_t nInverse(int64_t seed) {
  return (seed - ADDEND) * INVERSE_MULTIPLIER & MASK;
}
