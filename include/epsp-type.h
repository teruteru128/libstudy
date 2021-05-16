
#ifndef EPSP_TYPE_H
#define EPSP_TYPE_H

#include <stdint.h>
#include "string-array.h"

typedef struct server_point
{
    char *address;
    uint16_t port;
} server_point;

typedef struct peer_data
{
    server_point address;
    uint16_t peer_id;
} peer_data;

typedef struct epsp_packet epsp_packet_t;
/**
 * TODO: 送受信両方に使いたい
 * TODO: move to epsp_protocol.h
 */
typedef struct epsp_packet
{
    int code;
    int hop_count;
    string_array *data;
    size_t data_length;
    epsp_packet_t *next;
} epsp_packet_t;

enum data_require
{
    NONE,
    UNNECESSARY,
    REQUIRE
};

struct protocol_code_t
{
    int code;
    char str[24];
    enum data_require require;
    struct protocol_code_t *next;
};

struct protocol_code_t code_list[]
    = { { .code = 112, .str = "", .require = UNNECESSARY },
        { .code = 113, .str = "", .require = UNNECESSARY },
        { .code = 0, .str = NULL, .require = NONE } };
// HOP_COUNT & MAX_HOP_COUNT

#endif
