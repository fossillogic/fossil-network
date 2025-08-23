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
#include "fossil/network/tsl.h"
#include <stdlib.h>
#include <string.h>

// Internal TSL context
struct fossil_tsl_context {
    fossil_network_socket_t *sock;
    int handshake_done;
    unsigned char key; // trivial demo key (XOR)
};

fossil_tsl_context_t *fossil_tsl_create(fossil_network_socket_t *sock) {
    fossil_tsl_context_t *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) return NULL;
    ctx->sock = sock;
    ctx->handshake_done = 0;
    ctx->key = 0xAA; // demo
    return ctx;
}

void fossil_tsl_free(fossil_tsl_context_t *ctx) {
    if (ctx) free(ctx);
}

int fossil_tsl_handshake(fossil_tsl_context_t *ctx) {
    if (!ctx || !ctx->sock) return -1;
    // Stub handshake: pretend exchange happened
    ctx->handshake_done = 1;
    return 0;
}

ssize_t fossil_tsl_send(fossil_tsl_context_t *ctx,
                        const void *data, size_t len) {
    if (!ctx || !ctx->handshake_done) return -1;
    unsigned char *buf = malloc(len);
    if (!buf) return -1;

    // Trivial XOR "encryption"
    for (size_t i = 0; i < len; i++)
        buf[i] = ((unsigned char *)data)[i] ^ ctx->key;

    ssize_t sent = fossil_network_socket_send(ctx->sock, buf, len);
    free(buf);
    return sent;
}

ssize_t fossil_tsl_recv(fossil_tsl_context_t *ctx,
                        void *buffer, size_t len) {
    if (!ctx || !ctx->handshake_done) return -1;

    ssize_t received = fossil_network_socket_recv(ctx->sock, buffer, len);
    if (received <= 0) return received;

    // Trivial XOR "decryption"
    for (ssize_t i = 0; i < received; i++)
        ((unsigned char *)buffer)[i] ^= ctx->key;

    return received;
}

int fossil_tsl_wrap_socket(fossil_network_socket_t *sock,
                           fossil_tsl_context_t **out_ctx) {
    if (!sock || !out_ctx) return -1;
    *out_ctx = fossil_tsl_create(sock);
    return *out_ctx ? 0 : -1;
}
