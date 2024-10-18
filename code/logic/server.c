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
#include "fossil/network/server.h"

// Creates a server socket
fossil_net_server_socket_t* fossil_net_create_server(const char* ip, uint16_t port) {
    // Allocate memory for the server socket structure
    fossil_net_server_socket_t* server_socket = fossil_network_alloc(sizeof(fossil_net_server_socket_t));
    if (!server_socket) {
        printf("Memory allocation for server socket failed");
        return NULL;
    }

    // Create the socket
    server_socket->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket->socket_fd < 0) {
        printf("Socket creation failed: %s", strerror(errno));
        fossil_network_free(server_socket);
        return NULL;
    }

    // Set socket options (e.g., allow address reuse)
    int opt = 1;
    if (setsockopt(server_socket->socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
        close(server_socket->socket_fd);
        fossil_network_free(server_socket);
        return NULL;
    }

    // Set up the server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;

    // Use inet_pton for better IP address validation
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        printf("Invalid IP address format: %s", ip);
        close(server_socket->socket_fd);
        fossil_network_free(server_socket);
        return NULL;
    }

    server_addr.sin_port = htons(port);

    // Bind the socket to the provided IP and port
    if (bind(server_socket->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed: %s", strerror(errno));
        close(server_socket->socket_fd);
        fossil_network_free(server_socket);
        return NULL;
    }

    // Start listening for incoming connections
    if (listen(server_socket->socket_fd, SOMAXCONN) < 0) {
        printf("Listen failed: %s", strerror(errno));
        close(server_socket->socket_fd);
        fossil_network_free(server_socket);
        return NULL;
    }

    return server_socket;
}

// Accepts a connection on the server socket
int32_t fossil_net_accept_client(fossil_net_server_socket_t* server_socket, fossil_net_server_socket_t* client_socket) {
    if (!server_socket || !client_socket) {
        printf("Invalid socket(s) passed to accept function");
        return FOSSIL_NET_ERROR;
    }

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

// Sends data to the client
int32_t fossil_net_send_server(fossil_net_server_socket_t* socket, const void* buffer, size_t length) {
    if (!socket || socket->socket_fd < 0) {
        printf("Invalid server socket");
        return FOSSIL_NET_ERROR;
    }

    int bytes_sent = send(socket->socket_fd, buffer, length, 0);
    if (bytes_sent < 0) {
        printf("Failed to send data: %s", strerror(errno));
    }

    return bytes_sent;
}

// Receives data from the client
int32_t fossil_net_receive_server(fossil_net_server_socket_t* socket, void* buffer, size_t length) {
    if (!socket || socket->socket_fd < 0) {
        printf("Invalid server socket");
        return FOSSIL_NET_ERROR;
    }

    int bytes_received = recv(socket->socket_fd, buffer, length, 0);
    if (bytes_received < 0) {
        printf("Failed to receive data: %s", strerror(errno));
    }

    return bytes_received;
}

// Closes the server socket
void fossil_net_close_server(fossil_net_server_socket_t* socket) {
    if (socket) {
        if (socket->socket_fd != -1) {
            if (close(socket->socket_fd) < 0) {
                printf("Failed to close server socket: %s", strerror(errno));
            }
        }
        fossil_network_free(socket);
    }
}

// Cleans up the server network resources
void fossil_net_destroy_server(fossil_net_server_socket_t* socket) {
    if (socket) {
        // Close the socket if it's still open
        if (socket->socket_fd != -1) {
            if (close(socket->socket_fd) < 0) {
                printf("Failed to close server socket during destruction: %s", strerror(errno));
            }
        }

        // Free the server socket structure
        fossil_network_free(socket);
    }
}
