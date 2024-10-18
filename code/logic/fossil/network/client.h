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

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a client socket and connects to the specified server.
 * 
 * @param ip The IP address of the server to connect to.
 * @param port The port number of the server.
 * @return A pointer to a fossil_net_client_socket_t on success, NULL on failure.
 */
fossil_net_client_socket_t* fossil_net_create_client(const char* ip, uint16_t port);

/**
 * @brief Accepts a client connection on a server socket.
 * 
 * @param server_socket The server socket to accept connections on.
 * @param client_socket The client socket to store the connection details.
 * @return FOSSIL_NET_SUCCESS on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_accept_client(fossil_net_server_socket_t* server_socket, fossil_net_client_socket_t* client_socket);

/**
 * @brief Sends data to a connected client socket.
 * 
 * @param socket The client socket to send data to.
 * @param buffer A pointer to the data to send.
 * @param length The length of the data to send.
 * @return The number of bytes sent on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_send_client(fossil_net_client_socket_t* socket, const void* buffer, size_t length);

/**
 * @brief Receives data from a connected client socket.
 * 
 * @param socket The client socket to receive data from.
 * @param buffer A pointer to a buffer to store the received data.
 * @param length The maximum amount of data to receive.
 * @return The number of bytes received on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_receive_client(fossil_net_client_socket_t* socket, void* buffer, size_t length);

/**
 * @brief Closes a client socket.
 * 
 * @param socket The client socket to close.
 */
void fossil_net_close_client(fossil_net_client_socket_t* socket);

/**
 * @brief Destroys a client socket and frees the associated resources.
 * 
 * @param socket The client socket to destroy.
 */
void fossil_net_destroy_client(fossil_net_client_socket_t* socket);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_NETWORK_FRAMEWORK_H */
