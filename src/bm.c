
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
#include <openssl/opensslv.h>
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

size_t get_varint_size(uint64_t u)
{
    if (u < 0xfd)
    {
        return 1;
    }
    else if (u <= 0xffff)
    {
        return 3;
    }
    else if (u <= 0xffffffff)
    {
        return 5;
    }

    // u <= 0xffffffffffffffffULL
    return 9;
}

/*
 * TODO: メモリ領域確保と書き込みを分割すると一括でメモリ領域を確保できる
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L63
 * https://docs.python.org/ja/3/library/struct.html
 */
uint8_t *encodeVarint(uint8_t *out, uint64_t len)
{
    // TODO endian関数使ってエンコーディング
    if (out == NULL)
    {
        return NULL;
    }
    if (len < 0xfd)
    {
        out[0] = (unsigned char)len;
    }
    else if (len <= 0xffff)
    {
        out[0] = 0xfd;
        out[1] = (unsigned char)(len & 0xff);
        out[2] = (unsigned char)((len >> 8) & 0xff);
    }
    else if (len <= 0xffffffff)
    {
        out[0] = 0xfe;
        out[1] = (unsigned char)(len & 0xff);
        out[2] = (unsigned char)((len >> 8) & 0xff);
        out[3] = (unsigned char)((len >> 16) & 0xff);
        out[4] = (unsigned char)((len >> 24) & 0xff);
    }
    else if (len <= 0xffffffffffffffffULL)
    {
        out[0] = 0xff;
        out[1] = (unsigned char)(len & 0xff);
        out[2] = (unsigned char)((len >> 8) & 0xff);
        out[3] = (unsigned char)((len >> 16) & 0xff);
        out[4] = (unsigned char)((len >> 24) & 0xff);
        out[5] = (unsigned char)((len >> 32) & 0xff);
        out[6] = (unsigned char)((len >> 40) & 0xff);
        out[7] = (unsigned char)((len >> 48) & 0xff);
        out[8] = (unsigned char)((len >> 56) & 0xff);
    }
    else
    {
        // 長すぎる場合はエラー
        return NULL;
    }
    return out;
}

size_t get_varstr_size(const char *str)
{
    size_t length = strlen(str);
    return get_varint_size(length) + length;
}

uint8_t *encodeVarStr(uint8_t *out, const char *str)
{
    if(out == NULL)
    {
        return NULL;
    }
    size_t len = strlen(str);
    encodeVarint(out, len);
    size_t offset = get_varint_size(len);
    memcpy(out + offset, str, len);
    return out;
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
    size_t variantVersionoutlen = get_varint_size(version);
    size_t variantStreamoutlen = get_varint_size(stream);
    size_t storedBinaryDataLen = variantVersionoutlen + variantStreamoutlen + workripelen + 4;
    unsigned char *storedBinaryData = malloc(storedBinaryDataLen);
    encodeVarint(storedBinaryData, version);
    encodeVarint(storedBinaryData + variantVersionoutlen, stream);
    memcpy(storedBinaryData + variantVersionoutlen + variantStreamoutlen, workripe, workripelen);

    {
        // make checksum
        EVP_MD *sha512 = EVP_MD_fetch(NULL, "SHA512", NULL);
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
        EVP_MD_free(sha512);
        memcpy(storedBinaryData + variantVersionoutlen + variantStreamoutlen + workripelen,
               cache64, 4);
    }

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
    unsigned char rawkey[37] = {0x80U, 0};
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
    char *formatedV3 = formatKey(address3, privateSigningKeyWIF, privateEncryptionKeyWIF);
    printf("%s\n", formatedV3);
    free(formatedV3);
    free(address3);

    // generate version4 address
    char *address4 = encodeV4Address(ripe, 20);
    char *formatedV4 = formatKey(address4, privateSigningKeyWIF, privateEncryptionKeyWIF);
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

int deriviedPrivateKey(unsigned char *out, const char *passphrase,
                       const int64_t nonce)
{
    const EVP_MD *m = EVP_sha512();
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    EVP_DigestInit_ex2(ctx, m, NULL);
#else
    EVP_DigestInit_ex(ctx, m, NULL);
#endif
    EVP_DigestUpdate(ctx, passphrase, strlen(passphrase));
    size_t len = get_varint_size((uint64_t)nonce);
    unsigned char *varintout = malloc(len);
    encodeVarint(varintout, (uint64_t)nonce);
    EVP_DigestUpdate(ctx, varintout, len);
    free(varintout);
    EVP_DigestFinal_ex(ctx, out, NULL);
    EVP_MD_CTX_free(ctx);
    return 0;
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
    BN_CTX_end(ctx);
    BN_CTX_free(ctx);
    EC_GROUP_free(secp256k1);
    return EXIT_SUCCESS;
}
