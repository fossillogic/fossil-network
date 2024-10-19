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

// common standard headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// network related headers
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

/** ============================================================================
 * Fossil Logic Network Server API:
 * data types
 * -----------------------------------------------------------------------------
*/

enum {
    FOSSIL_NET_ERROR = -1,
    FOSSIL_NET_SUCCESS = 0
};

// Structure for the client socket, designed for extensibility
typedef struct {
    int socket_fd;                  // File descriptor for the client socket
    struct sockaddr_in client_addr; // Address of the connected client
    socklen_t addr_len;             // Length of the client address structure
} fossil_net_client_socket_t;

// Enum for protocol types
typedef enum {
    FOSSIL_NET_PROTOCOL_TCP,
    FOSSIL_NET_PROTOCOL_UDP,
    FOSSIL_NET_PROTOCOL_HTTP,
    FOSSIL_NET_PROTOCOL_HTTPS,
    FOSSIL_NET_PROTOCOL_FTP,
    FOSSIL_NET_PROTOCOL_SFTP
    // Future protocols can be added here
} fossil_net_protocol_t;

// Structure for the server socket, designed for extensibility
typedef struct fossil_net_server_socket_t {
    int socket_fd;                  // The file descriptor for the server socket
    fossil_net_protocol_t protocol; // Protocol type (TCP, UDP, etc.)
    char ip[16];                    // IP address (e.g., "127.0.0.1")
    uint16_t port;                  // Port number
} fossil_net_server_socket_t;

/** ============================================================================
 * Fossil Logic Network Server API:
 * Function Prototypes
 * -----------------------------------------------------------------------------
*/

/**
 * @brief Creates a server socket bound to the specified IP address and port.
 * 
 * @param ip The IP address to bind the server socket to.
 * @param port The port number to bind the server socket to.
 * @param protocol The protocol type (e.g., TCP, UDP).
 * @return A pointer to a fossil_net_server_socket_t on success, NULL on failure.
 */
fossil_net_server_socket_t* fossil_net_create_server(const char* ip, uint16_t port, fossil_net_protocol_t protocol);

/**
 * @brief Accepts a client connection on the server socket.
 * 
 * @param server_socket The server socket to accept connections on.
 * @return A pointer to a fossil_net_client_socket_t representing the client,
 *         NULL on failure.
 */
fossil_net_client_socket_t* fossil_net_accept_client(fossil_net_server_socket_t* server_socket);

/**
 * @brief Sends data to the connected client.
 * 
 * @param server_socket The server socket to send data from.
 * @param buffer A pointer to the data to send.
 * @param length The length of the data to send.
 * @return The number of bytes sent on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_send_to_client(fossil_net_client_socket_t* client_socket, const void* buffer, size_t length);

/**
 * @brief Receives data from the connected client.
 * 
 * @param server_socket The server socket to receive data from.
 * @param buffer A pointer to the buffer to store received data.
 * @param length The maximum number of bytes to receive.
 * @return The number of bytes received on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_receive_from_client(fossil_net_client_socket_t* client_socket, void* buffer, size_t length);

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

/**
 * @brief Gets the last error code for the server socket operations.
 * 
 * @return The last error code.
 */
int fossil_net_get_last_error(void);

/**
 * @brief Sets a timeout for client connections.
 * 
 * @param server_socket The server socket to set the timeout for.
 * @param timeout The timeout value in seconds.
 * @return FOSSIL_NET_SUCCESS on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_set_connection_timeout(fossil_net_server_socket_t* server_socket, int timeout);

/**
 * @brief Gracefully shuts down the server, closing all connections.
 * 
 * @param server_socket The server socket to shut down.
 */
void fossil_net_shutdown_server(fossil_net_server_socket_t* server_socket);

/**
 * @brief Sets an option for the server socket (e.g., reuse address).
 * 
 * @param server_socket The server socket to set the option for.
 * @param option The option to set.
 * @param value The value to assign to the option.
 * @return FOSSIL_NET_SUCCESS on success, FOSSIL_NET_ERROR on failure.
 */
int32_t fossil_net_set_server_option(fossil_net_server_socket_t* server_socket, int option, int value);

/**
 * @brief Cleans up resources associated with a client socket.
 * 
 * @param client_socket The client socket to clean up.
 */
void fossil_net_destroy_client(fossil_net_client_socket_t* client_socket);

/**
 * @brief Gets the IP address of the connected client.
 * 
 * @param client_socket The client socket to retrieve the IP address from.
 * @return The IP address as a string.
 */
const char* fossil_net_get_client_ip(fossil_net_client_socket_t* client_socket);

/**
 * @brief Gets the port number of the connected client.
 * 
 * @param client_socket The client socket to retrieve the port number from.
 * @return The port number.
 */
uint16_t fossil_net_get_client_port(fossil_net_client_socket_t* client_socket);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_NETWORK_FRAMEWORK_H */
