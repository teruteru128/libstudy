
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <xorshift.h>

int orz(size_t num)
{
    uint32_t seed;
    getrandom(&seed, sizeof(uint32_t), GRND_NONBLOCK);

    const char messages[][37] = { "orz", "申し訳ございませんでした",
                                  "ごめんなさい", "すみませんでした", "" };
    size_t messages_size = 0;
    while (messages[messages_size][0] != 0)
    {
        messages_size++;
    }
    for (size_t i = 0; i < num; i++)
    {
        printf("%s\n", messages[(seed = xorshift(seed)) % messages_size]);
    }

    return EXIT_SUCCESS;
}
