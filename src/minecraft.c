
#include <stdio.h>
#include <stdint.h>
#include <java_random.h>

uint64_t seeding(uint64_t seed, uint32_t chunkX, uint32_t chunkZ)
{
    return seed + chunkX * chunkX * 0x4c1906 + chunkX * 0x5ac0db + chunkZ * chunkZ * 0x4307a7L + chunkZ * 0x5f24f ^ 0x3ad8025f;
}

int isSlimeChunk(uint64_t *ctx, uint64_t seed, uint32_t x, uint32_t z)
{
    *ctx = initialScramble(seeding(seed, x, z));
    return !nextIntWithBounds((int64_t *)ctx, 10);
}
