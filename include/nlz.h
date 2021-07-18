
#ifndef NLZ_H
#define NLZ_H

#include <stdio.h>

/*flg = 0
for(int i = 0; i < 5; i++)
    flg |= hash[i] != 0;

*/
size_t getNLZ(const unsigned char *ripe, const size_t limit);

#endif
