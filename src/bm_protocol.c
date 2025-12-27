// プロトコルメッセージの構造化とパースの厳密化
#include <bm.h>
#include <bm_protocol.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <openssl/sha.h>
#include <errno.h>
#include <sys/random.h>
#include <time.h>
#include <unistd.h>

// BitMessageプロトコルのマジックバイト列
const unsigned char magicbytes[4] = {0xe9, 0xbe, 0xb4, 0xd9};

// チェックサム: SHA512("")の最初の4バイト
const unsigned char empty_payload_checksum[4] = {0xcf, 0x83, 0xe1, 0x35};

struct message *parse_message(const unsigned char *data, size_t data_len)
{
    if (data_len < 24)
    {
        // データが不十分
        fprintf(stderr, "Data length %zu is less than header size 24\n", data_len);
        return NULL;
    }

    int length = ntohl(*(uint32_t *)(data + 16));
    if (data_len < 24 + length)
    {
        // データが不十分
        return NULL;
    }
    struct message *msg = malloc(sizeof(struct message));
    if (msg == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    memcpy(msg->magic, data, 4);
    memcpy(msg->command, data + 4, 12);
    msg->length = length;
    msg->checksum = ntohl(*(uint32_t *)(data + 20));

    if (msg->length > 0)
    {
        msg->payload = malloc(msg->length);
        if (msg->payload == NULL)
        {
            perror("malloc");
            free(msg);
            exit(EXIT_FAILURE);
        }
        memcpy(msg->payload, data + 24, msg->length);
    }
    else
    {
        // varackなどペイロードが空のメッセージ
        msg->payload = NULL;
    }

    // チェックサムの検証
    // チェックサムはpayloadのSHA512ハッシュの最初の4バイト
    unsigned char hash1[SHA512_DIGEST_LENGTH];
    SHA512(data + 24, msg->length, hash1);
    uint32_t computed_checksum = *(uint32_t *)hash1;

    if (htonl(computed_checksum) != msg->checksum)
    {
        // チェックサム不一致
        free(msg->payload);
        free(msg);
        fprintf(stderr, "Checksum mismatch! Expected: %08x, Computed: %08x\n", msg->checksum, computed_checksum);
        return NULL;
    }

    return msg;
}

void free_message(struct message *msg)
{
    if (msg != NULL)
    {
        if (msg->payload != NULL)
        {
            free(msg->payload);
        }
        free(msg);
    }
}

void encodeTimeAndStreamInNetworkAddress(unsigned char *addr,
                                         uint64_t time,
                                         uint32_t stream)
{
    uint64_t net_time = htobe64(time);
    uint32_t net_stream = htobe32(stream);
    memcpy(addr, &net_time, 8);       // 時間をコピー
    memcpy(addr + 8, &net_stream, 4); // ストリーム番号をコピー
}

void encodeNetworkAddress(unsigned char *addr, struct sockaddr_storage *local_addr)
{
    // servicesの8バイトはゼロ埋め
    memset(addr, 0, 8);
    if (local_addr->ss_family == AF_INET)
    {
        // IPv4アドレスの場合、IPv4マッピングIPv6アドレスに変換してセット
        addr[8] = 0;
        addr[9] = 0;
        addr[10] = 0;
        addr[11] = 0;
        addr[12] = 0;
        addr[13] = 0;
        addr[14] = 0xff;
        addr[15] = 0xff;
        struct sockaddr_in *sin = (struct sockaddr_in *)local_addr;
        memcpy(addr + 16, &sin->sin_addr, 4); // IPv4アドレスをコピー
        uint16_t port = htons(sin->sin_port);
        memcpy(addr + 24, &port, 2); // ポート番号をコピー
    }
    else if (local_addr->ss_family == AF_INET6)
    {
        // IPv6アドレスの場合
        struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)local_addr;
        memcpy(addr + 8, &sin6->sin6_addr, 16); // IPv6アドレスをコピー
        uint16_t port = htons(sin6->sin6_port);
        memcpy(addr + 24, &port, 2); // ポート番号をコピー
    }
}

