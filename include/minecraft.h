
#ifndef MINECRAFT_H
#define MINECRAFT_H

#include <stdint.h>

int64_t s(int64_t seed, int32_t x, int32_t z);
int isSlimeChunk(int64_t *ctx, int64_t seed, int32_t x, int32_t z);

#endif
