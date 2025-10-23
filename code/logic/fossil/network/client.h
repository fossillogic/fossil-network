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
#ifndef FOSSIL_NETWORK_CLIENT_H
#define FOSSIL_NETWORK_CLIENT_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    fossil_network_socket_t socket;
    int connected;
} fossil_network_client_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Connect the client to the specified host and port.
 *
 * @param client Pointer to the client structure.
 * @param host Hostname or IP address to connect to.
 * @param port Port number to connect to.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_client_connect(fossil_network_client_t *client, const char *host, uint16_t port);

/**
 * @brief Send data through the client socket.
 *
 * @param client Pointer to the client structure.
 * @param data Pointer to the data to send.
 * @param len Length of the data to send.
 * @return Number of bytes sent on success, -1 on failure.
 */
ssize_t fossil_network_client_send(fossil_network_client_t *client, const void *data, size_t len);

/**
 * @brief Receive data from the client socket.
 *
 * @param client Pointer to the client structure.
 * @param data Pointer to the buffer to receive data.
 * @param len Length of the buffer.
 * @return Number of bytes received on success, -1 on failure.
 */
ssize_t fossil_network_client_recv(fossil_network_client_t *client, void *data, size_t len);

/**
 * @brief Reconnect the client to the last known host and port.
 *
 * @param client Pointer to the client structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_client_reconnect(fossil_network_client_t *client);

/**
 * @brief Close the client connection and release resources.
 *
 * @param client Pointer to the client structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_client_close(fossil_network_client_t *client);

#ifdef __cplusplus
}

namespace fossil {

    namespace network {

        /**
         * @class Client
         * @brief C++ wrapper for fossil_network_client_t and related functions.
         *
         * This class provides a high-level interface for managing a network client.
         * It wraps the underlying C functions for fossil_network_client_t, allowing
         * users to connect to a remote host, send and receive data, reconnect, and
         * close the connection using C++ methods.
         */
        class Client {
        public:
            /**
             * @brief Handle to the underlying C network client structure.
             *
             * This member holds the state and socket information for the client.
             */
            fossil_network_client_t handle;

            /**
             * @brief Default constructor.
             *
             * Initializes the client handle and sets the connection status to disconnected.
             */
            Client() { handle.connected = 0; }

            /**
             * @brief Connects the client to the specified host and port.
             *
             * @param host Hostname or IP address to connect to.
             * @param port Port number to connect to.
             * @return 0 on success, non-zero on failure.
             */
            int connect(const char *host, uint16_t port) {
            return fossil_network_client_connect(&handle, host, port);
            }

            /**
             * @brief Sends data through the client socket.
             *
             * @param data Pointer to the data to send.
             * @param len Length of the data to send.
             * @return Number of bytes sent on success, -1 on failure.
             */
            ssize_t send(const void *data, size_t len) {
            return fossil_network_client_send(&handle, data, len);
            }

            /**
             * @brief Receives data from the client socket.
             *
             * @param data Pointer to the buffer to receive data.
             * @param len Length of the buffer.
             * @return Number of bytes received on success, -1 on failure.
             */
            ssize_t recv(void *data, size_t len) {
            return fossil_network_client_recv(&handle, data, len);
            }

            /**
             * @brief Reconnects the client to the last known host and port.
             *
             * @return 0 on success, non-zero on failure.
             */
            int reconnect() {
            return fossil_network_client_reconnect(&handle);
            }

            /**
             * @brief Closes the client connection and releases resources.
             *
             * @return 0 on success, non-zero on failure.
             */
            int close() {
            return fossil_network_client_close(&handle);
            }
        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
