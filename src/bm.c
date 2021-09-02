
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "bitmessage.h"
#include "bm.h"
#include "bmapi.h"
#include "nlz.h"
#include <byteswap.h>
#include <changebase.h>
#include <err.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <stdlib.h>
#include <uuid/uuid.h>
#define localhost_ip "127.0.0.1"
#define bitmessage_port 8442
#define NAME "TR BM TEST CLIENT"
#define SERVER_URL "http://127.0.0.1:8442/"

void hashSHA512(EVP_MD_CTX *mdctx, const EVP_MD *sha512,
                unsigned char *cache64, PublicKey *singPublicKey,
                PublicKey *encPublicKey)
{
    EVP_DigestInit(mdctx, sha512);
    EVP_DigestUpdate(mdctx, singPublicKey, PUBLIC_KEY_LENGTH);
    EVP_DigestUpdate(mdctx, encPublicKey, PUBLIC_KEY_LENGTH);
    EVP_DigestFinal(mdctx, cache64, NULL);
}

void hashRIPEMD160(EVP_MD_CTX *mdctx, const EVP_MD *ripemd160,
                   unsigned char *cache64)
{
    EVP_DigestInit(mdctx, ripemd160);
    EVP_DigestUpdate(mdctx, cache64, 64);
    EVP_DigestFinal(mdctx, cache64, NULL);
}

int calcRipe(EVP_MD_CTX *mdctx, const EVP_MD *sha512, const EVP_MD *ripemd160,
             unsigned char *cache64, PublicKey *singPublicKey,
             PublicKey *encPublicKey)
{
    hashSHA512(mdctx, sha512, cache64, singPublicKey, encPublicKey);
    hashRIPEMD160(mdctx, ripemd160, cache64);
    return 0;
}

