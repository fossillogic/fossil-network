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
#ifndef FOSSIL_NETWORK_SERVER_H
#define FOSSIL_NETWORK_SERVER_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    fossil_network_socket_t listener;
    void (*on_connect)(fossil_network_socket_t *client);
    void (*on_recv)(fossil_network_socket_t *client, const void *data, size_t len);
    void (*on_disconnect)(fossil_network_socket_t *client);
    int running;
} fossil_network_server_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Start the server and begin listening on the specified address and port.
 *
 * @param server Pointer to the server structure.
 * @param address Address to bind the server to.
 * @param port Port number to listen on.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_server_start(fossil_network_server_t *server, const char *address, uint16_t port);

/**
 * @brief Stop the server and close all connections.
 *
 * @param server Pointer to the server structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_server_stop(fossil_network_server_t *server);

#ifdef __cplusplus
}

namespace fossil {

    namespace network {

        /**
         * @class Server
         * @brief C++ wrapper for fossil_network_server_t and related functions.
         *
         * This class provides a high-level interface for managing a network server.
         * It wraps the underlying C functions for fossil_network_server_t, allowing
         * users to start and stop the server using C++ methods.
         */
        class Server {
        public:
            /**
             * @brief Handle to the underlying C network server structure.
             *
             * This member holds the state and socket information for the server.
             */
            fossil_network_server_t handle;

            /**
             * @brief Default constructor.
             *
             * Initializes the server handle and sets the running status to stopped.
             */
            Server() { handle.running = 0; }

            /**
             * @brief Starts the server and begins listening on the specified address and port.
             *
             * @param address Address to bind the server to.
             * @param port Port number to listen on.
             * @return 0 on success, non-zero on failure.
             */
            int start(const char *address, uint16_t port) {
            return fossil_network_server_start(&handle, address, port);
            }

            /**
             * @brief Stops the server and closes all connections.
             *
             * @return 0 on success, non-zero on failure.
             */
            int stop() {
            return fossil_network_server_stop(&handle);
            }
        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
