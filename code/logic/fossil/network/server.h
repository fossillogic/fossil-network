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

/**
 * @brief Create and start a listening server on the specified address and port.
 *
 * @param proto_id Protocol name (e.g., "tcp", "udp", "http").
 * @param address  Local address to bind to (e.g., "127.0.0.1" or "0.0.0.0").
 * @param port     Port number to listen on.
 *
 * @return 0 on success, or negative on error.
 */
int fossil_network_server_listen(const char *proto_id, const char *address, uint16_t port);

/**
 * @brief Accept an incoming client connection.
 *
 * Only valid for connection-oriented protocols such as TCP.
 *
 * @param server_id Unique ID string for the active server.
 *
 * @return Client index (>=0) on success, negative on failure.
 */
int fossil_network_server_accept(const char *server_id);

/**
 * @brief Broadcast data to all connected clients.
 *
 * @param server_id Unique ID string for the active server.
 * @param data      Pointer to buffer containing data to send.
 * @param len       Length of the buffer in bytes.
 *
 * @return Number of clients successfully sent to, or negative on error.
 */
int fossil_network_server_broadcast(const char *server_id, const void *data, size_t len);

/**
 * @brief Close a running server and all associated client connections.
 *
 * @param server_id Unique ID string for the active server.
 *
 * @return 0 on success, negative on failure.
 */
int fossil_network_server_close(const char *server_id);

#ifdef __cplusplus
}
#include <string>
#include <cstring>

namespace fossil {

    namespace network {

        /**
         * @class Server
         * @brief C++ RAII wrapper for Fossil network server functions.
         *
         * This class provides a C++ interface for managing a Fossil network server.
         * It wraps the C API functions with RAII semantics, ensuring that resources
         * are properly released when the object is destroyed.
         */
        class Server {
        public:
            /**
             * @brief Unique identifier for the active server instance.
             *
             * This string is constructed from the protocol, address, and port,
             * and is used to reference the server in C API calls.
             */
            std::string server_id;

            /**
             * @brief Default constructor.
             *
             * Initializes an empty server instance. No server is started.
             */
            Server() = default;

            /**
             * @brief Destructor.
             *
             * Automatically closes the server and releases all associated resources
             * if the server is running.
             */
            ~Server() {
            if (!server_id.empty()) {
                fossil_network_server_close(server_id.c_str());
            }
            }

            /**
             * @brief Start a listening server.
             *
             * Attempts to create and start a server using the specified protocol,
             * address, and port. Only known protocols ("tcp", "udp", "http") are allowed.
             *
             * @param proto_id Protocol name (e.g., "tcp", "udp", "http").
             * @param address  Local address to bind to (e.g., "127.0.0.1").
             * @param port     Port number to listen on.
             * @return 0 on success, negative on error.
             */
            int listen(const char *proto_id, const char *address, uint16_t port) {
            // Validate parameters
            if (!proto_id || !address || port == 0) {
                return -1;
            }
            // Only allow known protocols
            if (std::strcmp(proto_id, "tcp") != 0 && std::strcmp(proto_id, "udp") != 0 && std::strcmp(proto_id, "http") != 0) {
                return -1;
            }
            // Construct server ID from protocol, address, and port
            server_id = std::string(proto_id) + ":" + address + ":" + std::to_string(port);
            int rc = fossil_network_server_listen(proto_id, address, port);
            if (rc != 0) server_id.clear();
            return rc;
            }

            /**
             * @brief Accept an incoming client connection.
             *
             * Only valid for connection-oriented protocols such as TCP.
             * Returns the client index on success, or negative on failure.
             *
             * @return Client index (>=0) on success, negative on failure.
             */
            int accept() {
            if (server_id.empty()) return -1;
            int rc = fossil_network_server_accept(server_id.c_str());
            if (rc < 0) return -1;
            return rc;
            }

            /**
             * @brief Broadcast data to all connected clients.
             *
             * Sends the specified buffer to all clients connected to the server.
             *
             * @param data Pointer to buffer containing data to send.
             * @param len  Length of the buffer in bytes.
             * @return Number of clients successfully sent to, or negative on error.
             */
            int broadcast(const void *data, size_t len) {
            if (server_id.empty() || !data || len == 0) return -1;
            int rc = fossil_network_server_broadcast(server_id.c_str(), data, len);
            if (rc < 0) return -1;
            return rc;
            }

            /**
             * @brief Close the running server and all associated client connections.
             *
             * Releases all resources and resets the server state.
             *
             * @return 0 on success, negative on failure.
             */
            int close() {
            if (server_id.empty()) return 0;
            int rc = fossil_network_server_close(server_id.c_str());
            server_id.clear();
            if (rc < 0) return rc;
            return 0;
            }
        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
