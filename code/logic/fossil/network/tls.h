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
#ifndef FOSSIL_NETWORK_TLS_H
#define FOSSIL_NETWORK_TLS_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// ------------------------------
// Opaque TSL context
// ------------------------------

/**
 * @brief Opaque structure representing a TSL (Transport Security Layer) context.
 *
 * This structure holds the state for a secured TSL session.
 */
typedef struct fossil_tsl_context fossil_tsl_context_t;

// ------------------------------
// Lifecycle
// ------------------------------

/**
 * @brief Create a new TSL context for the given network socket.
 *
 * @param sock Pointer to an existing fossil_network_socket_t.
 * @return Pointer to a newly allocated fossil_tsl_context_t, or NULL on failure.
 */
fossil_tsl_context_t *fossil_tsl_create(fossil_network_socket_t *sock);

/**
 * @brief Free the resources associated with a TSL context.
 *
 * @param ctx Pointer to the fossil_tsl_context_t to be freed.
 */
void fossil_tsl_free(fossil_tsl_context_t *ctx);

// ------------------------------
// Handshake
// ------------------------------

/**
 * @brief Perform a simplified TSL handshake (no real crypto, stubbed).
 *
 * @param ctx Pointer to the fossil_tsl_context_t.
 * @return 0 on success, negative value on error.
 */
int fossil_tsl_handshake(fossil_tsl_context_t *ctx);

// ------------------------------
// Data transmission (secured)
// ------------------------------

/**
 * @brief Send data securely over a TSL session.
 *
 * @param ctx Pointer to the fossil_tsl_context_t.
 * @param data Pointer to the data buffer to send.
 * @param len Number of bytes to send.
 * @return Number of bytes sent, or negative value on error.
 */
ssize_t fossil_tsl_send(fossil_tsl_context_t *ctx,
                        const void *data, size_t len);

/**
 * @brief Receive data securely from a TSL session.
 *
 * @param ctx Pointer to the fossil_tsl_context_t.
 * @param buffer Pointer to the buffer to receive data into.
 * @param len Maximum number of bytes to receive.
 * @return Number of bytes received, or negative value on error.
 */
ssize_t fossil_tsl_recv(fossil_tsl_context_t *ctx,
                        void *buffer, size_t len);

// ------------------------------
// Convenience
// ------------------------------

/**
 * @brief Wrap an existing socket into a secured TSL session.
 *
 * @param sock Pointer to the fossil_network_socket_t to wrap.
 * @param out_ctx Address of a pointer to receive the new fossil_tsl_context_t.
 * @return 0 on success, negative value on error.
 */
int fossil_tsl_wrap_socket(fossil_network_socket_t *sock,
                           fossil_tsl_context_t **out_ctx);

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

#endif /* FOSSIL_NETWORK_TLS_H */
