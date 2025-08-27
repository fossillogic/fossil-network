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

// Helper to set socket metadata
static int fossil__set_socket(fossil_network_socket_t *sock,
                              fossil_socket_fd_t fd,
                              int family, int type,
                              fossil_protocol_t proto) {
    if (!sock) return -1;
    sock->fd     = fd;
    sock->family = family;
    sock->type   = type;
    sock->proto  = proto;
    return 0;
}

// ------------------------------
// UDP Bind
// ------------------------------
int fossil_network_udp_bind(fossil_network_socket_t *sock,
                            const char *host, uint16_t port) {
    if (!sock) return -1;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    struct addrinfo hints, *res = NULL, *rp = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;    // For binding

    int rc = getaddrinfo(host, port_str, &hints, &res);
    if (rc != 0) return -1;

    fossil_socket_fd_t fd = (fossil_socket_fd_t)-1;

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        fd = (fossil_socket_fd_t)socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd == (fossil_socket_fd_t)-1) continue;

        int opt = 1;
        setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                   (const char *)&opt, sizeof(opt));

        if (bind(fd, rp->ai_addr, (socklen_t)rp->ai_addrlen) == 0) {
            // Success
            fossil__set_socket(sock, fd, rp->ai_family, rp->ai_socktype,
                               (fossil_protocol_t)rp->ai_protocol);
            freeaddrinfo(res);
            return 0;
        }

#ifdef _WIN32
        closesocket(fd);
#else
        close(fd);
#endif
        fd = (fossil_socket_fd_t)-1;
    }

    freeaddrinfo(res);
    return -1;
}

// ------------------------------
// UDP Broadcast Mode
// ------------------------------
int fossil_network_udp_set_broadcast(fossil_network_socket_t *sock,
                                     int enable) {
    if (!sock || sock->fd == (fossil_socket_fd_t)-1) return -1;

    int opt = (enable ? 1 : 0);
    int rc = setsockopt(sock->fd, SOL_SOCKET, SO_BROADCAST,
                        (const char *)&opt, sizeof(opt));
    return (rc == 0) ? 0 : -1;
}
