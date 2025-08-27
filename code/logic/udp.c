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
#include "fossil/network/udp.h"

#include <string.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <sys/socket.h>
#endif

int fossil_network_udp_bind(fossil_network_socket_t *sock,
                            const char *host, uint16_t port) {
    if (!sock) return -1;

    struct addrinfo hints, *res = NULL;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    if (getaddrinfo(host, port_str, &hints, &res) != 0) return -1;

    sock->fd = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock->fd < 0) {
        freeaddrinfo(res);
        return -1;
    }

    if (bind(sock->fd, res->ai_addr, (int)res->ai_addrlen) != 0) {
        fossil_network_socket_close(sock);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    return 0;
}

int fossil_network_udp_set_broadcast(fossil_network_socket_t *sock, int enable) {
    if (!sock) return -1;
    int opt = enable ? 1 : 0;
    return setsockopt(sock->fd, SOL_SOCKET, SO_BROADCAST,
                      (char*)&opt, sizeof(opt));
}
