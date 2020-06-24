
#include <stdio.h>
#include <stdint.h>

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
  return 0;
}

uint32_t xorshift128(const uint32_t seed1, const uint32_t seed2, const uint32_t seed3, const uint32_t seed4)
{
  return 0;
}

