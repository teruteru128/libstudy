
#ifndef BITMESSAGE_TYPE_H
#define BITMESSAGE_TYPE_H

#include <netdb.h>
#include <openssl/evp.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#define PRIVATE_KEY_LENGTH 32
#define PUBLIC_KEY_LENGTH 65
#define RIPE_HASH_LENGTH 20
typedef unsigned char PrivateKey[PRIVATE_KEY_LENGTH];
typedef unsigned char PublicKey[PUBLIC_KEY_LENGTH];
typedef unsigned char RipeHash[RIPE_HASH_LENGTH];

struct BMAddressPubKeySet
{
    PublicKey signingKey;
    PublicKey encryptingKey;
};

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

typedef struct
{
    SHA512_CTX sha512ctx;
    RIPEMD160_CTX ripemd160ctx;
    EVP_MD *sha512md;
    EVP_MD *ripemd160md;
    EVP_MD_CTX *ctx;
    unsigned char hash[EVP_MAX_MD_SIZE];
} RIPE_CTX;

extern const endpointinfo_t server_addresses_list[5];

enum EncodingType
{
    IGNORE = 0,
    TRIVAL = 1,
    SIMPLE = 2,
    EXTENDED = 3
};

#endif // BITMESSAGE_TYPE_H
