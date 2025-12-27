
#pragma once
#ifndef BM_PROTOCOL_H
#define BM_PROTOCOL_H
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <bm_network.h>

extern const unsigned char magicbytes[4];
extern const unsigned char empty_payload_checksum[4];

struct message
{
    char magic[4];
    char command[12];
    // payload length
    uint32_t length;
    uint32_t checksum;
    unsigned char *payload;
};

struct version_message
{
    uint32_t version;
    uint64_t services;
    uint64_t timestamp;
    unsigned char addr_recv[26];
    unsigned char addr_from[26];
    uint64_t nonce;
    char *user_agent;
    size_t stream_numbers_len;
    uint64_t *stream_numbers;
};

struct addr_message
{
    uint64_t count;
    struct address_info *addresses;
};

struct inventory_item
{
    unsigned char object_hash[32];
};

struct inventory_message
{
    uint64_t count;
    struct inventory_item *items;
};

extern struct message *parse_message(const unsigned char *data, size_t data_len);
extern void free_message(struct message *msg);

extern void encodeNetworkAddress(unsigned char *addr, struct sockaddr_storage *local_addr);
extern void encodeTimeAndStreamInNetworkAddress(unsigned char *addr,
                                                uint64_t time,
                                                uint32_t stream);

extern void printNetworkAddress(unsigned char *addr, size_t addrlen);
extern void parseVersionMessage(unsigned char *payload, size_t payload_len, struct version_message *out_msg);
extern void process_command(struct fd_data *data, struct message *msg);
extern size_t get_version_message_size(const char *user_agent_str);
extern size_t get_version_payload_size(const char *user_agent_str);
extern unsigned char *create_version_payload(uint8_t *out, const char *user_agent_str, int version,
                                           struct sockaddr_storage *peer_addr, struct sockaddr_storage *local_addr);
extern uint8_t *new_version_message(const char *user_agent_str, int version, struct sockaddr_storage *peer_addr, struct sockaddr_storage *local_addr);
#endif // BM_PROTOCOL_H
