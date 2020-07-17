
#ifndef BITSET_H
#define BITSET_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// http://hg.openjdk.java.net/jdk/jdk/file/fb39a8d1d101/src/java.base/share/classes/java/util/BitSet.java
typedef struct bitset_t bitset;
bitset *bitset_new();
bitset *bitset_new2(size_t nbits);
bool bitset_get(bitset *set, size_t bitIndex);
bitset *bitset_clone(bitset *set);
void bitset_clear(bitset *set);
void bitset_free(bitset *set);
void bitset_set(bitset *set, size_t bitIndex);
#endif
