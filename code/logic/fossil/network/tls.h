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

#include "network.h"

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
typedef struct fossil_tsl_context fossil_tsl_context_t;

// ------------------------------
// Lifecycle
// ------------------------------
fossil_tsl_context_t *fossil_tsl_create(fossil_network_socket_t *sock);
void fossil_tsl_free(fossil_tsl_context_t *ctx);

// ------------------------------
// Handshake
// ------------------------------
// Perform a simplified TSL handshake (no real crypto, stubbed).
int fossil_tsl_handshake(fossil_tsl_context_t *ctx);

// ------------------------------
// Data transmission (secured)
// ------------------------------
ssize_t fossil_tsl_send(fossil_tsl_context_t *ctx,
                        const void *data, size_t len);

ssize_t fossil_tsl_recv(fossil_tsl_context_t *ctx,
                        void *buffer, size_t len);

// ------------------------------
// Convenience
// ------------------------------
// Wrap an existing socket into a secured TSL session.
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
