
#include <bitset.h>
#include "internal_bitset.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static void checkInvariants(bitset *set)
{
  assert(set->wordsInUse == 0 || set->words[set->wordsInUse - 1] != 0);
  assert(/*set->wordsInUse >= 0 && */ set->wordsInUse <= set->wordsLength);
  assert(set->wordsInUse == set->wordsLength || set->words[set->wordsInUse] == 0);
}

static size_t wordIndex(size_t bitIndex)
{
  return bitIndex >> ADDRESS_BITS_PER_WORD;
}

#define max(a, b) (((a) >= (b)) ? (a) : (b))
#define min(a, b) (((a) <= (b)) ? (a) : (b))

static void ensureCapacity(bitset *set, size_t wordsRequired)
{
  if (set->wordsLength < wordsRequired)
  {
    size_t request = max(2 * set->wordsLength, wordsRequired);
    int64_t *tmp = realloc(set->words, sizeof(int64_t) * request);
    if (tmp != NULL)
    {
      memset(tmp + set->wordsLength, 0, sizeof(int64_t) * (request - set->wordsLength));
      set->words = tmp;
      set->wordsLength = request;
      set->sizeIsSticky = false;
    }
  }
}

static void expandTo(bitset *set, size_t wordIndex)
{
  size_t wordsRequired = wordIndex + 1;
  if (set->wordsInUse < wordsRequired)
  {
    ensureCapacity(set, wordsRequired);
    set->wordsInUse = wordsRequired;
  }
}

static void initWords(bitset *set, size_t nbits)
{
  set->words = calloc(wordIndex(nbits - 1) + 1, sizeof(int64_t));
  set->wordsLength = 8;
  set->wordsInUse = 0;
}

bitset *bitset_new()
{
  bitset *set = malloc(sizeof(bitset));
  initWords(set, BITS_PER_WORD);
  set->sizeIsSticky = false;
  return set;
}

bitset *bitset_new2(size_t nbits)
{
  bitset *set = malloc(sizeof(bitset));
  initWords(set, nbits);
  set->sizeIsSticky = true;
  return set;
}

bool bitset_get(bitset *set, size_t bitIndex)
{
  checkInvariants(set);
  size_t index = wordIndex(bitIndex);
  return (index < set->wordsInUse) && ((set->words[index] & (1L << bitIndex)) != 0);
}

#if 0
static void initWords(bitset *set, size_t nbits)
{
  set->wordsLength = wordIndex(nbits - 1) + 1;
  set->words = malloc(sizeof(int64_t) * set->wordsLength);
  memset(set->words, 0, sizeof(int64_t) * set->wordsLength);
}

void bitset_init(bitset *set)
{
  set->wordsInUse = 0;
  initWords(set, BITS_PER_WORD);
  set->sizeIsSticky = false;
}

void bitset_init2(bitset *set, size_t nbits)
{
  if (nbits < 0)
  {
    return;
  }
  initWords(set, nbits);
  set->sizeIsSticky = true;
}

bitset* bitset_alloc()
{
  bitset* set = malloc(sizeof(bitset));
  memset(set, 0, sizeof(bitset));
  bitset_init(set);
  return set;
}
#endif

static void trimToSize(bitset *set)
{
  if (set->wordsInUse != set->wordsLength)
  {
    int64_t *tmp = calloc(set->wordsInUse, sizeof(int64_t));
    size_t length = min(set->wordsLength, set->wordsInUse);
    memcpy(tmp, set->words, length * sizeof(int64_t));
    set->words = tmp;
    set->wordsLength = length;
    checkInvariants(set);
  }
}

bitset *bitset_clone(bitset *set)
{
  if (!set->sizeIsSticky)
    trimToSize(set);

  bitset *dolly = malloc(sizeof(bitset));
  if (!dolly)
    return NULL;
  dolly->words = set->sizeIsSticky;
  dolly->words = set->wordsInUse;
  dolly->words = set->wordsLength;
  dolly->words = calloc(set->wordsLength, sizeof(int64_t));
  if (!dolly->words)
  {
    free(dolly);
    return NULL;
  }
  memcpy(dolly->words, set->words, set->wordsLength * sizeof(int64_t));
  checkInvariants(dolly);
  return dolly;
}

void bitset_free(bitset *set)
{
  if (set->words != NULL)
  {
    free(set->words);
    set->words = NULL;
  }
}

void bitset_set(bitset *set, size_t bitIndex)
{
  size_t index = wordIndex(bitIndex);
  expandTo(set, index);
  set->words[index] |= (1L << bitIndex);

  checkInvariants(set);
}
