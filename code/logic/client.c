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
#include "fossil/network/client.h"

// Creates a client socket
fossil_net_client_socket_t* fossil_net_create_client(const char* ip, uint16_t port) {
    // Allocate memory for client socket structure
    fossil_net_client_socket_t* client_socket = fossil_network_alloc(sizeof(fossil_net_client_socket_t));
    if (!client_socket) {
        printf("Memory allocation for client socket failed");
        return NULL;
    }

    // Initialize socket file descriptor
    client_socket->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket->socket_fd < 0) {
        printf("Socket creation failed: %s", strerror(errno));
        fossil_network_free(client_socket);
        return NULL;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Use inet_pton for better IP validation
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address format: %s", ip);
        close(client_socket->socket_fd);
        fossil_network_free(client_socket);
        return NULL;
    }

    server_addr.sin_port = htons(port);

    // Connect to the server
    if (connect(client_socket->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection to server failed: %s", strerror(errno));
        close(client_socket->socket_fd);
        fossil_network_free(client_socket);
        return NULL;
    }

    return client_socket;
}

// Accepts a client connection
int32_t fossil_net_accept_client(fossil_net_server_socket_t* server_socket, fossil_net_client_socket_t* client_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Accept the client connection
    client_socket->socket_fd = accept(server_socket->socket_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket->socket_fd < 0) {
        printf("Failed to accept client: %s", strerror(errno));
        return FOSSIL_NET_ERROR;
    }

    return FOSSIL_NET_SUCCESS;
}

// Sends data to the connected client socket
int32_t fossil_net_send_client(fossil_net_client_socket_t* socket, const void* buffer, size_t length) {
    if (!socket || socket->socket_fd < 0) {
        printf("Invalid client socket");
        return FOSSIL_NET_ERROR;
    }

    int bytes_sent = send(socket->socket_fd, buffer, length, 0);
    if (bytes_sent < 0) {
        printf("Failed to send data: %s", strerror(errno));
    }

    return bytes_sent;
}

// Receives data from the connected client socket
int32_t fossil_net_receive_client(fossil_net_client_socket_t* socket, void* buffer, size_t length) {
    if (!socket || socket->socket_fd < 0) {
        printf("Invalid client socket");
        return FOSSIL_NET_ERROR;
    }

    int bytes_received = recv(socket->socket_fd, buffer, length, 0);
    if (bytes_received < 0) {
        printf("Failed to receive data: %s", strerror(errno));
    }

    return bytes_received;
}

// Closes the client socket
void fossil_net_close_client(fossil_net_client_socket_t* socket) {
    if (socket) {
        if (socket->socket_fd != -1) {
            if (close(socket->socket_fd) < 0) {
                printf("Failed to close socket: %s", strerror(errno));
            }
        }
        fossil_network_free(socket);
    }
}

// Cleans up the client network resources
void fossil_net_destroy_client(fossil_net_client_socket_t* socket) {
    if (socket) {
        // Close the socket file descriptor if it's still open
        if (socket->socket_fd != -1) {
            if (close(socket->socket_fd) < 0) {
                printf("Failed to close client socket: %s", strerror(errno));
            }
        }

        // Free the client socket structure
        fossil_network_free(socket);
    }
}
