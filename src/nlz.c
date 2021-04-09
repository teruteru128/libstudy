
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t getNLZ(unsigned char *ripe, size_t size)
{
  size_t nlz = 0;
  for (nlz = 0; ripe[nlz] == 0 && nlz < size; nlz++)
    ;
  return nlz;
}
