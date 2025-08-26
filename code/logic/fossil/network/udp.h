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
#ifndef FOSSIL_NETWORK_UDP_H
#define FOSSIL_NETWORK_UDP_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Bind a UDP socket to a local address and port.
 *
 * @param sock Pointer to initialized fossil_network_socket_t.
 * @param host Local address (NULL = any).
 * @param port Local UDP port.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_udp_bind(fossil_network_socket_t *sock,
                            const char *host, uint16_t port);

/**
 * @brief Enable broadcast mode for a UDP socket.
 *
 * @param sock Pointer to fossil_network_socket_t.
 * @param enable 1 = enable, 0 = disable.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_udp_set_broadcast(fossil_network_socket_t *sock,
                                     int enable);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_UDP_H */
