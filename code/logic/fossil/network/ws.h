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
#ifndef FOSSIL_NETWORK_WS_H
#define FOSSIL_NETWORK_WS_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Perform WebSocket client handshake.
 *
 * @param sock Connected TCP socket.
 * @param host Target host name.
 * @param path WebSocket path (e.g. "/chat").
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_wsocket_handshake(fossil_network_socket_t *sock,
                                const char *host, const char *path);

/**
 * @brief Send a WebSocket text frame.
 */
int fossil_network_wsocket_send_text(fossil_network_socket_t *sock,
                                const char *msg);

/**
 * @brief Receive a WebSocket text frame.
 */
int fossil_network_wsocket_recv_text(fossil_network_socket_t *sock,
                                char *buf, size_t len);

#ifdef __cplusplus
}
#include <string>

namespace fossil {

namespace network {

    class WebSocket {
    public:
        /**
         * @brief Perform WebSocket client handshake.
         *
         * @param sock Connected TCP socket.
         * @param host Target host name.
         * @param path WebSocket path (e.g. "/chat").
         * @return 0 on success, non-zero on failure.
         */
        static int handshake(fossil_network_socket_t *sock, const std::string &host, const std::string &path) {
            return fossil_network_wsocket_handshake(sock, host.c_str(), path.c_str());
        }

        /**
         * @brief Send a WebSocket text frame.
         */
        static int send_text(fossil_network_socket_t *sock, const std::string &msg) {
            return fossil_network_wsocket_send_text(sock, msg.c_str());
        }

        /**
         * @brief Receive a WebSocket text frame.
         */
        static int recv_text(fossil_network_socket_t *sock, std::string &buf) {
            // Choose a reasonable buffer size for receiving
            constexpr size_t max_len = 4096;
            char temp[max_len];
            int ret = fossil_network_wsocket_recv_text(sock, temp, max_len);
            if (ret >= 0) {
                buf.assign(temp, ret);
            }
            return ret;
        }
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_WS_H */
