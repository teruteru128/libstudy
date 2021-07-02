
#ifndef BITMESSAGE_TYPE_H
#define BITMESSAGE_TYPE_H

#include <netdb.h>
#define PRIVATE_KEY_LENGTH 32
#define PUBLIC_KEY_LENGTH 65
typedef unsigned char PrivateKey[PUBLIC_KEY_LENGTH];
typedef unsigned char PublicKey[PUBLIC_KEY_LENGTH];

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

struct chararray
{
  char *data;
  size_t length;
};

#endif // BITMESSAGE_TYPE_H 
