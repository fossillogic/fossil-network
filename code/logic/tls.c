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

#include <openssl/ssl.h>
#include <openssl/err.h>

// *****************************************************************************
// TLS Context Structure
// *****************************************************************************
struct fossil_network_tls_ctx {
    SSL_CTX *ssl_ctx;
    SSL     *ssl;
    int      is_server;
};

// *****************************************************************************
// Internal helpers
// *****************************************************************************
static void fossil_network_tls_init_lib(void) {
    static int initialized = 0;
    if (!initialized) {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();
        initialized = 1;
    }
}

static void fossil_network_tls_cleanup_lib(void) {
    EVP_cleanup();
}

// *****************************************************************************
// Public API
// *****************************************************************************
fossil_network_tls_ctx_t* fossil_network_tls_create(int is_server) {
    fossil_network_tls_init_lib();

    fossil_network_tls_ctx_t *ctx =
        (fossil_network_tls_ctx_t*)calloc(1, sizeof(fossil_network_tls_ctx_t));
    if (!ctx) return NULL;

    ctx->is_server = is_server;

    const SSL_METHOD *method = is_server ?
        TLS_server_method() : TLS_client_method();

    ctx->ssl_ctx = SSL_CTX_new(method);
    if (!ctx->ssl_ctx) {
        free(ctx);
        return NULL;
    }

    // Configure reasonable defaults
    SSL_CTX_set_options(ctx->ssl_ctx,
                        SSL_OP_NO_SSLv2 |
                        SSL_OP_NO_SSLv3 |
                        SSL_OP_NO_COMPRESSION);

    return ctx;
}

void fossil_network_tls_free(fossil_network_tls_ctx_t *ctx) {
    if (!ctx) return;

    if (ctx->ssl) {
        SSL_shutdown(ctx->ssl);
        SSL_free(ctx->ssl);
    }

    if (ctx->ssl_ctx)
        SSL_CTX_free(ctx->ssl_ctx);

    free(ctx);

    fossil_network_tls_cleanup_lib();
}

int fossil_network_tls_wrap(fossil_network_tls_ctx_t *ctx,
                            fossil_network_socket_t *sock) {
    if (!ctx || !ctx->ssl_ctx || !sock) return -1;

    ctx->ssl = SSL_new(ctx->ssl_ctx);
    if (!ctx->ssl) return -1;

    SSL_set_fd(ctx->ssl, sock->fd);

    int ret = ctx->is_server ? SSL_accept(ctx->ssl) : SSL_connect(ctx->ssl);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ctx->ssl);
        ctx->ssl = NULL;
        return -1;
    }

    return 0;
}

ssize_t fossil_network_tls_send(fossil_network_tls_ctx_t *ctx,
                                const void *buf, size_t len) {
    if (!ctx || !ctx->ssl) return -1;
    int ret = SSL_write(ctx->ssl, buf, (int)len);
    return (ret > 0) ? ret : -1;
}

ssize_t fossil_network_tls_recv(fossil_network_tls_ctx_t *ctx,
                                void *buf, size_t len) {
    if (!ctx || !ctx->ssl) return -1;
    int ret = SSL_read(ctx->ssl, buf, (int)len);
    return (ret > 0) ? ret : -1;
}