// versionメッセージのペイロードを表示するユーティリティ関数
static void printVersionPayload(unsigned char *payload, size_t payload_length)
{
    if (payload_length < 85)
    {
        printf("Invalid version payload length: %zu\n", payload_length);
        return;
    }
    size_t offset = 0;
    uint32_t version = ntohl(*((uint32_t *)(payload + offset)));
    offset += 4;
    uint64_t services = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    uint64_t timestamp = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    printf("Version: %u, Services: %" PRIu64 ", Timestamp: %" PRIu64 "\n", version, services, timestamp);
    printNetworkAddress(payload + offset, 26); // addr_recv
    offset += 26;
    printNetworkAddress(payload + offset, 26); // addr_from
    offset += 26;
    uint64_t nonce = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    printf("Nonce: %016" PRIx64 "\n", nonce);
    // user_agentのデコード
    uint64_t ua_len = 0;
    unsigned char *ua_ptr = payload + offset;
    if (ua_ptr[0] < 0xfd)
    {
        ua_len = ua_ptr[0];
        offset += 1;
    }
    else if (ua_ptr[0] == 0xfd)
    {
        ua_len = ua_ptr[1] | (ua_ptr[2] << 8);
        offset += 3;
    }
    else if (ua_ptr[0] == 0xfe)
    {
        ua_len = ua_ptr[1] | (ua_ptr[2] << 8) | (ua_ptr[3] << 16) | (ua_ptr[4] << 24);
        offset += 5;
    }
    else if (ua_ptr[0] == 0xff)
    {
        ua_len = ((uint64_t)ua_ptr[1]) | ((uint64_t)ua_ptr[2] << 8) | ((uint64_t)ua_ptr[3] << 16) | ((uint64_t)ua_ptr[4] << 24) |
                 ((uint64_t)ua_ptr[5] << 32) | ((uint64_t)ua_ptr[6] << 40) | ((uint64_t)ua_ptr[7] << 48) | ((uint64_t)ua_ptr[8] << 56);
        offset += 9;
    }
    char *user_agent = malloc(ua_len + 1);
    memcpy(user_agent, payload + offset, ua_len);
    user_agent[ua_len] = '\0';
    offset += ua_len;
    printf("User Agent: %s\n", user_agent);
    free(user_agent);
    // stream_numbersのデコード
    uint64_t stream_count = 0;
    unsigned char *sn_ptr = payload + offset;
    if (sn_ptr[0] < 0xfd)
    {
        stream_count = sn_ptr[0];
        offset += 1;
    }
    else if (sn_ptr[0] == 0xfd)
    {
        stream_count = sn_ptr[1] | (sn_ptr[2] << 8);
        offset += 3;
    }
    else if (sn_ptr[0] == 0xfe)
    {
        stream_count = sn_ptr[1] | (sn_ptr[2] << 8) | (sn_ptr[3] << 16) | (sn_ptr[4] << 24);
        offset += 5;
    }
    else if (sn_ptr[0] == 0xff)
    {
        stream_count = ((uint64_t)sn_ptr[1]) | ((uint64_t)sn_ptr[2] << 8) | ((uint64_t)sn_ptr[3] << 16) | ((uint64_t)sn_ptr[4] << 24) |
                       ((uint64_t)sn_ptr[5] << 32) | ((uint64_t)sn_ptr[6] << 40) | ((uint64_t)sn_ptr[7] << 48) | ((uint64_t)sn_ptr[8] << 56);
        offset += 9;
    }
    printf("Stream Count: %" PRIu64 "\n", stream_count);
    printf("Streams: ");
    for (uint64_t i = 0; i < stream_count; i++)
    {
        uint16_t stream_num = ntohs(*((uint16_t *)(payload + offset)));
        offset += 2;
        printf("%" PRIu16 " ", stream_num);
    }
    printf("\n");
}
// versionメッセージのペイロードを作成するユーティリティ関数
static unsigned char *createVersionPayload(struct sockaddr_storage *peer_addr, struct sockaddr_storage *local_addr, size_t *outlen)
{
    size_t ua_len = 22;
    size_t stream_count = 1;
    size_t payload_len = 85 + 1 + ua_len + 1 + stream_count;
    unsigned char *payload = malloc(payload_len);
    if (payload == NULL)
    {
        return NULL;
    }
    size_t offset = 0;
    // version
    uint32_t net_version = htonl(3);
    memcpy(payload + offset, &net_version, 4);
    offset += 4;
    // services
    uint64_t net_services = htobe64(1);
    memcpy(payload + offset, &net_services, 8);
    offset += 8;
    // timestamp
    uint64_t net_timestamp = htobe64((uint64_t)time(NULL));
    memcpy(payload + offset, &net_timestamp, 8);
    offset += 8;
    // addr_recv
    encodeNetworkAddress(payload + offset, peer_addr);
    offset += 26;
    // addr_from
    encodeNetworkAddress(payload + offset, local_addr);
    offset += 26;
    // nonce
    uint64_t nonce = 0;
    getrandom(&nonce, sizeof(nonce), GRND_NONBLOCK);
    uint64_t net_nonce = htobe64(nonce);
    memcpy(payload + offset, &net_nonce, 8);
    offset += 8;
    // user_agent
    const char *user_agent_str = "/TRBMTESTCLIENT:0.1.0/";
    payload[offset] = (unsigned char)ua_len;
    offset += 1;
    memcpy(payload + offset, user_agent_str, ua_len);
    offset += ua_len;
    // stream_numbers
    payload[offset] = (unsigned char)stream_count;
    offset += 1;
    payload[offset] = 1; // ストリーム番号1
    offset += 1;
    if (outlen != NULL)
    {
        *outlen = payload_len;
    }
    return payload;
}

