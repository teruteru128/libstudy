
#pragma once
#ifndef BM_PROTOCOL_H
#define BM_PROTOCOL_H
struct message
{
    char magic[4];
    char command[12];
    // payload length
    uint32_t length;
    uint32_t checksum;
    unsigned char *payload;
};

extern struct message *parse_message(const unsigned char *data, size_t data_len);
extern void free_message(struct message *msg);
#endif // BM_PROTOCOL_H
