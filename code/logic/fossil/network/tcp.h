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
#ifndef FOSSIL_NETWORK_TCP_H
#define FOSSIL_NETWORK_TCP_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Connect to a remote host using TCP.
 *
 * @param sock Pointer to initialized fossil_network_socket_t.
 * @param host Remote hostname or IP address.
 * @param port Remote TCP port.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_connect(fossil_network_socket_t *sock,
                               const char *host, uint16_t port);

/**
 * @brief Start a TCP server socket.
 *
 * @param sock Pointer to initialized fossil_network_socket_t.
 * @param host Local address to bind (NULL = any).
 * @param port Local port to bind.
 * @param backlog Max pending connections.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_listen(fossil_network_socket_t *sock,
                              const char *host, uint16_t port,
                              int backlog);

/**
 * @brief Accept an incoming TCP connection.
 *
 * @param server Listening socket.
 * @param client Output socket for accepted connection.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_tcp_accept(fossil_network_socket_t *server,
                              fossil_network_socket_t *client);

#ifdef __cplusplus
}
#include <string>

namespace fossil {

namespace network {

    /**
     * @brief C++ wrapper class for TCP socket operations.
     */
    class Tcp {
    public:
        /**
         * @brief Constructor. Initializes the underlying socket structure.
         */
        Tcp() { /* Initialize socket struct */ }

        /**
         * @brief Destructor. Cleans up the socket if needed.
         */
        ~Tcp() { /* Cleanup socket if needed */ }

        /**
         * @brief Connect to a remote host.
         * @param host Remote hostname or IP address.
         * @param port Remote TCP port.
         * @return 0 on success, non-zero on failure.
         */
        int connect(const std::string& host, uint16_t port) {
            return fossil_network_tcp_connect(&sock_, host.c_str(), port);
        }

        /**
         * @brief Start listening as a TCP server.
         * @param host Local address to bind (empty = any).
         * @param port Local port to bind.
         * @param backlog Maximum number of pending connections.
         * @return 0 on success, non-zero on failure.
         */
        int listen(const std::string& host, uint16_t port, int backlog) {
            const char* host_cstr = host.empty() ? nullptr : host.c_str();
            return fossil_network_tcp_listen(&sock_, host_cstr, port, backlog);
        }

        /**
         * @brief Accept an incoming TCP connection.
         * @param client Reference to Tcp object for the accepted connection.
         * @return 0 on success, non-zero on failure.
         */
        int accept(Tcp& client) {
            return fossil_network_tcp_accept(&sock_, &client.sock_);
        }

        /**
         * @brief Get a pointer to the underlying C socket structure.
         * @return Pointer to fossil_network_socket_t.
         */
        fossil_network_socket_t* native_handle() { return &sock_; }

    private:
        fossil_network_socket_t sock_; /**< Underlying C socket structure. */
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_TCP_H */
