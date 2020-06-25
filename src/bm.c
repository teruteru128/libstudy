
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include "bitmessage.h"
#include "bm.h"
#include "bmapi.h"
#define localhost_ip "127.0.0.1"
#define bitmessage_port 8442
#define NAME "TR BM TEST CLIENT"
#define SERVER_URL "http://127.0.0.1:8442/"

size_t ripe(RIPE_CTX *ctx, unsigned char *signpubkey, unsigned char *encpubkey)
{
  unsigned char *cache64 = ctx->cache64;
  SHA512_CTX sha512ctx;
  RIPEMD160_CTX ripemd160ctx;
  SHA512_Init(&sha512ctx);
  SHA512_Update(&sha512ctx, signpubkey, PUBLIC_KEY_LENGTH);
  SHA512_Update(&sha512ctx, encpubkey, PUBLIC_KEY_LENGTH);
  SHA512_Final(cache64, &sha512ctx);
  RIPEMD160_Init(&ripemd160ctx);
  RIPEMD160_Update(&ripemd160ctx, cache64, SHA512_DIGEST_LENGTH);
  RIPEMD160_Final(cache64, &ripemd160ctx);
  size_t nlz = 0;
  for (; cache64[nlz] == 0 && nlz < RIPEMD160_DIGEST_LENGTH; nlz++){}
  return nlz;
}

char *encodeVarint(unsigned long u)
{
    return NULL;
}

char *encodeBase58()
{
    return NULL;
}

char *encodeAddress0(int version, int stream, char *ripe, int max)
{
    max = 1 <= max ? max : 1;
    max = max <= 20 ? max : 20;
    if (version >= 2 && version < 4)
    {
        int i = 0;
        for (; ripe[i] == 0 && i < 2; i++)
        {
        }
    }
    else if (version == 4)
    {
        int i = 0;
        for (; ripe[i] == 0 && i < RIPEMD160_DIGEST_LENGTH; i++)
        {
        }
    }
    return NULL;
}

char *encodeAddress(int version, int stream, char *ripe)
{
    return encodeAddress0(version, stream, ripe, 20);
}

char *encodeV4Address(char *ripe)
{
    return encodeAddress0(4, 1, ripe, 20);
}

char *encodeV3Address(char *ripe)
{
    return encodeAddress0(3, 1, ripe, 2);
}

char *encodeShorterV3Address(char *ripe)
{
    return encodeAddress0(3, 1, ripe, 20);
}

char *encodeWIF(char *key)
{
    return NULL;
}

char *formatKey(char *address, char *privateSigningKeyWIF, char *privateEncryptionKeyWIF)
{
    char *buf = malloc(301);
    memset(buf, 0, 300);
    snprintf(buf, 300, "[%s]]\nlabel = relpace this label\nenabled = true\ndecoy = false\nnoncetrialsperbyte = 1000\npayloadlengthextrabytes = 1000\nprivsigningkey = %s\nprivencryptionkey = %s\n", address, privateSigningKeyWIF, privateEncryptionKeyWIF);
    return buf;
}

int exportAddress(unsigned char *privateSigningKey, unsigned char *publicSigningKey, unsigned char *privateEncryptionKey, unsigned char *publicEncryptionKey, unsigned char *ripe)
{
    /*
    char *address4 = encodeAddress(4, 1, ripe);
    char *privateSigningKeyWIF = encodeWIF(privateSigningKey);
    char *privateEncryptionKeyWIF = encodeWIF(privateEncryptionKey);
    char *formated = formatKey(address4, privateSigningKeyWIF, privateEncryptionKeyWIF);
    printf("%s\n", formated);
    */
    char buf[227];
    snprintf(buf, 227, "ripe = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n"
                       "private signing key = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n"
                       "private encryption key = %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n\n",
             ripe[0], ripe[1], ripe[2], ripe[3], ripe[4], ripe[5], ripe[6], ripe[7], ripe[8], ripe[9], ripe[10], ripe[11], ripe[12], ripe[13], ripe[14], ripe[15], ripe[16], ripe[17], ripe[18], ripe[19],
             privateSigningKey[0], privateSigningKey[1], privateSigningKey[2], privateSigningKey[3], privateSigningKey[4], privateSigningKey[5], privateSigningKey[6], privateSigningKey[7],
             privateSigningKey[8], privateSigningKey[9], privateSigningKey[10], privateSigningKey[11], privateSigningKey[12], privateSigningKey[13], privateSigningKey[14], privateSigningKey[15],
             privateSigningKey[16], privateSigningKey[17], privateSigningKey[18], privateSigningKey[19], privateSigningKey[20], privateSigningKey[21], privateSigningKey[22], privateSigningKey[23],
             privateSigningKey[24], privateSigningKey[25], privateSigningKey[26], privateSigningKey[27], privateSigningKey[28], privateSigningKey[29], privateSigningKey[30], privateSigningKey[31],
             privateEncryptionKey[0], privateEncryptionKey[1], privateEncryptionKey[2], privateEncryptionKey[3], privateEncryptionKey[4], privateEncryptionKey[5], privateEncryptionKey[6], privateEncryptionKey[7],
             privateEncryptionKey[8], privateEncryptionKey[9], privateEncryptionKey[10], privateEncryptionKey[11], privateEncryptionKey[12], privateEncryptionKey[13], privateEncryptionKey[14], privateEncryptionKey[15],
             privateEncryptionKey[16], privateEncryptionKey[17], privateEncryptionKey[18], privateEncryptionKey[19], privateEncryptionKey[20], privateEncryptionKey[21], privateEncryptionKey[22], privateEncryptionKey[23], privateEncryptionKey[24],
             privateEncryptionKey[25], privateEncryptionKey[26], privateEncryptionKey[27], privateEncryptionKey[28], privateEncryptionKey[29], privateEncryptionKey[30], privateEncryptionKey[31]);
    fputs(buf, stdout);
    /*
    free(formated);
    free(privateEncryptionKeyWIF);
    free(privateSigningKeyWIF);
    free(address4);
    */
    return EXIT_SUCCESS;
}
