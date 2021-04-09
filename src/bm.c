
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#include <openssl/evp.h>
#include <byteswap.h>
#include <changebase.h>
#include <err.h>
#include "bitmessage.h"
#include "bm.h"
#include "bmapi.h"
#include "nlz.h"
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
  size_t nlz = getNLZ(cache64, RIPEMD160_DIGEST_LENGTH);
  return nlz;
}

/*
 * 
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L63
 * https://docs.python.org/ja/3/library/struct.html
 */
struct chararray *encodeVarint(uint64_t u)
{
  struct chararray *p = malloc(sizeof(struct chararray));
  if (p == NULL)
  {
    return NULL;
  }
  if (u < 253)
  {
    p->data = malloc(sizeof(char));
    *p->data = (int8_t)u;
    p->length = 1;
    return p;
  }
  if (253 <= u && u < 65536)
  {
    p->data = malloc(sizeof(char) + sizeof(uint16_t));
    *p->data = -3;
    *((uint16_t *)&p->data[1]) = bswap_16((uint16_t)u);
    p->length = 3;
    return p;
  }
  if (65536 <= u && u < 4294967296L)
  {
    p->data = malloc(sizeof(char) + sizeof(uint32_t));
    *p->data = -2;
    *((uint32_t *)&p->data[1]) = bswap_32((uint32_t)u);
    p->length = 5;
    return p;
  }
  if (4294967296L <= u && u <= 18446744073709551615UL)
  {
    p->data = malloc(sizeof(char) + sizeof(uint64_t));
    *p->data = -1;
    *((uint64_t *)&p->data[1]) = bswap_64((uint64_t)u);
    p->length = 9;
    return p;
  }
  // おそらくここには来ない
  free(p);
  return NULL;
}

void chararrayfree(struct chararray *p)
{
  if (p->data)
    free(p->data);
  free(p);
}

char *encodeBase58()
{
  return NULL;
}

char *encodeAddress0(uint64_t version, uint64_t stream, unsigned char *ripe, size_t ripelen, size_t max)
{
  unsigned char *workripe = ripe;
  size_t workripelen = ripelen;
  if (version >= 2 && version < 4)
  {
    if (ripelen != 20)
    {
      return NULL;
    }
    if (max < 20)
    {
      if (memcmp(ripe, "\0\0", 2) == 0)
      {
        workripe = &ripe[2];
        workripelen -= 2;
      }
      else if (memcmp(ripe, "\0", 1) == 0)
      {
        workripe = &ripe[1];
        workripelen -= 1;
      }
    }
    else
    {
      size_t i = 0;
      for (; ripe[i] == 0 && i < max; i++)
      {
      }
    }
  }
  else
  {
    if (ripelen != 20)
    {
      return NULL;
    }
    size_t i = 0;
    for (; ripe[i] == 0 && i < ripelen; i++)
      ;

    workripe = &ripe[i];
    workripelen -= i;
  }
  struct chararray *variantVersion = encodeVarint(version);
  struct chararray *variantStream = encodeVarint(stream);
  size_t storedBinaryDataLen = variantVersion->length + variantStream->length + workripelen + 4;
  unsigned char *storedBinaryData = malloc(variantVersion->length + variantStream->length + workripelen + 4);
  memcpy(storedBinaryData, variantVersion->data, variantVersion->length);
  memcpy(storedBinaryData + variantVersion->length, variantStream->data, variantStream->length);
  memcpy(storedBinaryData + variantVersion->length + variantStream->length, workripe, workripelen);

  {
    const EVP_MD *sha512 = EVP_sha512();
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, sha512);
    EVP_DigestUpdate(ctx, storedBinaryData, storedBinaryDataLen - 4);
    unsigned int s = 0;
    unsigned char cache64[64];
    EVP_DigestFinal(ctx, cache64, &s);
    EVP_DigestInit(ctx, sha512);
    EVP_DigestUpdate(ctx, cache64, 64);
    EVP_DigestFinal(ctx, cache64, &s);
    EVP_MD_CTX_free(ctx);
    memcpy(storedBinaryData + variantVersion->length + variantStream->length + workripelen, cache64, 4);
  }
  chararrayfree(variantVersion);
  chararrayfree(variantStream);

  char *a = base58encode(storedBinaryData, storedBinaryDataLen);
  free(storedBinaryData);
  char *ret = malloc(45);
  snprintf(ret, 45, "BM-%s", a);
  free(a);
  {
    char *tmp = realloc(ret, strlen(ret) + 1);
    if (!ret)
    {
      free(ret);
      err(EXIT_FAILURE, "can not realloc");
    }
    ret = tmp;
  }
  return ret;
}

/*
 * ripeをBitMessageアドレスにエンコードします。
 * 
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L143
 * https://github.com/teruteru128/java-study/blob/03906187223ad8e5e8f8629e23ecbe2fbca5b7b4/src/main/java/com/twitter/teruteru128/study/bitmessage/genaddress/BMAddress.java#L18
 */
char *encodeAddress(uint64_t version, uint64_t stream, unsigned char *ripe, size_t ripelen)
{
  return encodeAddress0(version, stream, ripe, ripelen, 20);
}

char *encodeV4Address(unsigned char *ripe, size_t r)
{
  return encodeAddress0(4U, 1U, ripe, r, 20);
}

char *encodeV3Address(unsigned char *ripe, size_t r)
{
  return encodeAddress0(3UL, 1UL, ripe, r, 2);
}

char *encodeShorterV3Address(unsigned char *ripe, size_t r)
{
  return encodeAddress0(3UL, 1UL, ripe, r, 20);
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
