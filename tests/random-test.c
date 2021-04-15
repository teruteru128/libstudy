
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <java_random.h>

int main(void)
{
    int64_t seed = 0;
    int32_t r = nextInt(&seed);
    fprintf(stderr, "%d\n", r);
#if 0
    if (r != -1155484576)
        return 1;
#endif

    seed = INT_MAX;
    r = nextInt(&seed);
    fprintf(stderr, "%d\n", r);
#if 0
    if (r != -1155484576)
        return 1;
#endif

    seed = 0;
    r = nextIntWithBounds(&seed, 10);
    fprintf(stderr, "%d\n", r);
#if 0
    if (r != 0)
        return 1;
#endif

    seed = INT_MAX;
    r = nextIntWithBounds(&seed, 10);
    fprintf(stderr, "%d\n", r);
#if 0
    if (r != 1)
        return 1;
#endif

    return 0;
}
