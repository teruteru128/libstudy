
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <byteswap.h>

/**
 * アドレスとポート番号を指定されたストリームへ表示する。
 * <I> adrinf: アドレス情報
 * <I> stream: 出力ストリーム
 */
void print_addrinfo0(struct addrinfo *adrinf, FILE *stream)
{
  char hbuf[NI_MAXHOST]; /* 返されるアドレスを格納する */
  char sbuf[NI_MAXSERV]; /* 返されるポート番号を格納する */
  char protocol[16] = "";
  switch (adrinf->ai_protocol)
  {
  case IPPROTO_TCP:
    strncpy(protocol, "TCP", 4);
    break;
  case IPPROTO_UDP:
    strncpy(protocol, "UDP", 4);
    break;
  default:
    strncpy(protocol, "UNKNOWN", 8);
    break;
  }

  /* アドレス情報に対応するアドレスとポート番号を得る */
  int rc = getnameinfo(adrinf->ai_addr, adrinf->ai_addrlen,
                       hbuf, sizeof(hbuf),
                       sbuf, sizeof(sbuf),
                       NI_NUMERICHOST | NI_NUMERICSERV);
  if (rc != 0)
  {
    perror("getnameinfo");
    return;
  }

  fprintf(stream, "[%s]:%s(%s), flags : %d, family : %d, socktype : %d, protocol : %d", hbuf, sbuf, protocol, adrinf->ai_flags, adrinf->ai_family, adrinf->ai_socktype, adrinf->ai_protocol);
  /*
  if (adrinf->ai_family == AF_INET6)
  {
    struct sockaddr_in6 *addr = (struct sockaddr_in6 *)adrinf->ai_addr;
    fprintf(stream, ", port : %d", bswap_16(addr->sin6_port));
  }
  else if (adrinf->ai_family == AF_INET)
  {
    struct sockaddr_in *addr = (struct sockaddr_in *)adrinf->ai_addr;
    fprintf(stream, ", port : %d", bswap_16(addr->sin_port));
  }
  */
  if (adrinf->ai_canonname != NULL)
    fprintf(stream, ", canonname : %s", adrinf->ai_canonname);
  fputs("\n", stream);
}

/**
 * アドレスとポート番号を標準出力ストリームへ表示する。
 * <I> adrinf: アドレス情報
 */
void print_addrinfo(struct addrinfo *adrinf)
{
  print_addrinfo0(adrinf, stdout);
}
