/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/network/tcp.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <errno.h>
#endif


static int fossil__set_socket(fossil_network_socket_t *sock, int fd,
                              int family, int type, int proto) {
    if (!sock) return -1;
    sock->fd = fd;
    sock->family = family;
    sock->type = type;
    sock->protocol = proto;
    return 0;
}

int fossil_network_tcp_connect(fossil_network_socket_t *sock,
                               const char *host, uint16_t port) {
    if (!sock || !host) return -1;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    struct addrinfo hints, *res = NULL, *rp = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;   /* IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) return -1;

    int fd = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        fd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        if (connect(fd, rp->ai_addr, (socklen_t)rp->ai_addrlen) == 0) {
            /* Success */
            fossil__set_socket(sock, fd, rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            freeaddrinfo(res);
            return 0;
        }

#ifdef _WIN32
        closesocket(fd);
#else
        close(fd);
#endif
        fd = -1;
    }

    freeaddrinfo(res);
    return -1;
}

int fossil_network_tcp_listen(fossil_network_socket_t *sock,
                              const char *host, uint16_t port,
                              int backlog) {
    if (!sock) return -1;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    struct addrinfo hints, *res = NULL, *rp = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;  /* For binding */

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) return -1;

    int fd = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        fd = (int)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) continue;

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&opt, sizeof(opt));

        if (bind(fd, rp->ai_addr, (socklen_t)rp->ai_addrlen) == 0 &&
            listen(fd, backlog) == 0) {
            /* Success */
            fossil__set_socket(sock, fd, rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            freeaddrinfo(res);
            return 0;
        }

#ifdef _WIN32
        closesocket(fd);
#else
        close(fd);
#endif
        fd = -1;
    }

    freeaddrinfo(res);
    return -1;
}

int fossil_network_tcp_accept(fossil_network_socket_t *server,
                              fossil_network_socket_t *client) {
    if (!server || !client) return -1;

    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    int fd = (int)accept(server->fd, (struct sockaddr *)&addr, &addrlen);
    if (fd < 0) return -1;

    fossil__set_socket(client, fd, server->family, server->type, server->protocol);
    return 0;
}
