
#ifndef RANDOM_H
#define RANDOM_H

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int read_file(const char *const, void *, const size_t, const size_t);
size_t read_random(void *buf, const size_t size, size_t nmemb, int use_true_random);

void init_genrand64(uint64_t);
void init_by_array64(uint64_t *, size_t);
uint64_t genrand64_int64(void);
int64_t genrand64_int63(void);
double genrand64_real1(void);
double genrand64_real2(void);
double genrand64_real3(void);
int nextBytes(unsigned char *buf, size_t len);

#endif
