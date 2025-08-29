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

    /**
     * @brief C++ RAII wrapper for fossil_network_tls_ctx_t.
     *
     * This class manages the lifecycle of a TLS context, providing
     * convenient methods for wrapping sockets, sending, and receiving
     * data over TLS. It is non-copyable but movable.
     */
    class Tls {
    public:
        /**
         * @brief Construct a TLS context as client or server.
         * @param is_server True for server mode, false for client mode.
         */
        explicit Tls(bool is_server)
            : ctx_(fossil_network_tls_create(is_server ? 1 : 0))
        {}

        /**
         * @brief Destructor. Frees the TLS context if owned.
         */
        ~Tls() {
            if (ctx_) fossil_network_tls_free(ctx_);
        }

        // Non-copyable
        Tls(const Tls&) = delete;
        Tls& operator=(const Tls&) = delete;

        /**
         * @brief Move constructor.
         * Transfers ownership of the TLS context.
         */
        Tls(Tls&& other) noexcept : ctx_(other.ctx_) {
            other.ctx_ = nullptr;
        }

        /**
         * @brief Move assignment operator.
         * Transfers ownership of the TLS context.
         */
        Tls& operator=(Tls&& other) noexcept {
            if (this != &other) {
                if (ctx_) fossil_network_tls_free(ctx_);
                ctx_ = other.ctx_;
                other.ctx_ = nullptr;
            }
            return *this;
        }

        /**
         * @brief Wrap an existing socket with TLS.
         * @param sock Pointer to fossil_network_socket_t.
         * @return 0 on success, non-zero on failure.
         */
        int wrap(fossil_network_socket_t* sock) {
            return fossil_network_tls_wrap(ctx_, sock);
        }

        /**
         * @brief Send encrypted data over TLS.
         * @param buf Pointer to data buffer.
         * @param len Number of bytes to send.
         * @return Number of bytes sent, or negative on error.
         */
        ssize_t send(const void* buf, size_t len) {
            return fossil_network_tls_send(ctx_, buf, len);
        }

        /**
         * @brief Receive decrypted data over TLS.
         * @param buf Pointer to buffer to receive data.
         * @param len Maximum number of bytes to receive.
         * @return Number of bytes received, or negative on error.
         */
        ssize_t recv(void* buf, size_t len) {
            return fossil_network_tls_recv(ctx_, buf, len);
        }

        /**
         * @brief Get the underlying TLS context handle.
         * @return Pointer to fossil_network_tls_ctx_t.
         */
        fossil_network_tls_ctx_t* native_handle() { return ctx_; }

    private:
        fossil_network_tls_ctx_t* ctx_; /**< Underlying TLS context pointer. */
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_TLS_H */
