
#ifndef BM_H
#define BM_H

#include <stdint.h>
#include <config.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>
#include <netdb.h>

#define PUBLIC_KEY_LENGTH 65

typedef struct
{
  SHA512_CTX sha512ctx;
  RIPEMD160_CTX ripemd160ctx;
  unsigned char cache64[SHA512_DIGEST_LENGTH];
} RIPE_CTX;

typedef struct endpointinfo
{
  char host[NI_MAXHOST];
  char port[NI_MAXSERV];
} endpointinfo_t;

typedef struct peerinfo
{
  int peer_id;
} peerinfo_t;

typedef struct serverinfo
{
  endpointinfo_t ep;
} serverinfo_t;

typedef struct connectioninfo
{
  peerinfo_t client;
  serverinfo_t server;
} connectioninfo_t;

const endpointinfo_t server_addresses_list[5];

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
