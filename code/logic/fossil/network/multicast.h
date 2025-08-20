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
#ifndef FOSSIL_NETWORK_MULTICAST_H
#define FOSSIL_NETWORK_MULTICAST_H

#include "network.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// ------------------------------
// Multicast socket structure
// ------------------------------
typedef struct fossil_network_multicast_t {
    fossil_network_socket_t sock;
} fossil_network_multicast_t;

// ------------------------------
// API
// ------------------------------
fossil_network_multicast_t * fossil_network_multicast_create(const char *group, uint16_t port);

ssize_t fossil_network_multicast_send(fossil_network_multicast_t *mc,
                              const char *msg,
                              size_t len,
                              const char *group,
                              uint16_t port);

ssize_t fossil_network_multicast_recv(fossil_network_multicast_t *mc,
                              char *buffer,
                              size_t buf_size);

int fossil_network_multicast_destroy(fossil_network_multicast_t *mc);

#ifdef __cplusplus
}
#include <stdexcept>
#include <vector>
#include <string>

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_MULTICAST_H */