// 26バイト版と38バイト版のnetwork addressを表示するユーティリティ関数
void printNetworkAddress(unsigned char *addr, size_t addrlen)
{
    if (addrlen != 26 && addrlen != 38)
    {
        printf("Invalid network address length: %zu\n", addrlen);
        return;
    }
    size_t offset = 0;
    uint64_t time;
    uint32_t stream;
    if (addrlen == 38)
    {
        // 38バイト版は最初の8バイトがtime
        time = be64toh(*((uint64_t *)(addr + offset)));
        offset += 8;
        // 次の4バイトがstream
        stream = ntohl(*((uint32_t *)(addr + offset)));
        offset += 4;
    }
    else
    {
        // 26バイト版はtimeとstream情報なし、timeとstreamを0に設定
        time = 0;
        stream = 0;
    }
    uint64_t services = be64toh(*((uint64_t *)(addr + offset)));
    offset += 8;
    unsigned char ip[16];
    memcpy(ip, addr + offset, 16);
    offset += 16;
    uint16_t port = ntohs(*((uint16_t *)(addr + offset)));
    offset += 2;

    printf("Time: %" PRIu64 ", Stream: %" PRIu32 ", Services: %" PRIu64 ", ", time, stream, services);
    // IPアドレスの表示
    if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 &&
        ip[4] == 0 && ip[5] == 0 && ip[6] == 0 && ip[7] == 0 &&
        ip[8] == 0 && ip[9] == 0 && ip[10] == 0xff && ip[11] == 0xff)
    {
        // IPv4-mapped IPv6 address
        printf("IPv4 Address: %u.%u.%u.%u, Port: %u\n", ip[12], ip[13], ip[14], ip[15], port);
    }
    else
    {
        // IPv6 address
        char ipv6_str[40];
        snprintf(ipv6_str, sizeof(ipv6_str),
                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                 "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                 ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7],
                 ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15]);
        printf("IPv6 Address: %s, Port: %u\n", ipv6_str, port);
    }
}

