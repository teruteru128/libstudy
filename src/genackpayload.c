
#include <java_random.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/types.h>
#endif

/**
 * @brief
 * https://github.com/Bitmessage/PyBitmessage/blob/6dee19fe2668f768e74d1bdeb4431e23c7dcbbd6/src/pyelliptic/ecc.py#L462
 *
 */
int encrypt(unsigned char *message, size_t messagelen, unsigned char *pubkey,
            unsigned char *out, size_t *outlen)
{

    // 一時鍵を作る
    EVP_PKEY *ephem = NULL;
    EVP_PKEY_CTX *keygenctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    OSSL_PARAM_BLD *param_bld = OSSL_PARAM_BLD_new();
    OSSL_PARAM_BLD_push_utf8_string(param_bld, OSSL_PKEY_PARAM_GROUP_NAME,
                                    "secp256k1", 0);
    OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(param_bld);
    EVP_PKEY_CTX_set_params(keygenctx, params);
    EVP_PKEY_keygen_init(keygenctx);
    EVP_PKEY_keygen(keygenctx, &ephem);
    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(param_bld);
    EVP_PKEY_CTX_free(keygenctx);
    // 一時鍵と引数の公開鍵で共有秘密を作る
    // EVP_PKEY_derive_init / EVP_KEYEXCH
    unsigned char shared_secret[EVP_MAX_MD_SIZE];
    size_t sslen = EVP_MAX_MD_SIZE;
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#else
    EVP_PKEY_CTX *kxctx = EVP_PKEY_CTX_new(ephem, NULL);
    EVP_PKEY_derive_init(kxctx);
    // EVP_PKEY_derive_set_peer(kxctx, peer);
    EVP_PKEY_derive(kxctx, shared_secret, &sslen);
    EVP_PKEY_CTX_free(kxctx);
#endif
    // SHA-512でハッシュを作り前半32バイトが暗号化鍵、後半32バイトがMAC鍵
    unsigned char key[EVP_MAX_MD_SIZE];
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    EVP_Q_digest(NULL, "SHA256", NULL, shared_secret, EVP_MAX_MD_SIZE, key,
                 NULL);
#else
    SHA512(NULL, 0, key);
#endif
    return 0;
}

int genAckPayload(int streamNumber, int stealthLevel, unsigned char **payload,
                  size_t *payloadlen)
{
    if (payload == NULL || payloadlen == NULL)
    {
        return 1;
    }
    int acktype = 0;
    int version = 0;
    unsigned char *ackdata = NULL;
    size_t ackdatalen = 0;

    if (stealthLevel == 2)
    {
        unsigned char priv[32];
        getrandom(priv, 32, GRND_RANDOM);
        unsigned char *pubkey;
        int64_t seed;
        // get 0xXXXXXXXXXXXX0000L
        getrandom(&seed, 6, GRND_RANDOM);
        // convert to 0x0000XXXXXXXXXXXXL
        seed = le64toh(seed);
        const size_t dummyMessageLength = nextIntWithBounds(&seed, 567) + 234;
        unsigned char *dummyMessage = malloc(dummyMessageLength);
        getrandom(dummyMessage, dummyMessageLength, 0);
        acktype = 2;
        version = 1;
    }
    else if (stealthLevel == 1)
    {
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
        getrandom(ackdata, ackdatalen, GRND_NONBLOCK);
        acktype = 0;
        version = 4;
    }
    else if (stealthLevel == 0)
    {
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
        getrandom(ackdata, ackdatalen, GRND_INSECURE);
        acktype = 2;
        version = 1;
    }

    return 0;
}
