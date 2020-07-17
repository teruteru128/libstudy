
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct bitset_t
{
  int64_t *words;
  size_t wordsLength;
  size_t wordsInUse;
  bool sizeIsSticky;
} bitset;

#define ADDRESS_BITS_PER_WORD (6)
#define BITS_PER_WORD (1 << ADDRESS_BITS_PER_WORD)
#define BIT_INDEX_MASK (BITS_PER_WORD - 1)
