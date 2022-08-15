
#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>
#include <stdio.h>

void init_genrand64(uint64_t);
void init_by_array64(uint64_t *, size_t);
uint64_t genrand64_int64(void);
int64_t genrand64_int63(void);
double genrand64_real(void);
double genrand64_real2(void);

#endif
