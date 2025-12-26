
#define _GNU_SOURCE
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <byteswap.h>
#include <stdio.h>
#include <string.h>

#define PROTOCOL_LEN 16

/**
 * アドレスとポート番号を指定されたストリームへ表示する。
 * <I> adrinf: アドレス情報
 * <I> stream: 出力ストリーム
 */
void printaddrinfo0(struct addrinfo *adrinf, FILE *stream)
{
    if (adrinf == NULL || stream == NULL)
    {
        return;
    }
    char hbuf[NI_MAXHOST]; /* 返されるアドレスを格納する */
    char sbuf[NI_MAXSERV]; /* 返されるポート番号を格納する */
    char protocol[PROTOCOL_LEN] = "";
    switch (adrinf->ai_protocol)
    {
    case IPPROTO_TCP:
        strncpy(protocol, "TCP", PROTOCOL_LEN);
        break;
    case IPPROTO_UDP:
        strncpy(protocol, "UDP", PROTOCOL_LEN);
        break;
    default:
        snprintf(protocol, PROTOCOL_LEN, "UNKNOWN(%d)", adrinf->ai_protocol);
        break;
    }

    /* アドレス情報に対応するアドレスとポート番号を得る */
    int rc
        = getnameinfo(adrinf->ai_addr, adrinf->ai_addrlen, hbuf, sizeof(hbuf),
                      sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    if (rc != 0)
    {
        perror("getnameinfo");
        return;
    }

    fprintf(stream,
            "[%s]:%s(%s), addrlen : %u, flags : %d, family : %d, socktype : "
            "%d, protocol : %d",
            hbuf, sbuf, protocol, adrinf->ai_addrlen, adrinf->ai_flags,
            adrinf->ai_family, adrinf->ai_socktype, adrinf->ai_protocol);
    /*
     * if (adrinf->ai_family == AF_INET6)
     * {
     *   struct sockaddr_in6 *addr = (struct sockaddr_in6 *)adrinf->ai_addr;
     *   fprintf(stream, ", port : %d", bswap_16(addr->sin6_port));
     * }
     * else if (adrinf->ai_family == AF_INET)
     * {
     *   struct sockaddr_in *addr = (struct sockaddr_in *)adrinf->ai_addr;
     *   fprintf(stream, ", port : %d", bswap_16(addr->sin_port));
     * }
     */
    if (adrinf->ai_canonname != NULL)
        fprintf(stream, ", canonname : %s", adrinf->ai_canonname);
    fputs("\n", stream);
}

/**
 * アドレスとポート番号を標準出力ストリームへ表示する。
 * <I> adrinf: アドレス情報
 */
void printaddrinfo(struct addrinfo *adrinf) { printaddrinfo0(adrinf, stdout); }
