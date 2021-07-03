
#ifndef BM_H
#define BM_H

#include "bitmessage_type.h"
#include <openssl/evp.h>
#include <openssl/ripemd.h>
#include <openssl/sha.h>
#include <stdint.h>
#include <xmlrpc.h>
#include <xmlrpc_client.h>

typedef struct
{
    SHA512_CTX sha512ctx;
    RIPEMD160_CTX ripemd160ctx;
    EVP_MD_CTX *ctx;
    unsigned char hash[SHA512_DIGEST_LENGTH];
} RIPE_CTX;

void hashSHA512(EVP_MD_CTX *mdctx, const EVP_MD *sha512,
                unsigned char *cache64, PublicKey *signPublicKey,
             PublicKey *encPublicKey);
void hashRIPEMD160(EVP_MD_CTX *mdctx, const EVP_MD *ripemd160,
                   unsigned char *cache64);
void chararrayfree(struct chararray *p);
char *encodeBase58();
char *formatKey(char *, char *, char *);
int exportAddress(PrivateKey *, PublicKey *, PrivateKey *,
                  PublicKey *, unsigned char *);
// secp256k1上でプライベート鍵をパブリック鍵に変換する
int getPublicKey(PublicKey *pubKey, PrivateKey *priKey);
// 秘密鍵を財布インポート形式に変換する
char *encodeWIF(PrivateKey *);
// 公開鍵2個からripeハッシュを計算
int calcRipe(EVP_MD_CTX *mdctx, const EVP_MD *sha512, const EVP_MD *ripemd160,
             unsigned char *cache64, PublicKey *signPublicKey,
             PublicKey *encPublicKey);

/* int ripe(ripectx, signpub, encpub) */
size_t ripe(RIPE_CTX *, unsigned char *, unsigned char *);
// ripeハッシュをBMアドレスに変換
char *encodeAddress0(uint64_t version, uint64_t stream, unsigned char *ripe,
                     size_t ripelen, size_t max);
char *encodeAddress(uint64_t version, uint64_t stream, unsigned char *ripe,
                    size_t ripelen);
char *encodeV4Address(unsigned char *, size_t ripelen);
char *encodeV3Address(unsigned char *ripe, size_t ripelen);
char *encodeShorterV3Address(unsigned char *, size_t ripelen);

#endif
