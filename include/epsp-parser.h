
#ifndef EPSP_PARSER_H
#define EPSP_PARSER_H

#include "string-list.h"
#include "string-array.h"
#include "epsp-type.h"

//int epsp_packet_parse(epsp_packet *packet, char *line);
/*
   何を参考にすべきなんだろうか？
   FILE *とfopen方式
   regex_tとregcomp方式
   多分領域確保と開放を繰り返すからFILE *, fopen方式のほうが便利
   getaddrinfo方式は多分違う
*/
epsp_packet_t *epsp_packet_new(char *line);

//epsp_packet *epsp_packet_new();
void epsp_packet_free(epsp_packet_t *packet);

// parser & unparser
// epsp_builder.h

#endif
