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
#ifndef FOSSIL_NETWORK_HTTP_H
#define FOSSIL_NETWORK_HTTP_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char method[16];
    char path[256];
    char version[16];
} fossil_network_http_request_t;

typedef struct {
    int status_code;
    char reason[64];
} fossil_network_http_response_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Send a simple HTTP GET request.
 *
 * @param sock Connected socket.
 * @param path Resource path (e.g. "/index.html").
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_http_get(fossil_network_socket_t *sock,
                            const char *path);

/**
 * @brief Read an HTTP response.
 *
 * @param sock Connected socket.
 * @param resp Output response structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_http_read_response(fossil_network_socket_t *sock,
                                      fossil_network_http_response_t *resp);

#ifdef __cplusplus
}
#include <string>

namespace fossil {

namespace network {

    class Http {
    public:
        /**
         * @brief Send a simple HTTP GET request.
         */
        static int get(fossil_network_socket_t *sock, const std::string &path) {
            return fossil_network_http_get(sock, path.c_str());
        }

        /**
         * @brief Read an HTTP response.
         */
        static int read_response(fossil_network_socket_t *sock,
                                 fossil_network_http_response_t *resp) {
            return fossil_network_http_read_response(sock, resp);
        }
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_HTTP_H */
