
#ifndef JAVA_RANDOM_H
#define JAVA_RANDOM_H

#include <stdint.h>
#include <stdlib.h>

typedef int32_t(nextfunc)(int bits, void *args);

int64_t nextLong(struct drand48_data *);
int32_t nextInt(int64_t *);
int32_t nextIntWithBounds(struct drand48_data *, int32_t);
int32_t nextintwithbounds2_r(int32_t bound, nextfunc *next, void *args);
double nextDouble(struct drand48_data *);
float nextFloat(struct drand48_data *);

// java.util.Random 内部状態計算ユーティリティ
int64_t lcg(int64_t);
int64_t lcgInverse(int64_t);
uint64_t initialScramble(uint64_t);

#endif
