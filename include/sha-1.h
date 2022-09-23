
#ifndef SHA_1_H
#define SHA_1_H

typedef unsigned int SHA_LONG;

#define SHA_LBLOCK 16
/* SHA treats input data as a 
 * contiguous array of 32 bit wide 
 * big-endian values. */
#define SHA_CBLOCK (SHA_LBLOCK * 4)
#define SHA_LAST_BLOCK (SHA_CBLOCK - 8)
#define SHA_DIGEST_LENGTH 20

typedef struct SHAstate_st
{
    SHA_LONG h0, h1, h2, h3, h4;
    SHA_LONG Nl, Nh;
    SHA_LONG data[SHA_LBLOCK];
    unsigned int num;
} SHA_CTX;

#endif
