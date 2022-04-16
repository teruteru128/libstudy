
#ifndef BITMESSAGE_TYPE_H
#define BITMESSAGE_TYPE_H

#include <netdb.h>
#include <openssl/evp.h>
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

const endpointinfo_t server_addresses_list[5];

#endif // BITMESSAGE_TYPE_H
