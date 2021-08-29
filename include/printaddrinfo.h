
#ifndef PRINTADDRINFO_H
#define PRINTADDRINFO_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
void printaddrinfo0(struct addrinfo *, FILE *);
void printaddrinfo(struct addrinfo *);

#endif
