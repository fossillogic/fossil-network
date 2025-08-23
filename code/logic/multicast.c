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
#include "fossil/network/multicast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <iphlpapi.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

// ------------------------------
// Create multicast socket
// ------------------------------
fossil_network_multicast_t *fossil_network_multicast_create(const char *group,
                                                            uint16_t port) {
    fossil_network_multicast_t *mc = calloc(1, sizeof(*mc));
    if (!mc) return NULL;

    if (fossil_network_socket_create(&mc->sock, AF_INET, FOSSIL_PROTO_UDP) != 0) {
        free(mc);
        return NULL;
    }

    if (fossil_network_socket_bind(&mc->sock, "0.0.0.0", port) != 0) {
        fossil_network_socket_close(&mc->sock);
        free(mc);
        return NULL;
    }

    // TODO: join multicast group (cross-platform)
    // On POSIX: use setsockopt(IPPROTO_IP, IP_ADD_MEMBERSHIP)
    // On Windows: same via Winsock

    (void)group; // placeholder

    return mc;
}

// ------------------------------
// Send multicast message
// ------------------------------
static int fossil_network_multicast_ensure_connected(
    fossil_network_multicast_t* mc,
    const char* group,
    uint16_t port) {
    if (!mc || !group) return -1;

    if (mc->is_connected &&
        mc->connected_port == port &&
        mc->connected_group[0] != '\0' &&
        strcmp(mc->connected_group, group) == 0) {
        return 0; // already connected to this group:port
    }

    // (Re)connect the UDP socket to set the default destination.
    if (fossil_network_socket_connect(&mc->sock, group, port) != 0)
        return -1;

    // Cache destination
    strncpy(mc->connected_group, group, sizeof(mc->connected_group) - 1);
    mc->connected_group[sizeof(mc->connected_group) - 1] = '\0';
    mc->connected_port = port;
    mc->is_connected = true;
    return 0;
}

ssize_t fossil_network_multicast_send(
    fossil_network_multicast_t *mc,
    const char *msg,
    size_t len,
    const char *group,
    uint16_t port) {
    if (!mc || !msg) return -1;

    if (fossil_network_multicast_ensure_connected(mc, group, port) != 0)
        return -1;

    return fossil_network_socket_send(&mc->sock, msg, len);
}

// ------------------------------
// Receive multicast message
// ------------------------------
ssize_t fossil_network_multicast_recv(fossil_network_multicast_t *mc,
                                      char *buffer,
                                      size_t buf_size) {
    if (!mc) return -1;
    return fossil_network_socket_recv(&mc->sock, buffer, buf_size);
}

// ------------------------------
// Destroy multicast socket
// ------------------------------
int fossil_network_multicast_destroy(fossil_network_multicast_t *mc) {
    if (!mc) return -1;
    fossil_network_socket_close(&mc->sock);
    free(mc);
    return 0;
}
