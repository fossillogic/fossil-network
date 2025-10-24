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

namespace fossil {

    namespace network {

        /**
         * @class Server
         * @brief C++ RAII wrapper for Fossil network server functions.
         *
         * This class provides a high-level interface for managing a network server.
         * It wraps the underlying C functions for fossil_network_server_listen,
         * fossil_network_server_accept, fossil_network_server_broadcast, and
         * fossil_network_server_close, allowing users to start, accept, broadcast,
         * and stop the server using C++ methods.
         */
        class Server {
        public:
            /**
             * @brief Unique server identifier used by the C API.
             */
            std::string server_id;

            /**
             * @brief Constructor.
             *
             * Does not start the server; call listen() to start.
             */
            Server() = default;

            /**
             * @brief Destructor.
             *
             * Ensures the server is closed if still running.
             */
            ~Server() {
                if (!server_id.empty()) {
                    fossil_network_server_close(server_id.c_str());
                }
            }

            /**
             * @brief Start listening on the specified protocol, address, and port.
             *
             * @param proto_id Protocol name (e.g., "tcp").
             * @param address  Address to bind to.
             * @param port     Port number.
             * @return 0 on success, negative on error.
             */
            int listen(const char *proto_id, const char *address, uint16_t port) {
                // Generate a unique server_id (for example, based on address/port)
                server_id = std::string(proto_id) + ":" + address + ":" + std::to_string(port);
                return fossil_network_server_listen(proto_id, address, port);
            }

            /**
             * @brief Accept an incoming client connection.
             *
             * @return Client index (>=0) on success, negative on failure.
             */
            int accept() {
                if (server_id.empty()) return -1;
                return fossil_network_server_accept(server_id.c_str());
            }

            /**
             * @brief Broadcast data to all connected clients.
             *
             * @param data Pointer to buffer containing data to send.
             * @param len  Length of the buffer in bytes.
             * @return Number of clients successfully sent to, or negative on error.
             */
            int broadcast(const void *data, size_t len) {
                if (server_id.empty()) return -1;
                return fossil_network_server_broadcast(server_id.c_str(), data, len);
            }

            /**
             * @brief Close the server and all associated client connections.
             *
             * @return 0 on success, negative on failure.
             */
            int close() {
                if (server_id.empty()) return 0;
                int result = fossil_network_server_close(server_id.c_str());
                server_id.clear();
                return result;
            }

        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
