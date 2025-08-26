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

typedef struct fossil_network_tls_ctx fossil_network_tls_ctx_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Create a TLS context for client or server.
 *
 * @param is_server 1 = server mode, 0 = client mode.
 * @return New TLS context, or NULL on failure.
 */
fossil_network_tls_ctx_t* fossil_network_tls_create(int is_server);

/**
 * @brief Free a TLS context.
 */
void fossil_network_tls_free(fossil_network_tls_ctx_t *ctx);

/**
 * @brief Wrap an existing socket with TLS.
 *
 * @param ctx TLS context.
 * @param sock Underlying fossil_network_socket_t.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tls_wrap(fossil_network_tls_ctx_t *ctx,
                            fossil_network_socket_t *sock);

/**
 * @brief Send encrypted data over TLS.
 */
ssize_t fossil_network_tls_send(fossil_network_tls_ctx_t *ctx,
                                const void *buf, size_t len);

/**
 * @brief Receive decrypted data over TLS.
 */
ssize_t fossil_network_tls_recv(fossil_network_tls_ctx_t *ctx,
                                void *buf, size_t len);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_TLS_H */
