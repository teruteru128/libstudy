
#ifndef NLZ_H
#define NLZ_H

#include <stdio.h>

static size_t getNLZ(unsigned char *ripe, const size_t size)
{
    size_t nlz = 0;
    /*
    unsigned long *ripe_ul = (unsigned long *)ripe;
    size_t size_ul = size >> 3;
    while(ripe_ul[nlz] == 0 && nlz < size_ul)
    {
        nlz++;
    }
    nlz <<= 3;
    */
    /*
    unsigned int *ripe_u = (unsigned int *)ripe;
    size_t size_ul = size >> 2;
    while(ripe_ul[nlz] == 0 && nlz < size_ul)
    {
        nlz++;
    }
    nlz <<= 2;
    */
    while (ripe[nlz] == 0 && nlz < size)
    {
        nlz++;
    }
    return nlz;
}

#endif
