
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xorshift.h>

#define URANDOM_PATH "/dev/urandom"
#define BUF_SIZE 8
#include <random.h>

int orz(size_t num)
{
    size_t count = num > 0 ? num : 334;
    uint32_t seed;
    if (read_file(URANDOM_PATH, &seed, sizeof(uint32_t), 1) != 0)
    {
        warnx("failed");
        return EXIT_FAILURE;
    }

    const char messages[][37] = {
        "orz",
        "申し訳ございませんでした",
        "ごめんなさい",
        "すみませんでした",
        ""};
    size_t messages_size = 0;
    while(messages[messages_size][0] != 0)
    {
        messages_size++;
    }
    for (size_t i = 0; i < count; i++)
    {
        printf("%s\n", messages[(seed = xorshift(seed)) % messages_size]);
    }

    return EXIT_SUCCESS;
}
