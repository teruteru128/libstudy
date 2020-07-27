
#ifndef BM_H
#define BM_H

#include <stdint.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>

#define PUBLIC_KEY_LENGTH 65

typedef struct
{
  SHA512_CTX sha512ctx;
  RIPEMD160_CTX ripemd160ctx;
  unsigned char cache64[SHA512_DIGEST_LENGTH];
} RIPE_CTX;

typedef struct clientinfo_t
{
} clientinfo;

typedef struct serverinfo_t
{
} serverinfo;

typedef struct connectioninfo_t
{
  clientinfo client;
  serverinfo server;
} connectioninfo;

struct chararray
{
  char *data;
  size_t length;
};

/* int ripe(ripectx, signpub, encpub) */
size_t ripe(RIPE_CTX *, unsigned char *, unsigned char *);
struct chararray *encodeVarint(uint64_t u);
void chararrayfree(struct chararray *p);
char *encodeBase58();
char *encodeAddress0(int, int, unsigned char *, size_t ripelen, size_t max);
char *encodeAddress(int version, int stream, unsigned char *ripe, size_t ripelen);
char *encodeV4Address(unsigned char *, size_t ripelen);
char *encodeV3Address(unsigned char *ripe, size_t ripelen);
char *encodeShorterV3Address(unsigned char *, size_t ripelen);
char *encodeWIF(char *);
char *formatKey(char *, char *, char *);
int exportAddress(unsigned char *, unsigned char *, unsigned char *, unsigned char *, unsigned char *);

#endif
