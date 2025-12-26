// ネットワーク関連の機能を実装するファイル
// ネットワーク多重化+サーバー対応など将来的な拡張を見据えた設計とする
#include <bm_network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <time.h>
#include <sys/random.h>
#include <bm_protocol.h> // struct messageの定義のため
#include <endian.h>      // htobe64, htobe32などのため

int epfd = -1;

// 128kb
#define INIT_CONNECT_BUFFER_SIZE 131072

struct fd_data *new_fd_data(enum fd_type type, int fd)
{
    struct fd_data *data = malloc(sizeof(struct fd_data));
    if (data == NULL)
    {
        return NULL;
    }
    data->type = CLIENT_SOCKET;
    data->fd = fd;
    data->size = INIT_CONNECT_BUFFER_SIZE;
    data->length = 0;
    data->connectedBuffer = malloc(INIT_CONNECT_BUFFER_SIZE);
    if (data->connectedBuffer == NULL)
    {
        free(data);
        return NULL;
    }
    return data;
}

void free_fd_data(struct fd_data *data)
{
    if (data == NULL)
    {
        return;
    }
    if (data->connectedBuffer == NULL)
    {
        free(data->connectedBuffer);
    }
    free(data);
}

// verackコマンドを送信する
int replyVarack(struct fd_data *data)
{
    unsigned char verack_header[24];
    memcpy(verack_header, magicbytes, 4);
    char verack_command[12] = {0};
    strncpy(verack_command, "verack", 12);
    memcpy(verack_header + 4, verack_command, 12);
    uint32_t verack_payload_length = 0;
    uint32_t net_verack_payload_length = htobe32(verack_payload_length);
    memcpy(verack_header + 16, &net_verack_payload_length, 4);
    memcpy(verack_header + 20, empty_payload_checksum, 4);
    ssize_t w = write(data->fd, verack_header, 24);
    if (w != 24)
    {
        perror("write verack error");
        close(data->fd);
        free_fd_data(data);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

// pongメッセージを返信する
int replyPong(struct fd_data *data)
{
    unsigned char pong_header[24];
    memcpy(pong_header, magicbytes, 4);
    char pong_command[12] = {0};
    strncpy(pong_command, "pong", 12);
    memcpy(pong_header + 4, pong_command, 12);
    uint32_t pong_payload_length = 0;
    uint32_t net_pong_payload_length = htobe32(pong_payload_length);
    memcpy(pong_header + 16, &net_pong_payload_length, 4);
    memcpy(pong_header + 20, empty_payload_checksum, 4);
    ssize_t w = write(data->fd, pong_header, 24);
    if (w != 24)
    {
        perror("write pong error");
        close(data->fd);
        free_fd_data(data);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
