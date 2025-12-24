// プロトコルメッセージの構造化とパースの厳密化
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
