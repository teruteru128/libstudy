
#ifndef NLZ_H
#define NLZ_H

#include <stdio.h>

/*flg = 0
for(int i = 0; i < 5; i++)
    flg |= hash[i] != 0;

*/
static size_t getNLZ(unsigned char *ripe, const size_t limit)
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

#endif
