
#include <java_random.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
// "sys/random.h" includes "endian.h" via "sys/types.h".
#include <sys/random.h>

#if OPENSSL_VERSION_PREREQ(3, 0)
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/types.h>
#endif

static EVP_PKEY *generatekey()
{
    EVP_PKEY *ephem = NULL;
#if OPENSSL_VERSION_PREREQ(3, 0)
    EVP_PKEY_CTX *keygenctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    const OSSL_PARAM params[]
        = { OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, "secp256k1", 0),
            OSSL_PARAM_END };
    EVP_PKEY_CTX_set_params(keygenctx, params);
    EVP_PKEY_CTX_set_params(keygenctx, params);
    EVP_PKEY_keygen_init(keygenctx);
    EVP_PKEY_keygen(keygenctx, &ephem);
    EVP_PKEY_CTX_free(keygenctx);
#else
    // TODO まだよ
#endif
    return ephem;
}

/**
 * @brief
 * https://github.com/Bitmessage/PyBitmessage/blob/6dee19fe2668f768e74d1bdeb4431e23c7dcbbd6/src/pyelliptic/ecc.py#L462
 *
 */
int encrypt(unsigned char *message, size_t messagelen, unsigned char *pubkey,
            unsigned char *out, size_t *outlen)
{

    // 一時鍵を作る
    // Generate a dummy privkey and derive the pubkey
    EVP_PKEY *ephem = generatekey();
    // 一時鍵と引数の公開鍵で共有秘密を作る
    // EVP_PKEY_derive_init / EVP_KEYEXCH
    unsigned char shared_secret[EVP_MAX_MD_SIZE];
    size_t sslen = EVP_MAX_MD_SIZE;
    // SHA-512でハッシュを作り前半32バイトが暗号化鍵、後半32バイトがMAC鍵
    unsigned char key[EVP_MAX_MD_SIZE];
#if OPENSSL_VERSION_PREREQ(3, 0)
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
    unsigned char priv[32] = "";
    unsigned char *pubkey = NULL;
    int64_t seed = 0;
    size_t dummyMessageLength = 0;
    unsigned char *dummyMessage = 0;

    switch (stealthLevel)
    {
    case 2:
        // get private key
        getrandom(priv, 32, GRND_RANDOM);
        // get 0xXXXXXXXXXXXX0000L
        getrandom(&seed, 6, GRND_RANDOM);
        // convert to 0x0000XXXXXXXXXXXXL
        seed = initialScramble(le64toh(seed));
        dummyMessageLength = nextIntWithBounds(&seed, 567) + 234;
        dummyMessage = malloc(dummyMessageLength);
        getrandom(dummyMessage, dummyMessageLength, 0);
        acktype = 2;
        version = 1;
        break;

    case 1:
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
        getrandom(ackdata, ackdatalen, GRND_NONBLOCK);
        acktype = 0;
        version = 4;
        break;

    default:
        ackdatalen = 32;
        ackdata = malloc(ackdatalen);
#if defined(GRND_INSECURE)
        getrandom(ackdata, ackdatalen, GRND_INSECURE);
#else
        getrandom(ackdata, ackdatalen, 0);
#endif
        acktype = 2;
        version = 1;
        break;
    }

    return 0;
}
