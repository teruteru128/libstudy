
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <changebase.h>
#include <bm_sonota.h>

#define DATA "000111d38e5fc9071ffcd20b4a763cc9ae4f252bb4e48fd66a835e252ada93ff480d6dd43dc62a641155a5"
#define TARGET "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"

int main(int argc, char const *argv[])
{
    unsigned char *in = NULL;
    size_t len = parseHex(&in, DATA);
    char *out = base58encode(in, len);
    int r = strcmp(out, TARGET);
    free(in);
    free(out);
    return r == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
