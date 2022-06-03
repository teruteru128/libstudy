
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "bitmessage.h"
#include "bm.h"
#include "bmapi.h"
#include "nlz.h"
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

/*
 *
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L63
 * https://docs.python.org/ja/3/library/struct.html
 */
int encodeVarint(uint64_t u, unsigned char **out, size_t *outlen)
{
    if (out == NULL || outlen == NULL)
    {
        return 1;
    }
    if (u < 253)
    {
        *outlen = sizeof(char);
        *out = malloc(*outlen);
        **out = (int8_t)u;
        return 0;
    }
    if (253 <= u && u < 65536)
    {
        *outlen = sizeof(char) + sizeof(uint16_t);
        *out = malloc(*outlen);
        **out = 0xfd;
        *((uint16_t *)(*out + 1)) = htobe16((uint16_t)u);
        return 0;
    }
    if (65536 <= u && u < 4294967296L)
    {
        *outlen = sizeof(char) + sizeof(uint32_t);
        *out = malloc(*outlen);
        **out = 0xfe;
        *((uint32_t *)(*out + 1)) = htobe32((uint32_t)u);
        return 0;
    }
    if (4294967296L <= u && u <= 18446744073709551615UL)
    {
        *outlen = sizeof(char) + sizeof(uint64_t);
        *out = malloc(*outlen);
        **out = 0xff;
        *((uint64_t *)(*out + 1)) = htobe64((uint64_t)u);
        return 0;
    }
    // おそらくここには来ない
    return 2;
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
    unsigned char *variantVersionout = NULL;
    size_t variantVersionoutlen = 0;
    unsigned char *variantStreamout = NULL;
    size_t variantStreamoutlen = 0;
    encodeVarint(version, &variantVersionout, &variantVersionoutlen);
    encodeVarint(stream, &variantStreamout, &variantStreamoutlen);
    size_t storedBinaryDataLen
        = variantVersionoutlen + variantStreamoutlen + workripelen + 4;
    unsigned char *storedBinaryData = malloc(
        variantVersionoutlen + variantStreamoutlen + workripelen + 4);
    memcpy(storedBinaryData, variantVersionout, variantVersionoutlen);
    memcpy(storedBinaryData + variantVersionoutlen, variantStreamout,
           variantStreamoutlen);
    memcpy(storedBinaryData + variantVersionoutlen + variantStreamoutlen,
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
        memcpy(storedBinaryData + variantVersionoutlen
                   + variantStreamoutlen + workripelen,
               cache64, 4);
    }
    free(variantVersionout);
    free(variantStreamout);

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

/**
 * @brief
 *
 * @param key データ型をPrivateKey *にするか unsigned char*にするか判断に悩む。
 * PrivateKey: いちいち変換する手間を掛けたくない
 * unsigned char*: 固定長のデータのサイズ(32byte)がわからない
 * @return char*
 */
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
    // FIXME: 確保する領域はもっと小さくてもいい
    char *buf = calloc(1, 512);
    snprintf(buf, 500,
             "[%s]\nlabel = relpace this label\nenabled = true\ndecoy = "
             "false\nnoncetrialsperbyte = 1000\npayloadlengthextrabytes = "
             "1000\nprivsigningkey = %s\nprivencryptionkey = %s\n",
             address, privateSigningKeyWIF, privateEncryptionKeyWIF);
    char *tmp = realloc(buf, strlen(buf) + 1);
    if (tmp == NULL)
    {
        return buf;
    }
    return tmp;
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
    EC_POINT_point2oct(secp256k1, pubkeyp, POINT_CONVERSION_UNCOMPRESSED,
                       *pubKey, 65, ctx);
    return EXIT_SUCCESS;
}
