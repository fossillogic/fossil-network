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
#ifndef FOSSIL_NETWORK_SERVER_H
#define FOSSIL_NETWORK_SERVER_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// Forward declaration
typedef struct fossil_network_server fossil_network_server_t;

/**
 * Create a server instance
 *
 * @param host   Hostname or IP to bind to (NULL = any)
 * @param service Port or service name (e.g. "8080", "smtp")
 * @param proto  Protocol type (TCP, UDP, etc.)
 * @return pointer to server object or NULL on failure
 */
fossil_network_server_t *
fossil_network_server_create(const char *host,
                             const char *service,
                             fossil_protocol_t proto);

/**
 * Start listening for connections
 *
 * @param srv     Server object
 * @param backlog Maximum number of pending connections
 * @return 0 on success, -1 on error
 */
int fossil_network_server_listen(fossil_network_server_t *srv, int backlog);

/**
 * Accept a client connection
 *
 * @param srv Server object
 * @return pointer to socket object for client or NULL on error
 */
fossil_network_socket_t *
fossil_network_server_accept(fossil_network_server_t *srv);

/**
 * Send data to a client
 */
ssize_t fossil_network_server_send(fossil_network_socket_t *client,
                                   const void *buf,
                                   size_t len,
                                   int flags);

/**
 * Receive data from a client
 */
ssize_t fossil_network_server_recv(fossil_network_socket_t *client,
                                   void *buf,
                                   size_t len,
                                   int flags);

/**
 * Free server resources
 */
void fossil_network_server_destroy(fossil_network_server_t *srv);

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

#endif /* FOSSIL_NETWORK_SERVER_H */
