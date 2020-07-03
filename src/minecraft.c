
#include <stdio.h>
#include <stdint.h>

#define MC_CACHE_SIZE 1250
#define OFFSET ((MC_CACHE_SIZE + 1) / 2)

int64_t s(int64_t seed, int32_t x, int32_t z)
{
  return (seed + (long)(x * x * 0x4c1906) + (long)(x * 0x5ac0db) + (long)(z * z) * 0x4307a7L + (long)(z * 0x5f24f)) ^ 0x3ad8025f;
}

static int64_t memoX[MC_CACHE_SIZE];
static int64_t memoZ[MC_CACHE_SIZE];

void mc_slime_init()
{
  int64_t pos = 0;
  int64_t square = 0;
  //int64_t x1, x2;
  for (size_t i = 0; i < MC_CACHE_SIZE; i++)
  {
    pos = i - OFFSET;
    square = pos * pos;
    memoX[i] = square * 0x4c1906 + pos * 0x5ac0db;
    memoZ[i] = pos * pos * 0x4307a7L + pos * 0x5f24f;
  }
}

int64_t s1(int64_t seed, int32_t x, int32_t z)
{
  return (seed + memoX[x + OFFSET] + memoZ[z + OFFSET]) ^ 0x3ad8025f;
}

int64_t s2(int64_t seed, int32_t x, int32_t z)
{
  int64_t x2 = x * x * 0x4c1906;
  int64_t x1 = x * 0x5ac0db;
  int64_t z2 = z * z * 0x4307a7L;
  int64_t z1 = z * 0x5f24f;
  return (seed + x2 + x1 + z2 + z1) ^ 0x3ad8025f;
}