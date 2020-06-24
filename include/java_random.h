
#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

typedef struct random_t
{
  int64_t seed;
} Random;

Random *setSeed(Random *, int64_t);
int32_t next(Random *, int32_t);
int64_t nextLong(Random *);
int32_t nextInt(Random *);
int32_t nextIntWithBounds(Random *, int32_t);

// java.util.Random 内部状態計算ユーティリティ
int64_t p(int64_t);
int64_t pInverse(int64_t);
int64_t initializeSeed(int64_t);

#endif