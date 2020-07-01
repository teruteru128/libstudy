
#ifndef JAVA_RANDOM_H
#define JAVA_RANDOM_H

#include <stdint.h>

typedef struct random_t Random;

// Random *Random_constructor();
// int Random_destructor(Random *);
// Random *random_new();
// void random_free(Random *);
Random *setSeed(int64_t *, int64_t);
int32_t next(int64_t *, int32_t);
int64_t nextLong(int64_t *);
int32_t nextInt(int64_t *);
int32_t nextIntWithBounds(int64_t *, int32_t);
double nextDouble(int64_t *);

// java.util.Random 内部状態計算ユーティリティ
int64_t n(int64_t);
int64_t nInverse(int64_t);
int64_t initialScramble(int64_t);

#endif
