
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <changebase.h>

int main(int argc, char const *argv[])
{
    char *hex2 = "00000D9663F57318B4E52288BFDC8B3C23E84DE1";
    unsigned char *in = NULL;
    unsigned int len = (unsigned int)parseHex(&in, hex2);
    printf("%u\n", len);
    //memset(in, 0, 10);
    char *out = encodeAddress(4, 1, in, len);
    printf("%s\n", out);
    free(in);
    free(out);
    return 0;
}