
#pragma once
#ifndef BM_PROTOCOL_H
#define BM_PROTOCOL_H
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
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

extern void encodeNetworkAddress(unsigned char *addr, struct sockaddr_storage *local_addr);
extern void encodeTimeAndStreamInNetworkAddress(unsigned char *addr,
                                                uint64_t time,
                                                uint32_t stream);

extern void printNetworkAddress(unsigned char *addr, size_t addrlen);
#endif // BM_PROTOCOL_H