size_t ripe(RIPE_CTX *ripectx, unsigned char *signpubkey,
            unsigned char *encpubkey)
{
    unsigned char *cache64 = ripectx->hash;
    EVP_MD_CTX *mdctx = ripectx->ctx;
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
static struct chararray *encodeVarint(uint64_t u)
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

char *encodeAddress0(uint64_t version, uint64_t stream, unsigned char *ripe,
                     size_t ripelen, size_t max)
{
    unsigned char *workripe = ripe;
    size_t workripelen = ripelen;
    if (version >= 2 && version < 4)
    {
        /* version 2以上4未満 */
        if (ripelen != 20)
        {
            return NULL;
        }
        if (max < 20)
        {
            if (memcmp(ripe, "\0\0", 2) == 0)
            {
                workripe = ripe + 2;
                workripelen -= 2;
            }
            else if (memcmp(ripe, "\0", 1) == 0)
            {
                workripe = ripe + 1;
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
        /* version 4 */
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
    size_t storedBinaryDataLen
        = variantVersion->length + variantStream->length + workripelen + 4;
    unsigned char *storedBinaryData = malloc(
        variantVersion->length + variantStream->length + workripelen + 4);
    memcpy(storedBinaryData, variantVersion->data, variantVersion->length);
    memcpy(storedBinaryData + variantVersion->length, variantStream->data,
           variantStream->length);
    memcpy(storedBinaryData + variantVersion->length + variantStream->length,
           workripe, workripelen);

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
        memcpy(storedBinaryData + variantVersion->length
                   + variantStream->length + workripelen,
               cache64, 4);
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
char *encodeAddress(uint64_t version, uint64_t stream, unsigned char *ripe,
                    size_t ripelen)
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

char *encodeWIF(PrivateKey *key)
{
    unsigned char rawkey[37] = { 0x80U, 0 };
    unsigned char hash[EVP_MAX_MD_SIZE] = "";

    memcpy(rawkey + 1, key, 32);

    const EVP_MD *sha256 = EVP_sha256();
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit(ctx, sha256);
    EVP_DigestUpdate(ctx, rawkey, 33);
    EVP_DigestFinal(ctx, hash, NULL);
    EVP_DigestInit(ctx, sha256);
    EVP_DigestUpdate(ctx, hash, 32);
    EVP_DigestFinal(ctx, hash, NULL);
    EVP_MD_CTX_free(ctx);
    ctx = NULL;

    memcpy(rawkey + 33, hash, 4);

    return base58encode(rawkey, 37);
}

char *formatKey(char *address, char *privateSigningKeyWIF,
                char *privateEncryptionKeyWIF)
{
    char *buf = malloc(301);
    memset(buf, 0, 301);
    snprintf(buf, 301,
             "[%s]\nlabel = relpace this label\nenabled = true\ndecoy = "
             "false\nnoncetrialsperbyte = 1000\npayloadlengthextrabytes = "
             "1000\nprivsigningkey = %s\nprivencryptionkey = %s\n",
             address, privateSigningKeyWIF, privateEncryptionKeyWIF);
    return buf;
}

int exportAddress(PrivateKey *privateSigningKey, PublicKey *publicSigningKey,
                  PrivateKey *privateEncryptionKey,
                  PublicKey *publicEncryptionKey, unsigned char *ripe)
{
    // 秘密鍵をWIF(Wallet import format)にエンコード
    char *privateSigningKeyWIF = encodeWIF(privateSigningKey);
    char *privateEncryptionKeyWIF = encodeWIF(privateEncryptionKey);

    // generate version3 address
    char *address3 = encodeShorterV3Address(ripe, 20);
    char *formatedV3
        = formatKey(address3, privateSigningKeyWIF, privateEncryptionKeyWIF);
    printf("%s\n", formatedV3);
    free(formatedV3);
    free(address3);

    // generate version4 address
    char *address4 = encodeV4Address(ripe, 20);
    char *formatedV4
        = formatKey(address4, privateSigningKeyWIF, privateEncryptionKeyWIF);
    printf("%s\n", formatedV4);
    free(formatedV4);

    free(privateEncryptionKeyWIF);
    free(privateSigningKeyWIF);
    free(address4);

    /*
    char ripehexstr[41] = "";
    for (size_t i = 0, offset = 0; i < 20; i++, offset += 2)
    {
        snprintf(ripehexstr + offset, 3, "%02x", ripe[i]);
    }
    char sigkeyhexstr[65] = "";
    for (size_t i = 0, offset = 0; i < 65; i++, offset += 2)
    {
        snprintf(sigkeyhexstr + offset, 3, "%02x", privateSigningKey[i]);
    }
    char enckeyhexstr[65] = "";
    for (size_t i = 0, offset = 0; i < 65; i++, offset += 2)
    {
        snprintf(enckeyhexstr + offset, 3, "%02x", privateEncryptionKey[i]);
    }
    char buf[227];
    snprintf(buf, 227, "ripe = %s\nprivate signing key = %s\nprivate encryption
    key = %s\n\n", ripehexstr, sigkeyhexstr, enckeyhexstr); fputs(buf, stdout);
    */
    return EXIT_SUCCESS;
}

int getPublicKey(PublicKey *pubKey, PrivateKey *priKey)
{
    EC_GROUP *secp256k1 = EC_GROUP_new_by_curve_name(NID_secp256k1);
    BN_CTX *ctx = BN_CTX_new();
    BN_CTX_start(ctx);
    BIGNUM *prikeybn = BN_CTX_get(ctx);
    if (BN_bin2bn(*priKey, 32, prikeybn) == NULL)
    {
        return EXIT_FAILURE;
    }
    EC_POINT *pubkeyp = EC_POINT_new(secp256k1);
    EC_POINT_mul(secp256k1, pubkeyp, prikeybn, NULL, NULL, ctx);
    EC_POINT_point2oct(secp256k1, pubkeyp, POINT_CONVERSION_UNCOMPRESSED, *pubKey, 65, ctx);
    return EXIT_SUCCESS;
}