uint64_t decodeVarint(unsigned char *data, size_t *consumed_bytes)
{
    uint64_t value = 0;
    size_t offset = 0;
    if (data[0] < 0xfd)
    {
        value = data[0];
        offset = 1;
    }
    else if (data[0] == 0xfd)
    {
        value = data[1] | (data[2] << 8);
        offset = 3;
    }
    else if (data[0] == 0xfe)
    {
        value = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
        offset = 5;
    }
    else if (data[0] == 0xff)
    {
        value = ((uint64_t)data[1]) | ((uint64_t)data[2] << 8) | ((uint64_t)data[3] << 16) | ((uint64_t)data[4] << 24) |
                ((uint64_t)data[5] << 32) | ((uint64_t)data[6] << 40) | ((uint64_t)data[7] << 48) | ((uint64_t)data[8] << 56);
        offset = 9;
    }
    if (consumed_bytes != NULL)
    {
        *consumed_bytes = offset;
    }
    return value;
}

void parseVersionMessage(unsigned char *payload, size_t payload_len, struct version_message *out_msg)
{
    size_t offset = 0;
    out_msg->version = ntohl(*((uint32_t *)(payload + offset)));
    offset += 4;
    out_msg->services = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    out_msg->timestamp = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    memcpy(out_msg->addr_recv, payload + offset, 26);
    offset += 26;
    memcpy(out_msg->addr_from, payload + offset, 26);
    offset += 26;
    out_msg->nonce = be64toh(*((uint64_t *)(payload + offset)));
    offset += 8;
    // user_agentのデコード
    size_t outlen = 0;
    uint64_t ua_len = decodeVarint(payload + offset, &outlen);
    offset += outlen;
    out_msg->user_agent = malloc(ua_len + 1);
    if (out_msg->user_agent == NULL)
    {
        perror("Memory allocation failed for user_agent");
        exit(EXIT_FAILURE);
    }
    memcpy(out_msg->user_agent, payload + offset, ua_len);
    out_msg->user_agent[ua_len] = '\0';
    offset += ua_len;
    // stream_numbersのデコード
    uint64_t stream_count = decodeVarint(payload + offset, &outlen);
    offset += outlen;
    out_msg->stream_numbers_len = stream_count;
    out_msg->stream_numbers = malloc(sizeof(uint64_t) * stream_count);
    if (out_msg->stream_numbers == NULL)
    {
        perror("Memory allocation failed for stream_numbers");
        free(out_msg->user_agent);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < stream_count; i++)
    {
        out_msg->stream_numbers[i] = decodeVarint(payload + offset, &outlen);
        offset += outlen;
    }
}

static size_t get_varint_list_size(uint64_t *list, size_t listlen)
{
    size_t list_header_size = get_varint_size(listlen);
    size_t sum_of_elements_size = 0;
    for (size_t i = 0; i < listlen; i++)
    {
        sum_of_elements_size += get_varint_size(list[i]);
    }
    return list_header_size + sum_of_elements_size;
}

static uint8_t *endodeVariableLengthListOfIntegers(uint8_t *out, uint64_t *list, size_t listlen)
{
    if (out == NULL)
    {
        return NULL;
    }
    encodeVarint(out, listlen);
    size_t offset = get_varint_size(listlen);
    for (size_t i = 0; i < listlen; i++)
    {
        uint64_t e = list[i];
        encodeVarint(out + offset, e);
        offset += get_varint_size(e);
    }
    return out;
}

size_t get_version_message_size(const char *user_agent_str)
{
    size_t payload_len = get_version_payload_size(user_agent_str);
    return 24 + payload_len;
}

size_t get_version_payload_size(const char *user_agent_str)
{
    size_t ua_len = get_varstr_size(user_agent_str);
    return 82 + ua_len;
}

