
#ifndef CHANGEBASE_H
#define CHANGEBASE_H

#include <stdio.h>

size_t parseHex(unsigned char **out, const char *str);
size_t hexdecode(unsigned char **out, const char *str);
char *base58encode(const unsigned char *input, const size_t length);
int base32decode(const char *src, unsigned char *out, size_t *outlen);

#endif
