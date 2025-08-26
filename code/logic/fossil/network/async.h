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
#ifndef FOSSIL_NETWORK_ASYNC_H
#define FOSSIL_NETWORK_ASYNC_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fossil_network_async fossil_network_async_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Create a new async event loop.
 */
fossil_network_async_t* fossil_network_async_create(void);

/**
 * @brief Destroy an async event loop.
 */
void fossil_network_async_free(fossil_network_async_t *loop);

/**
 * @brief Register a socket with the loop.
 *
 * @param loop Event loop handle.
 * @param sock Socket to watch.
 * @param events Bitmask (1=read, 2=write).
 * @param userdata Arbitrary pointer passed to callback.
 */
int fossil_network_async_add(fossil_network_async_t *loop,
                             fossil_network_socket_t *sock,
                             int events,
                             void *userdata);

/**
 * @brief Run the event loop.
 *
 * @param loop Event loop handle.
 * @param timeout_ms Timeout per iteration.
 */
int fossil_network_async_run(fossil_network_async_t *loop,
                             int timeout_ms);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_ASYNC_H */
