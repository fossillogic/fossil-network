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

#include "internal.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a server socket bound to the specified IP address and port.
 * 
 * @param ip The IP address to bind the server socket to.
 * @param port The port number to bind the server socket to.
 * @return A pointer to a fossil_net_server_socket_t on success, NULL on failure.
 */
fossil_net_server_socket_t* fossil_net_create_server(const char* ip, uint16_t port);

/**
 * @brief Accepts a client connection on the server socket.
 * 
 * @param server_socket The server socket to accept connections on.
 * @param client_socket A pointer to store the client connection details.
 * @return FOSSIL_NET_SUCCESS on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_accept_client(fossil_net_server_socket_t* server_socket, fossil_net_server_socket_t* client_socket);

/**
 * @brief Sends data to the client through the server socket.
 * 
 * @param socket The server socket to send data from.
 * @param buffer A pointer to the data to send.
 * @param length The length of the data to send.
 * @return The number of bytes sent on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_send_server(fossil_net_server_socket_t* socket, const void* buffer, size_t length);

/**
 * @brief Receives data from the client through the server socket.
 * 
 * @param socket The server socket to receive data from.
 * @param buffer A pointer to the buffer to store received data.
 * @param length The maximum number of bytes to receive.
 * @return The number of bytes received on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_receive_server(fossil_net_server_socket_t* socket, void* buffer, size_t length);

/**
 * @brief Closes the server socket.
 * 
 * @param socket The server socket to close.
 */
void fossil_net_close_server(fossil_net_server_socket_t* socket);

/**
 * @brief Destroys the server socket and frees the associated resources.
 * 
 * @param socket The server socket to destroy.
 */
void fossil_net_destroy_server(fossil_net_server_socket_t* socket);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_NETWORK_FRAMEWORK_H */
