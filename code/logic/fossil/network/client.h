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

#define FOSSIL_NETWORK_CLIENT_MAX 32

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Connect the client to the specified host and port using a protocol identifier.
 *
 * @param proto_id Protocol identifier string.
 * @param host Hostname or IP address to connect to.
 * @param port Port number to connect to.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_client_connect(const char *proto_id, const char *host, uint16_t port);

/**
 * @brief Send data through the client socket using a client identifier.
 *
 * @param client_id Client identifier string.
 * @param buf Pointer to the data to send.
 * @param len Length of the data to send.
 * @return Number of bytes sent on success, -1 on failure.
 */
int fossil_network_client_send(const char *client_id, const void *buf, size_t len);

/**
 * @brief Receive data from the client socket using a client identifier.
 *
 * @param client_id Client identifier string.
 * @param buf Pointer to the buffer to receive data.
 * @param len Length of the buffer.
 * @return Number of bytes received on success, -1 on failure.
 */
int fossil_network_client_recv(const char *client_id, void *buf, size_t len);

/**
 * @brief Disconnect the client and release resources using a client identifier.
 *
 * @param client_id Client identifier string.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_client_disconnect(const char *client_id);

#ifdef __cplusplus
}
#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

namespace fossil {

    namespace network {

        /**
         * @class Client
         * @brief C++ wrapper for fossil_network_client_* functions using protocol and client identifiers.
         *
         * This class provides a high-level interface for managing a network client.
         * It wraps the underlying C functions, allowing users to connect, send, receive,
         * and disconnect using C++ methods and string identifiers.
         */
        class Client {
        public:
            std::string proto_id;
            std::string client_id;
            bool connected;

            Client(const std::string& protocol, const std::string& client)
            : proto_id(protocol), client_id(client), connected(false) {}

            int connect(const char *host, uint16_t port) {
                // Validate protocol and host
                if (proto_id.empty() || !host || std::string(host).empty())
                    return -1;
                int res = fossil_network_client_connect(proto_id.c_str(), host, port);
                connected = (res == 0);
                return res;
            }

            int send(const void *data, size_t len) {
                // Validate client_id, data, and length, and connection state
                if (client_id.empty() || !data || len == 0 || !connected)
                    return -1;
                return fossil_network_client_send(client_id.c_str(), data, len);
            }

            int recv(void *data, size_t len) {
                // Validate client_id, data, and length, and connection state
                if (client_id.empty() || !data || len == 0 || !connected)
                    return -1;
                return fossil_network_client_recv(client_id.c_str(), data, len);
            }

            int disconnect() {
                // Validate client_id and connection state
                if (client_id.empty() || !connected)
                    return -1;
                int res = fossil_network_client_disconnect(client_id.c_str());
                connected = false;
                return res;
            }

        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
