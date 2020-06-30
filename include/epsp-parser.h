
#ifndef EPSP_PARSER_H
#define EPSP_PARSER_H

#include "string_list.h"
#include "string_array.h"
#include "epsp_data_type.h"

epsp_packet* epsp_packet_parse(char* line);

void epsp_packet_free(epsp_packet* packet);

// parser & unparser
// epsp_builder.h

#endif