unsigned char *create_version_payload(uint8_t *out, const char *user_agent_str, int version, struct sockaddr_storage *peer_addr, struct sockaddr_storage *local_addr)
{
    if (out == NULL)
    {
        return NULL;
    }
    // user_agent
    size_t ua_len = get_varstr_size(user_agent_str);
    size_t offset = 0;
    uint32_t net_version = htobe32((uint32_t)version);
    memcpy(out + offset, &net_version, 4);
    offset += 4;
    uint64_t services = 0;
    uint64_t net_services = htobe64(services);
    memcpy(out + offset, &net_services, 8);
    offset += 8;
    uint64_t timestamp = (uint64_t)time(NULL);
    uint64_t net_timestamp = htobe64(timestamp);
    memcpy(out + offset, &net_timestamp, 8);
    offset += 8;
    // addr_recv
    encodeNetworkAddress(out + offset, peer_addr);
    offset += 26;
    // addr_from
    encodeNetworkAddress(out + offset, local_addr);
    offset += 26;
    uint64_t nonce = 0;
    getrandom(&nonce, sizeof(nonce), GRND_NONBLOCK);
    uint64_t net_nonce = htobe64(nonce);
    memcpy(out + offset, &net_nonce, 8);
    offset += 8;
    // user_agent
    encodeVarStr(out + offset, user_agent_str);
    offset += ua_len;
    // stream_numbers
    size_t stream_list_len = 1;
    size_t stream_list_encoded_len = 2;
    unsigned char stream_list_encoded[] = {1, 1};
    memcpy(out + offset, stream_list_encoded, stream_list_encoded_len);
    offset += stream_list_encoded_len;
    return out;
}

uint8_t *new_version_message(const char *user_agent_str, int version, struct sockaddr_storage *peer_addr, struct sockaddr_storage *local_addr)
{
    size_t versionmsglen = get_version_payload_size(user_agent_str);
    uint8_t *version_message = malloc(24 + versionmsglen);
    create_version_payload(version_message + 24, user_agent_str, version, peer_addr, local_addr);
    // ヘッダの作成
    memcpy(version_message, magicbytes, 4);
    strncpy(version_message + 4, "version", 12);
    uint32_t net_payload_length = htobe32((uint32_t)versionmsglen);
    memcpy(version_message + 16, &net_payload_length, 4);
    // checksumの計算
    unsigned char checksum_full[64];
    SHA512(version_message + 24, versionmsglen, checksum_full);
    memcpy(version_message + 20, checksum_full, 4);
    return version_message;
}

void freeVersionMessage(struct version_message *msg)
{
    if (msg->user_agent != NULL)
    {
        free(msg->user_agent);
        msg->user_agent = NULL;
    }
    if (msg->stream_numbers != NULL)
    {
        free(msg->stream_numbers);
        msg->stream_numbers = NULL;
    }
}

struct address_info
{
    uint64_t time;
    uint32_t stream;
    uint64_t services;
    uint8_t ip[16];
    uint16_t port;
};

void parseAddrMessage(unsigned char *payload, size_t payload_len, struct addr_message *out_msg)
{
    size_t offset = 0;
    size_t outlen = 0;
    out_msg->count = decodeVarint(payload + offset, &outlen);
    offset += outlen;
    out_msg->addresses = malloc(sizeof(struct address_info) * out_msg->count);
    if (out_msg->addresses == NULL)
    {
        perror("Memory allocation failed for addresses");
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < out_msg->count; i++)
    {
        out_msg->addresses[i].time = be64toh(*((uint64_t *)(payload + offset)));
        offset += 8;
        out_msg->addresses[i].stream = ntohl(*((uint32_t *)(payload + offset)));
        offset += 4;
        out_msg->addresses[i].services = be64toh(*((uint64_t *)(payload + offset)));
        offset += 8;
        memcpy(out_msg->addresses[i].ip, payload + offset, 16);
        offset += 16;
        out_msg->addresses[i].port = ntohs(*((uint16_t *)(payload + offset)));
        offset += 2;
    }
}

