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
#ifndef FOSSIL_NETWORK_PROTOCOL_H
#define FOSSIL_NETWORK_PROTOCOL_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Send a length-prefixed message.
 */
int fossil_network_proto_send_length(fossil_network_socket_t *sock,
                                     const void *buf, size_t len);

/**
 * @brief Receive a length-prefixed message.
 */
int fossil_network_proto_recv_length(fossil_network_socket_t *sock,
                                     void *buf, size_t max_len);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
