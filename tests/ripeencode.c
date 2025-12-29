
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <changebase.h>
#include <bm_sonota.h>

#define DATA "00000D9663F57318B4E52288BFDC8B3C23E84DE1"
#define TARGET "BM-NAxwJoagtHikgoyv2wsDpdFdwb867sKn"

int main(int argc, char const *argv[])
{
    unsigned char *in = NULL;
    size_t len = parseHex(&in, DATA);
    char *out = encodeAddress(4, 1, in, len);
    int r = strcmp(out, TARGET);
    free(in);
    free(out);
    return r == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
