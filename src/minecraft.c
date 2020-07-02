
#include <stdint.h>

#define MC_CACHE_SIZE 1250

int64_t s(int64_t seed, int32_t x, int32_t z)
{
  return (seed + (long)(x * x * 0x4c1906) + (long)(x * 0x5ac0db) + (long)(z * z) * 0x4307a7L + (long)(z * 0x5f24f)) ^ 0x3ad8025f;
}

static int64_t memoX[MC_CACHE_SIZE];
static int64_t memoZ[MC_CACHE_SIZE];

void mc_slime_init()
{
  int32_t pos = 0;
  const int32_t offset = (MC_CACHE_SIZE + 1) / 2;
  int32_t square = 0;
  int32_t x1;
  for (int i = 0; i < MC_CACHE_SIZE; i++)
  {
    pos = i - offset;
    square = pos * pos;
    memoX[i] = 0L + (long)(pos * pos * 0x4c1906) + (long)(pos * 0x5ac0db);
    memoZ[i] = pos * pos * 0x4307a7L + pos * 0x5f24f;
  }
}

int64_t s1(int64_t seed, int32_t x, int32_t z)
{
  return (seed + memoX[x + 625] + memoZ[z + 625]) ^ 0x3ad8025f;
}

int64_t s2(int64_t seed, int32_t x, int32_t z)
{
  int64_t x2 = x * x * 0x4c1906;
  int64_t x1 = x * 0x5ac0db;
  int64_t z2 = z * z * 0x4307a7L;
  int64_t z1 = z * 0x5f24f;
  return (seed + x2 + x1 + z2 + z1) ^ 0x3ad8025f;
}
