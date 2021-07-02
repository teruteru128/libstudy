
#ifndef BM_H
#define BM_H

#include <stdint.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>
#include "bitmessage_type.h"

typedef struct
{
  SHA512_CTX sha512ctx;
  RIPEMD160_CTX ripemd160ctx;
  unsigned char cache64[SHA512_DIGEST_LENGTH];
} RIPE_CTX;

void hashSHA512(EVP_MD_CTX *mdctx, const EVP_MD *sha512, unsigned char *cache64, unsigned char *publicKey, size_t signkeyindex, size_t enckeyindex);
void hashRIPEMD160(EVP_MD_CTX *mdctx, const EVP_MD *ripemd160, unsigned char *cache64);
int calcRipe(EVP_MD_CTX *mdctx, const EVP_MD *sha512, const EVP_MD *ripemd160, unsigned char *cache64, unsigned char *publicKey, size_t signkeyindex, size_t enckeyindex);

/* int ripe(ripectx, signpub, encpub) */
size_t ripe(RIPE_CTX *, unsigned char *, unsigned char *);
struct chararray *encodeVarint(uint64_t u);
void chararrayfree(struct chararray *p);
char *encodeBase58();
char *encodeAddress0(uint64_t version, uint64_t stream, unsigned char *ripe, size_t ripelen, size_t max);
char *encodeAddress(uint64_t version, uint64_t stream, unsigned char *ripe, size_t ripelen);
char *encodeV4Address(unsigned char *, size_t ripelen);
char *encodeV3Address(unsigned char *ripe, size_t ripelen);
char *encodeShorterV3Address(unsigned char *, size_t ripelen);
char *encodeWIF(unsigned char *);
char *formatKey(char *, char *, char *);
int exportAddress(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *);

#endif
