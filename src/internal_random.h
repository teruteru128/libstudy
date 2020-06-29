
#ifndef INTERNAL_RANDOM_H
#define INTERNAL_RANDOM_H

#define MULTIPLIER 0x5DEECE66DL
#define INVERSE_MULTIPLIER 0xDFE05BCB1365L
#define ADDEND 0xBL
#define MASK 0xFFFFFFFFFFFFL
#define DOUBLE_UNIT 0x1.0p-53

typedef struct random_t
{
  int64_t seed;
} Random;

#endif
