
#ifndef INTERNAL_RANDOM_H
#define INTERNAL_RANDOM_H

#define MULTIPLIER 0x5DEECE66DULL
#define ADDEND 0xBULL
#define MASK 0xFFFFFFFFFFFFULL
#define DOUBLE_UNIT 0x1.0p-53

typedef struct random_t
{
  int64_t seed;
} Random;

#endif
