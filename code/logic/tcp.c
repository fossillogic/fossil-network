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
#include <stdlib.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <sys/socket.h>
#endif

int fossil_network_tcp_connect(fossil_network_socket_t *sock,
                               const char *host, uint16_t port) {
    if (!sock || !host) return -1;

    struct addrinfo hints, *res = NULL;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port_str, &hints, &res) != 0) return -1;

    sock->fd = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock->fd < 0) {
        freeaddrinfo(res);
        return -1;
    }

    if (connect(sock->fd, res->ai_addr, (int)res->ai_addrlen) != 0) {
        fossil_network_socket_close(sock);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return 0;
}

int fossil_network_tcp_listen(fossil_network_socket_t *sock,
                              const char *host, uint16_t port,
                              int backlog) {
    if (!sock) return -1;

    struct addrinfo hints, *res = NULL;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if (getaddrinfo(host, port_str, &hints, &res) != 0) return -1;

    sock->fd = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock->fd < 0) {
        freeaddrinfo(res);
        return -1;
    }

    int opt = 1;
    setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    if (bind(sock->fd, res->ai_addr, (int)res->ai_addrlen) != 0) {
        fossil_network_socket_close(sock);
        freeaddrinfo(res);
        return -1;
    }

    if (listen(sock->fd, backlog) != 0) {
        fossil_network_socket_close(sock);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return 0;
}

int fossil_network_tcp_accept(fossil_network_socket_t *server,
                              fossil_network_socket_t *client) {
    if (!server || !client) return -1;

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    int fd = (int)accept(server->fd, (struct sockaddr *)&addr, &addr_len);
    if (fd < 0) return -1;

    client->fd = fd;
    return 0;
}
