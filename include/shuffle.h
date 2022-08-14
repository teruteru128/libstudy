
#ifndef SHUFFLE_H
#define SHUFFLE_H 1

#include <stddef.h>
#include <stdint.h>

void shuffle(void *base, size_t nmemb, size_t size);
// deprecated
uint32_t *getseedp(void);

#endif // SHUFFLE_H
