
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>

#include <stdio.h>

/**
 * アドレスとポート番号を指定されたストリームへ表示する。
 * <I> adrinf: アドレス情報
 * <I> stream: 出力ストリーム
 */
void print_addrinfo0(struct addrinfo *adrinf, FILE *stream)
{
  char hbuf[NI_MAXHOST]; /* 返されるアドレスを格納する */
  char sbuf[NI_MAXSERV]; /* 返されるポート番号を格納する */
  int rc;
  char *protocol = NULL;
  switch (adrinf->ai_protocol)
  {
  case IPPROTO_TCP:
    protocol = "TCP";
    break;
  case IPPROTO_UDP:
    protocol = "UDP";
    break;
  default:
    protocol = "UNKNOWN";
    break;
  }

  /* アドレス情報に対応するアドレスとポート番号を得る */
  rc = getnameinfo(adrinf->ai_addr, adrinf->ai_addrlen,
                   hbuf, sizeof(hbuf),
                   sbuf, sizeof(sbuf),
                   NI_NUMERICHOST | NI_NUMERICSERV);
  if (rc != 0)
  {
    perror("getnameinfo");
    return;
  }

  fprintf(stream, "[%s]:%s(%s)\n", hbuf, sbuf, protocol);
}

/**
 * アドレスとポート番号を標準出力ストリームへ表示する。
 * <I> adrinf: アドレス情報
 */
void print_addrinfo(struct addrinfo *adrinf)
{
  print_addrinfo0(adrinf, stdout);
}
