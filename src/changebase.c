
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <bm.h>
#include <byteswap.h>
#include <err.h>
#include <openssl/evp.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz"
#define BASE_58 58

static char divmod58(unsigned char *number, size_t length, size_t startAt)
{
    int remainder = 0;
    int digit256;
    int temp;
    div_t d;
    for (size_t i = startAt; i < length; i++)
    {
        digit256 = number[i] & 0xFF;
        temp = (remainder << 8) + digit256;

        d = div(temp, BASE_58);
        number[i] = (unsigned char)d.quot;

        remainder = d.rem;
    }

    return (char)remainder;
}

/*
 * どうやったらこんな使い勝手のいいライブラリが作れるんですか？
 * https://github.com/Bitmessage/PyBitmessage/blob/8659c5313d54ea8c1f899f8a4e0bc8c02ae5dab3/src/pyelliptic/arithmetic.py#L41
 * そもそもPythonのencode関数の引数valが多倍長整数臭いんだよねえ……
 * →Python 2.x系はint型が少なくとも32ビット精度の整数、longが多倍長整数
 * Python　3.x系だとintが多倍長整数
 * https://docs.python.org/ja/2.7/library/stdtypes.html#numeric-types-int-float-long-complex
 * C言語だとmpz_tとかBIGNUMとかの使用が前提やぞ
 * def encode(val, base, minlen=0):
 * def decode(string, base):
 * def changebase(string, frm, to, minlen=0):
 *
 * 生バイナリを渡して文字列に変換する
 * char *encode(char *, size_t len, int base, size_t minlen);
 *
 * 文字列を渡してバイナリに変換する
 * 本当にNULL終端された文字列だと仮定していいのか？
 * NULL終端されていないと仮定して文字列長も受け取ったほうが便利じゃないか？
 * stringの実際の長さと引数lenで渡した文字列長が一致しない場合は……呼び出し側の責任なので問題ないのか
 * そもそも返すバイナリの長さがわからないのが問題だな？
 * データ長をデータ本体ともども引数経由で返して返り値はエラー判定に使うか？
 * char *decode(char *string, int base);
 * int decode(char *string, size_t stringlen, int base, char **val, size_t
 * *vallen); char *changebase(char *string, size_t len, int from, int to,
 * size_t minlen); int changebase(char *string, size_t len, int from, int to,
 * size_t minlen, char **val, size_t *vallen);
 *
 * JavaとかPythonとかだと配列にlengthが付いてるから楽でいいなあ……
 *
 * そもそも文字列と生バイナリの区別がデータ型でできないのが割と辛い
 * ……Javaでも変わらんか
 *
 * バイナリがunsigned charで文字列が(signed) charをよく使ってる気がする
 *
 * iconv(3)に習うならoutputのbufも呼び出し側が指定すべきなんだよな
 * iconvの実装が古い可能性
 * 2, 10, 16, 58, 256
 *
 * 生バイナリ(256進数)→58進数:1.365倍
 * 生バイナリ(256進数)→16進数:2倍の領域
 * 生バイナリ(256進数)→10進数:2.408倍の領域
 * 生バイナリ(256進数)→10進数:8倍の領域
 * 58進数→16進数:1.464倍の領域
 * 58進数→10進数:1.763倍の領域
 * 58進数→2進数:5.858倍の領域
 * 16進数→10進数:1.204倍の領域
 * 16進数→2進数:4倍の領域
 * 10進数→2進数:3.322倍の領域
 * 計算めんどくさいし誤差でオーバーランしたら嫌だし
 * 小数点以下切り上げでとりあえず確保してreallocいいんじゃないかな( ˘ω˘)
 * ->ローカル変数で領域を確保してstrdupするなりなんなりでもいい気がする……文字列ならな
 *
 * encodeBase58
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L14
 *
 * decodeBase58
 * https://github.com/Bitmessage/PyBitmessage/blob/d09782e53d3f42132532b6e39011cd27e7f41d25/src/addresses.py#L33
 *
 * TODO: base58.cに移動する
 *
 */
/**
 * @brief バイナリを渡して58進数文字列に変換する
 *
 * @param input
 * @param length
 * @return char*
 */
char *base58encode(const unsigned char *input, const size_t length)
{
    if (input == NULL || length == 0)
    {
        return NULL;
    }

    unsigned char *work = malloc(length);
    memcpy(work, input, length);
    size_t zeroCount = 0;
    while (zeroCount < length && work[zeroCount] == 0)
        zeroCount++;
    size_t templen = length * 2;
    char *temp = malloc(templen);
    size_t j = templen;
    memset(temp, 0, j);

    size_t startAt = zeroCount;
    while (startAt < length)
    {
        int mod = divmod58(work, length, startAt);
        if (work[startAt] == 0)
        {
            ++startAt;
        }

        temp[--j] = ALPHABET[mod];
    }
    free(work);
    work = NULL;
    while (j < templen && temp[j] == ALPHABET[0])
    {
        ++j;
    }
    while (zeroCount--)
    {
        temp[--j] = ALPHABET[0];
    }
    size_t outputlen = templen - j;
    char *output = malloc(outputlen + 1);
    memset(output, 0, outputlen + 1);
    memcpy(output, &temp[j], outputlen);
    /*
    char *tmp = realloc(output, strlen(output) + 1);
    if (!tmp)
    {
        free(output);
        err(EXIT_FAILURE, "realloc in base58encode");
    }
    output = tmp;
    */
    free(temp);
    return output;
}

#define TABLE                                                                 \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x00\x00\x00\x00\x00\x00"        \
    "\x00\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x0a\x0b\x0c\x0d\x0e\x0f\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"        \
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

/*
 * strを16進数文字列としてパースします。
 * jsonパーサ
 * https://mattn.kaoriya.net/software/lang/c/20130710214647.htm
 */
size_t parseHex(unsigned char **out, const char *str)
{
    size_t length = strlen(str) / 2;
    size_t i = 0;
    unsigned char *data = calloc(length, sizeof(char));
    if (!data)
    {
        perror("calloc in parseHex");
        exit(1);
    }
    for (i = 0; i < length; i++)
    {
        data[i] = (unsigned char)((TABLE[str[2 * i] & 0xff] << 4)
                                  | (TABLE[str[2 * i + 1] & 0xff]));
    }
    *out = data;
    return length;
}

#define BASE32_TABLE "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"

const char base32table[33] = BASE32_TABLE;

// TODO base32decodeの実装しんどいれす^q^
// changebaseにbase32を導入？
/**
 * @brief
 * https://qiita.com/waaaaRapy/items/8549890bda4f8ad33963#%E5%8B%95%E4%BD%9C%E3%83%86%E3%82%B9%E3%83%88
 *
 * @param src
 * @param srclen
 * @param out
 * @param outlen
 * @return int
 */
int base32decode(const char *src, unsigned char *out, size_t *outlen)
{
    if (src == NULL)
    {
        return 1;
    }
    if (out == NULL && out == NULL)
    {
        return 1;
    }
    if (out == NULL && outlen != NULL)
    {
        *outlen = (strlen(src) * 5 / 8);
        return 0;
    }

    return 1;
}
