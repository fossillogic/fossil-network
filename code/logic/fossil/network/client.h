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
#ifndef FOSSIL_NETWORK_CLIENT_H
#define FOSSIL_NETWORK_CLIENT_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// Forward declaration
typedef struct fossil_network_client fossil_network_client_t;

/**
 * Create a client instance
 *
 * @param host   Hostname or IP address
 * @param service Port or service name (e.g. "80", "http")
 * @param proto  Protocol type (TCP, UDP, etc.)
 * @return pointer to client object or NULL on failure
 */
fossil_network_client_t *
fossil_network_client_create(const char *host,
                             const char *service,
                             fossil_protocol_t proto);

/**
 * Establish a connection
 *
 * @param cli Client object
 * @return 0 on success, -1 on error
 */
int fossil_network_client_connect(fossil_network_client_t *cli);

/**
 * Send data
 *
 * @param cli  Client object
 * @param buf  Data buffer
 * @param len  Buffer length
 * @param flags Socket send flags
 * @return number of bytes sent or -1 on error
 */
ssize_t fossil_network_client_send(fossil_network_client_t *cli,
                                   const void *buf,
                                   size_t len,
                                   int flags);

/**
 * Receive data
 *
 * @param cli  Client object
 * @param buf  Buffer to store received data
 * @param len  Buffer length
 * @param flags Socket recv flags
 * @return number of bytes received or -1 on error
 */
ssize_t fossil_network_client_recv(fossil_network_client_t *cli,
                                   void *buf,
                                   size_t len,
                                   int flags);

/**
 * Free client resources
 */
void fossil_network_client_destroy(fossil_network_client_t *cli);

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

#endif /* FOSSIL_NETWORK_CLIENT_H */
