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
#ifndef FOSSIL_NETWORK_CLIENT_H
#define FOSSIL_NETWORK_CLIENT_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
CLIENT INTERFACE
=============================================================================*/

/**
 * @brief Opaque client handle.
 */
typedef struct fossil_net_client fossil_net_client_t;

/**
 * @brief Create a new network client.
 *
 * Allocates and initializes a client for the specified protocol and address family.
 *
 * @param type   Socket type string ID ("tcp", "udp", etc.).
 * @param family Address family string ID ("ipv4", "ipv6").
 * @return Pointer to client handle, or NULL on failure.
 */
fossil_net_client_t *fossil_net_client_create(const char *type, const char *family);

/**
 * @brief Destroy a network client and release its resources.
 *
 * @param client Pointer to client handle.
 */
void fossil_net_client_destroy(fossil_net_client_t *client);

/**
 * @brief Connect the client to a remote address.
 *
 * @param client Pointer to client handle.
 * @param addr   Pointer to remote address structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_connect(fossil_net_client_t *client, const fossil_net_address_t *addr);

/**
 * @brief Disconnect the client from the remote host.
 *
 * @param client Pointer to client handle.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_disconnect(fossil_net_client_t *client);

/**
 * @brief Send data through the client connection.
 *
 * @param client Pointer to client handle.
 * @param data   Pointer to data buffer to send.
 * @param size   Size of data buffer in bytes.
 * @param sent   Pointer to variable to receive number of bytes sent.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_send(fossil_net_client_t *client, const void *data, uint32_t size, uint32_t *sent);

/**
 * @brief Receive data from the client connection.
 *
 * @param client   Pointer to client handle.
 * @param buffer   Pointer to buffer to receive data.
 * @param size     Size of buffer in bytes.
 * @param received Pointer to variable to receive number of bytes received.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_receive(fossil_net_client_t *client, void *buffer, uint32_t size, uint32_t *received);

/**
 * @brief Get the local address of the client.
 *
 * @param client Pointer to client handle.
 * @param addr   Pointer to address structure to fill with local address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_get_local_address(fossil_net_client_t *client, fossil_net_address_t *addr);

/**
 * @brief Get the remote address the client is connected to.
 *
 * @param client Pointer to client handle.
 * @param addr   Pointer to address structure to fill with remote address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_get_remote_address(fossil_net_client_t *client, fossil_net_address_t *addr);

/**
 * @brief Set blocking or non-blocking mode for the client.
 *
 * @param client   Pointer to client handle.
 * @param blocking true for blocking, false for non-blocking.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_client_set_blocking(fossil_net_client_t *client, bool blocking);

/**
 * @brief Get the last error code for the client.
 *
 * @param client Pointer to client handle.
 * @return Error code (platform-specific).
 */
int fossil_net_client_error_last(fossil_net_client_t *client);

/**
 * @brief Convert a client error code to a human-readable string.
 *
 * @param err Error code.
 * @return Pointer to static string describing the error.
 */
const char *fossil_net_client_error_string(int err);


#ifdef __cplusplus
}
#include <string>
#include <cstring>

namespace fossil::net
{

    class Client
    {
    private:
        fossil_net_client_t *handle_;

    public:
        //=============================================================================
        // CLIENT INTERFACE
        //=============================================================================

        /**
         * @brief Construct a new Client object.
         *
         * Allocates and initializes a client for the specified protocol and address family.
         */
        Client(const char *type, const char *family)
            : handle_(fossil_net_client_create(type, family))
        {}

        /**
         * @brief Destroy the Client object and release its resources.
         */
        ~Client()
        {
            if (handle_)
                fossil_net_client_destroy(handle_);
        }

        /**
         * @brief Connect the client to a remote address.
         */
        int connect(const fossil_net_address_t *addr)
        {
            return fossil_net_client_connect(handle_, addr);
        }

        /**
         * @brief Disconnect the client from the remote host.
         */
        int disconnect()
        {
            return fossil_net_client_disconnect(handle_);
        }

        /**
         * @brief Send data through the client connection.
         */
        int send(const void *data, uint32_t size, uint32_t *sent)
        {
            return fossil_net_client_send(handle_, data, size, sent);
        }

        /**
         * @brief Receive data from the client connection.
         */
        int receive(void *buffer, uint32_t size, uint32_t *received)
        {
            return fossil_net_client_receive(handle_, buffer, size, received);
        }

        /**
         * @brief Get the local address of the client.
         */
        int get_local_address(fossil_net_address_t *addr)
        {
            return fossil_net_client_get_local_address(handle_, addr);
        }

        /**
         * @brief Get the remote address the client is connected to.
         */
        int get_remote_address(fossil_net_address_t *addr)
        {
            return fossil_net_client_get_remote_address(handle_, addr);
        }

        /**
         * @brief Set blocking or non-blocking mode for the client.
         */
        int set_blocking(bool blocking)
        {
            return fossil_net_client_set_blocking(handle_, blocking);
        }

        /**
         * @brief Get the last error code for the client.
         */
        int error_last()
        {
            return fossil_net_client_error_last(handle_);
        }

        /**
         * @brief Convert a client error code to a human-readable string.
         */
        static const char *error_string(int err)
        {
            return fossil_net_client_error_string(err);
        }

        /**
         * @brief Get the underlying C handle.
         */
        fossil_net_client_t *native_handle() const
        {
            return handle_;
        }

        // Disable copy
        Client(const Client &) = delete;
        Client &operator=(const Client &) = delete;

        // Allow move
        Client(Client &&other) noexcept : handle_(other.handle_)
        {
            other.handle_ = nullptr;
        }
        Client &operator=(Client &&other) noexcept
        {
            if (this != &other)
            {
                if (handle_)
                    fossil_net_client_destroy(handle_);
                handle_ = other.handle_;
                other.handle_ = nullptr;
            }
            return *this;
        }
    };

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_SOCKET_H */
