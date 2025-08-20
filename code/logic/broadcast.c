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
#include "fossil/network/broadcast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------------------
// Create broadcast socket
// ------------------------------
fossil_network_broadcast_t *fossil_network_broadcast_create(uint16_t port) {
    fossil_network_broadcast_t *bc = calloc(1, sizeof(*bc));
    if (!bc) return NULL;

    if (fossil_network_socket_create(&bc->sock, AF_INET, FOSSIL_PROTO_UDP) != 0) {
        free(bc);
        return NULL;
    }

    if (fossil_network_socket_bind(&bc->sock, "0.0.0.0", port) != 0) {
        fossil_network_socket_close(&bc->sock);
        free(bc);
        return NULL;
    }

    return bc;
}

// ------------------------------
// Send broadcast message
// ------------------------------
ssize_t fossil_network_broadcast_send(fossil_network_broadcast_t *bc,
                                      const char *msg,
                                      size_t len,
                                      uint16_t port) {
    if (!bc) return -1;

    // Broadcast address
    return fossil_network_socket_sendto(&bc->sock,
                                        msg,
                                        len,
                                        "255.255.255.255",
                                        port);
}

// ------------------------------
// Receive broadcast message
// ------------------------------
ssize_t fossil_network_broadcast_recv(fossil_network_broadcast_t *bc,
                                      char *buffer,
                                      size_t buf_size) {
    if (!bc) return -1;
    return fossil_network_socket_recv(&bc->sock, buffer, buf_size);
}

// ------------------------------
// Destroy broadcast socket
// ------------------------------
int fossil_network_broadcast_destroy(fossil_network_broadcast_t *bc) {
    if (!bc) return -1;
    fossil_network_socket_close(&bc->sock);
    free(bc);
    return 0;
}
