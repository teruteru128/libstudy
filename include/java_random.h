
#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

typedef struct random_t Random;

// Random *Random_constructor();
// int Random_destructor(Random *);
// Random *random_new();
// int random_free(Random *);
Random *setSeed(Random *, int64_t);
int32_t next(Random *, int32_t);
int64_t nextLong(Random *);
int32_t nextInt(Random *);
int32_t nextIntWithBounds(Random *, int32_t);
double nextDouble(Random *);

// java.util.Random 内部状態計算ユーティリティ
int64_t n(int64_t);
int64_t nInverse(int64_t);
int64_t initializeSeed(int64_t);

#endif