void freeAddrMessage(struct addr_message *msg)
{
    if (msg->addresses != NULL)
    {
        free(msg->addresses);
        msg->addresses = NULL;
    }
}
int is_valid_bm_addr(const uint8_t *p)
{
    // 1. IPv4射影アドレスの場合 (::ffff:x.x.x.x)
    // 最初の10バイトが0、次の2バイトが0xff
    int all_zero_prefix = p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 0 &&
                          p[4] == 0 && p[5] == 0 && p[6] == 0 && p[7] == 0 &&
                          p[8] == 0 && p[9] == 0;
    if (all_zero_prefix)
    {
        // 次の2バイトが ff ff なら正常なIPv4
        if (p[10] == 0xff && p[11] == 0xff)
            return 1;
        // それ以外の 0000...dfff... などは破損
        return 0;
    }

    // 2. 本物のIPv6アドレスの場合
    // 先頭が 00... は通常ありえない。グローバル(2000::/3)かULA(fd00::/8)が一般的
    if (p[0] == 0x00)
    {
        return 0;
    }
    if ((p[0] & 0xe0) != 0x20)
    {
        return 0;
    }

    // それ以外はある程度信頼して表示
    return 1;
}

void printAddrMessage(struct addr_message *addr_msg)
{
    fprintf(stderr, "Number of addresses: %" PRIu64 "\n", addr_msg->count);
    for (uint64_t i = 0; i < addr_msg->count; i++)
    {
        if (is_valid_bm_addr(addr_msg->addresses[i].ip))
        {
            struct tm tm_info;
            char time_buffer[128];
            localtime_r((time_t *)&addr_msg->addresses[i].time, &tm_info);
            strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
            fprintf(stderr, "  Address %" PRIu64 ": time=%" PRIu64 "(%s), stream=%" PRIu32 ", services=%016" PRIx64 ", port=%u,", i,
                    addr_msg->addresses[i].time, time_buffer, addr_msg->addresses[i].stream, addr_msg->addresses[i].services, addr_msg->addresses[i].port);
            // IPアドレスの表示
            unsigned char *ip = addr_msg->addresses[i].ip;
            if (ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0 &&
                ip[4] == 0 && ip[5] == 0 && ip[6] == 0 && ip[7] == 0 &&
                ip[8] == 0 && ip[9] == 0 && ip[10] == 0xff && ip[11] == 0xff)
            {
                // IPv4-mapped IPv6 address
                fprintf(stderr, " IPv4 Address: %u.%u.%u.%u\n", ip[12], ip[13], ip[14], ip[15]);
            }
            else
            {
                // IPv6 address
                char ipv6_str[40];
                snprintf(ipv6_str, sizeof(ipv6_str),
                         "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
                         "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                         ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7],
                         ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15]);
                fprintf(stderr, " IPv6 Address: %s\n", ipv6_str);
            }
        }
        else
        {
            fprintf(stderr, "  Address %" PRIu64 ": ignored\n", i);
        }
    }
}

void parseInventoryMessage(unsigned char *payload, size_t payload_len, struct inventory_message *out_msg)
{
    size_t offset = 0;
    size_t outlen = 0;
    out_msg->count = decodeVarint(payload + offset, &outlen);
    offset += outlen;
    uint64_t actual_count = (payload_len - offset) / 32;
    /* fprintf(stderr, "Declared inv count: %" PRIu64 ", Actual inv count in payload: %" PRIu64 "\n", out_msg->count, actual_count);
    if (out_msg->count != actual_count)
    {
        fprintf(stderr, "Warning: inv count mismatch! Declared: %" PRIu64 ", Actual: %" PRIu64 "\n", out_msg->count, actual_count);
    } */
    out_msg->count = actual_count;
    out_msg->items = malloc(sizeof(struct inventory_item) * actual_count);
    if (out_msg->items == NULL)
    {
        perror("Memory allocation failed for inventory items");
        exit(EXIT_FAILURE);
    }
    // 実際のペイロードに含まれるアイテム数を計算
    for (size_t i = 0; i < actual_count; i++)
    {
        memcpy(out_msg->items[i].object_hash, payload + offset, 32);
        offset += 32;
    }
}

