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
#include "fossil/network/protocol.h"


// ------------------------------
// Internal helpers
// ------------------------------
static int fossil_network_proto_send_all(int fd, const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t*)buf;
    size_t total = 0;
    while (total < len) {
        ssize_t n = send(fd, p + total, len - total, 0);
        if (n <= 0) return -1; // error or connection closed
        total += (size_t)n;
    }
    return 0;
}

static int fossil_network_proto_recv_all(int fd, void *buf, size_t len) {
    uint8_t *p = (uint8_t*)buf;
    size_t total = 0;
    while (total < len) {
        ssize_t n = recv(fd, p + total, len - total, 0);
        if (n <= 0) return -1; // error or connection closed
        total += (size_t)n;
    }
    return 0;
}

// ------------------------------
// Public API
// ------------------------------

/**
 * @brief Send a length-prefixed message.
 *
 * Format:
 *   [4-byte big-endian length][payload bytes...]
 */
int fossil_network_proto_send_length(fossil_network_socket_t *sock,
                                     const void *buf, size_t len) {
    if (!sock || sock->fd < 0 || !buf) return -1;

    // encode length prefix
    uint32_t net_len = htonl((uint32_t)len);

    // send length
    if (fossil_network_proto_send_all(sock->fd, &net_len, sizeof(net_len)) != 0)
        return -1;

    // send payload
    if (fossil_network_proto_send_all(sock->fd, buf, len) != 0)
        return -1;

    return 0;
}

/**
 * @brief Receive a length-prefixed message.
 *
 * The buffer must be large enough to hold the entire payload.
 */
int fossil_network_proto_recv_length(fossil_network_socket_t *sock,
                                     void *buf, size_t max_len) {
    if (!sock || sock->fd < 0 || !buf) return -1;

    uint32_t net_len = 0;
    if (fossil_network_proto_recv_all(sock->fd, &net_len, sizeof(net_len)) != 0)
        return -1;

    uint32_t msg_len = ntohl(net_len);
    if (msg_len > max_len) {
        // message too large for buffer
        return -1;
    }

    if (fossil_network_proto_recv_all(sock->fd, buf, msg_len) != 0)
        return -1;

    return (int)msg_len; // return number of bytes received
}
