
#if !defined _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <java_random.h>

static uint64_t seeding(uint64_t seed, uint32_t chunkX, uint32_t chunkZ)
{
    return seed + chunkX * chunkX * 0x4c1906 + chunkX * 0x5ac0db + chunkZ * chunkZ * 0x4307a7L + chunkZ * 0x5f24f ^ 0x3ad8025f;
}

int isSlimeChunk(struct drand48_data *ctx, uint64_t seed, uint32_t x, uint32_t z)
{
    uint64_t a = initialScramble(seeding(seed, x, z));
    unsigned short buf[3];
    buf[0] = (unsigned short)(a >> 0);
    buf[1] = (unsigned short)(a >> 16);
    buf[2] = (unsigned short)(a >> 32);
    seed48_r(buf, ctx);
    return !nextIntWithBounds(ctx, 10);
}