void freeInventoryMessage(struct inventory_message *msg)
{
    if (msg->items != NULL)
    {
        free(msg->items);
        msg->items = NULL;
    }
}

void process_command(struct fd_data *data, struct message *msg)
{
    // コマンドに対する処理を Strategy パターンを模倣して実装
    if (strncmp(msg->command, "verack", 12) == 0)
    {
        fprintf(stderr, "Received verack message\n");
        // NOP
    }
    else if (strncmp(msg->command, "version", 12) == 0)
    {
        fprintf(stderr, "Received version message\n");
        struct version_message ver_msg;
        parseVersionMessage(msg->payload, msg->length, &ver_msg);
        fprintf(stderr, "Version: %u, Services: %" PRIu64 ", Timestamp: %" PRIu64 "\n", ver_msg.version, ver_msg.services, ver_msg.timestamp);
        printNetworkAddress(ver_msg.addr_recv, 26); // addr_recv
        printNetworkAddress(ver_msg.addr_from, 26); // addr_from
        fprintf(stderr, "Nonce: %016" PRIx64 "\n", ver_msg.nonce);
        fprintf(stderr, "User Agent: %s\n", ver_msg.user_agent);
        fprintf(stderr, "Stream Count: %" PRIu64 "\n", ver_msg.stream_numbers_len);
        fprintf(stderr, "Streams: ");
        for (uint64_t i = 0; i < ver_msg.stream_numbers_len; i++)
        {
            fprintf(stderr, "%" PRIu64 " ", ver_msg.stream_numbers[i]);
        }
        fprintf(stderr, "\n");
        freeVersionMessage(&ver_msg);
        if (replyVarack(data) != EXIT_SUCCESS)
        {
            //?
        }
    }
    else if (strncmp(msg->command, "addr", 12) == 0)
    {
        fprintf(stderr, "Received addr message\n");
        struct addr_message addr_msg;
        parseAddrMessage(msg->payload, msg->length, &addr_msg);
        printAddrMessage(&addr_msg);
        // 本当は適宜ストレージに保存するんやろな
        // struct addr_message を bm_node_t に詰め替えて送信
        // メモリ解放
        freeAddrMessage(&addr_msg);
    }
    else if (strncmp(msg->command, "inv", 12) == 0)
    {
        fprintf(stderr, "Received inv message\n");
        struct inventory_message inv_msg;
        parseInventoryMessage(msg->payload, msg->length, &inv_msg);
        //fprintf(stderr, "Number of inventory items: %" PRIu64 "\n", inv_msg.count);
        /* for (uint64_t i = 0; i < inv_msg.count; i++)
        {
            fprintf(stderr, "  Item %" PRIu64 ": hash=", i);
            for (int j = 0; j < 32; j++)
            {
                fprintf(stderr, "%02x", inv_msg.items[i].object_hash[j]);
            }
            fprintf(stderr, "\n");
        } */
        freeInventoryMessage(&inv_msg);
        // getdata送信スレッドにinvベクタを転送する
    }
    else if (strncmp(msg->command, "ping", 12) == 0)
    {
        time_t now = time(NULL);
        struct tm tm;
        localtime_r(&now, &tm);
        fprintf(stderr, "Received ping message(%4d/%02d/%02d %02d:%02d:%02d)\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        if (replyPong(data) != EXIT_SUCCESS)
        {
            perror("write pong error");
            close(data->fd);
            free_fd_data(data);
        }
    }
    else if (strncmp(msg->command, "getdata", 12) == 0)
    {
        fprintf(stderr, "Received getdata message\n");
        // DO ANYTHING
    }
    else if (strncmp(msg->command, "object", 12) == 0)
    {
        fprintf(stderr, "Received object message\n");
        // object payload保存スレッドに転送する
    }
    else
    {
        char command[13] = {0};
        strncpy(command, msg->command, 12);
        fprintf(stderr, "Unknown command: %s\n", command);
    }
}
