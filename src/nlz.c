
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t getNLZ(const unsigned char *ripe, const size_t limit)
{
    size_t nlz = 0;
    /*
    unsigned long *ripe_ul = (unsigned long *)ripe;
    size_t size_ul = limit >> 3;
    while(ripe_ul[nlz] == 0 && nlz < size_ul)
    {
        nlz++;
    }
    nlz <<= 3;
    */
    /*
    unsigned int *ripe_u = (unsigned int *)ripe;
    size_t size_ul = limit >> 2;
    while(ripe_ul[nlz] == 0 && nlz < size_ul)
    {
        nlz++;
    }
    nlz <<= 2;
    */
    while (ripe[nlz] == 0 && nlz < limit)
    {
        nlz++;
    }
    return nlz;
}
