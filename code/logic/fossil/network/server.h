/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2013
 *
 * Copyright (C) 2013-Current Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_NETWORK_SERVER_H
#define FOSSIL_NETWORK_SERVER_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
SERVER INTERFACE
=============================================================================*/

/**
 * @brief Opaque server handle.
 */
typedef struct fossil_net_server fossil_net_server_t;

/**
 * @brief Create a new server instance.
 *
 * Initializes a server to listen on the specified address and port.
 *
 * @param type   Socket type string ID ("tcp", "udp").
 * @param family Address family string ID ("ipv4", "ipv6").
 * @param addr   Local address to bind to (can be NULL for any).
 * @param port   Local port to bind to.
 * @return Pointer to server instance, or NULL on failure.
 */
fossil_net_server_t *fossil_net_server_create(
    const char *type,
    const char *family,
    const char *addr,
    uint16_t port);

/**
 * @brief Destroy a server instance and release its resources.
 *
 * @param server Pointer to server instance.
 */
void fossil_net_server_destroy(fossil_net_server_t *server);

/**
 * @brief Start listening for incoming connections.
 *
 * @param server  Pointer to server instance.
 * @param backlog Maximum length of the pending connections queue.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_server_listen(
    fossil_net_server_t *server,
    int backlog);

/**
 * @brief Accept an incoming client connection.
 *
 * Blocks or polls for a new client connection and returns a socket for communication.
 *
 * @param server Pointer to server instance.
 * @param client_sock Pointer to socket structure to initialize.
 * @param client_addr Pointer to address structure to receive client info (optional).
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_server_accept(
    fossil_net_server_t *server,
    fossil_net_socket_t *client_sock,
    fossil_net_address_t *client_addr);

/**
 * @brief Get the local address the server is bound to.
 *
 * @param server Pointer to server instance.
 * @param addr   Pointer to address structure to fill.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_server_get_address(
    fossil_net_server_t *server,
    fossil_net_address_t *addr);

/**
 * @brief Set blocking or non-blocking mode for the server socket.
 *
 * @param server   Pointer to server instance.
 * @param blocking true for blocking, false for non-blocking.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_server_set_blocking(
    fossil_net_server_t *server,
    bool blocking);

#ifdef __cplusplus
}
#include <string>
#include <cstring>

namespace fossil::net
{

    class Server
    {
    private:
        fossil_net_server_t *server_;

    public:
        /**
         * @brief Construct a new Server object.
         *
         * Wraps fossil_net_server_create.
         */
        Server(const std::string &type, const std::string &family, const std::string &addr, uint16_t port)
            : server_(nullptr)
        {
            server_ = fossil_net_server_create(type.c_str(), family.c_str(), addr.empty() ? nullptr : addr.c_str(), port);
        }

        /**
         * @brief Destructor. Releases server resources.
         *
         * Wraps fossil_net_server_destroy.
         */
        ~Server()
        {
            if (server_)
                fossil_net_server_destroy(server_);
        }

        /**
         * @brief Start listening for incoming connections.
         *
         * Wraps fossil_net_server_listen.
         */
        int listen(int backlog)
        {
            return fossil_net_server_listen(server_, backlog);
        }

        /**
         * @brief Accept an incoming client connection.
         *
         * Wraps fossil_net_server_accept.
         */
        int accept(fossil_net_socket_t *client_sock, fossil_net_address_t *client_addr = nullptr)
        {
            return fossil_net_server_accept(server_, client_sock, client_addr);
        }

        /**
         * @brief Get the local address the server is bound to.
         *
         * Wraps fossil_net_server_get_address.
         */
        int get_address(fossil_net_address_t *addr)
        {
            return fossil_net_server_get_address(server_, addr);
        }

        /**
         * @brief Set blocking or non-blocking mode for the server socket.
         *
         * Wraps fossil_net_server_set_blocking.
         */
        int set_blocking(bool blocking)
        {
            return fossil_net_server_set_blocking(server_, blocking);
        }

        /**
         * @brief Check if the server is valid.
         */
        bool is_valid() const
        {
            return server_ != nullptr;
        }

        /**
         * @brief Get the underlying C server pointer.
         */
        fossil_net_server_t *native_handle() const
        {
            return server_;
        }

        // Disable copy
        Server(const Server &) = delete;
        Server &operator=(const Server &) = delete;

        // Allow move
        Server(Server &&other) noexcept : server_(other.server_)
        {
            other.server_ = nullptr;
        }
        Server &operator=(Server &&other) noexcept
        {
            if (this != &other)
            {
                if (server_)
                    fossil_net_server_destroy(server_);
                server_ = other.server_;
                other.server_ = nullptr;
            }
            return *this;
        }
    };

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_SOCKET_H */
