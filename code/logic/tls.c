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
#include "fossil/network/tls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>  // for read/write on POSIX
#ifdef _WIN32
#include <winsock2.h>
#endif

// *****************************************************************************
// TLS Context Structure (stub)
// *****************************************************************************
struct fossil_network_tls_ctx {
    fossil_network_socket_t *sock; // underlying socket
    int is_server;
};

// *****************************************************************************
// Public API (stub)
// *****************************************************************************
fossil_network_tls_ctx_t* fossil_network_tls_create(int is_server) {
    fossil_network_tls_ctx_t *ctx = (fossil_network_tls_ctx_t*)calloc(1, sizeof(fossil_network_tls_ctx_t));
    if (!ctx) return NULL;
    ctx->is_server = is_server;
    ctx->sock = NULL;
    return ctx;
}

void fossil_network_tls_free(fossil_network_tls_ctx_t *ctx) {
    if (!ctx) return;
    free(ctx);
}

int fossil_network_tls_wrap(fossil_network_tls_ctx_t *ctx,
                            fossil_network_socket_t *sock) {
    if (!ctx || !sock) return -1;
    ctx->sock = sock;
    return 0; // stub always succeeds
}

ssize_t fossil_network_tls_send(fossil_network_tls_ctx_t *ctx,
                                const void *buf, size_t len) {
    if (!ctx || !ctx->sock) return -1;
#ifdef _WIN32
    return send(ctx->sock->fd, buf, (int)len, 0);
#else
    return write(ctx->sock->fd, buf, len);
#endif
}

ssize_t fossil_network_tls_recv(fossil_network_tls_ctx_t *ctx,
                                void *buf, size_t len) {
    if (!ctx || !ctx->sock) return -1;
#ifdef _WIN32
    return recv(ctx->sock->fd, buf, (int)len, 0);
#else
    return read(ctx->sock->fd, buf, len);
#endif
}
