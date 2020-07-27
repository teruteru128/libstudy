
#ifndef CHANGEBASE_H
#define CHANGEBASE_H

#include <stdio.h>

size_t parseHex(unsigned char **out, const char *str);
char *base58encode(unsigned char *input, size_t length);

#endif
