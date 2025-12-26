
#ifndef BM_NETWORK_H
#define BM_NETWORK_H
#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>

extern int epfd;

enum fd_type
{
    CLIENT_SOCKET,
#define CLIENT_SOCKET CLIENT_SOCKET
    SERVER_SOCKET,
#define SERVER_SOCKET SERVER_SOCKET
    TIMERFD
#define TIMERFD TIMERFD
};

struct fd_data
{
    enum fd_type type;
    int fd;
    size_t size;
    size_t length;
    uint8_t *connectedBuffer;
};
extern struct fd_data *new_fd_data(enum fd_type type, int fd);
extern void free_fd_data(struct fd_data *data);
extern int replyVarack(struct fd_data *data);
extern int replyPong(struct fd_data *data);
#endif // BM_NETWORK_H
