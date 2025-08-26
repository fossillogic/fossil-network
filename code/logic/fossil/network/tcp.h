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
#ifndef FOSSIL_NETWORK_TCP_H
#define FOSSIL_NETWORK_TCP_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Connect to a remote host using TCP.
 *
 * @param sock Pointer to initialized fossil_network_socket_t.
 * @param host Remote hostname or IP address.
 * @param port Remote TCP port.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_connect(fossil_network_socket_t *sock,
                               const char *host, uint16_t port);

/**
 * @brief Start a TCP server socket.
 *
 * @param sock Pointer to initialized fossil_network_socket_t.
 * @param host Local address to bind (NULL = any).
 * @param port Local port to bind.
 * @param backlog Max pending connections.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_listen(fossil_network_socket_t *sock,
                              const char *host, uint16_t port,
                              int backlog);

/**
 * @brief Accept an incoming TCP connection.
 *
 * @param server Listening socket.
 * @param client Output socket for accepted connection.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_accept(fossil_network_socket_t *server,
                              fossil_network_socket_t *client);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_TCP_H */
