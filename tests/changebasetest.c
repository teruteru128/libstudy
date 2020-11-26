
#include <config.h>
#include <changebase.h>
#include <stdlib.h>

extern int strcmp(const char *, const char *);

int main(void)
{
    char hex[] = "594152414e41494b413f00";
    unsigned char *bin;
    parseHex(&bin, hex);
    printf("%s\n", bin);
    int ret = strcmp(bin, "YARANAIKA?");
    free(bin);
    return ret;
}
