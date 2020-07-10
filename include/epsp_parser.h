
#ifndef EPSP_PARSER_H
#define EPSP_PARSER_H

#include "string_list.h"
#include "string_array.h"
#include "epsp_type.h"

int epsp_packet_parse(epsp_packet *packet, char *line);

epsp_packet *epsp_packet_new();
void epsp_packet_free(epsp_packet *packet);

// parser & unparser
// epsp_builder.h

#endif
