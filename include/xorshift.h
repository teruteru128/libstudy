
#ifndef XORSHIFT_T
#define XORSHIFT_T

#include <stdint.h>

uint32_t xorshift (const uint32_t);
uint64_t xorshift64(const uint64_t);
uint32_t xorshift96(const uint32_t, const uint32_t, const uint32_t);
uint32_t xorshift128(const uint32_t, const uint32_t, const uint32_t, const uint32_t);
double nextDoubleXor();
#endif
