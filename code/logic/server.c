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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Static function to set socket options
static int set_socket_options(int socket_fd) {
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return FOSSIL_NET_ERROR;
    }
    return FOSSIL_NET_SUCCESS;
}

fossil_net_server_socket_t* fossil_net_create_server(const char* ip, uint16_t port, fossil_net_protocol_t protocol) {
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in6 server_addr6;
    int is_ipv6 = strchr(ip, ':') != NULL; // Detect if the IP is IPv6

    fossil_net_server_socket_t* server_socket = malloc(sizeof(fossil_net_server_socket_t));
    if (!server_socket) {
        perror("malloc");
        return NULL;
    }

    // Create socket based on protocol and IP version (IPv4 or IPv6)
    switch (protocol) {
        case FOSSIL_NET_PROTOCOL_TCP:
            socket_fd = is_ipv6 ? socket(AF_INET6, SOCK_STREAM, 0) : socket(AF_INET, SOCK_STREAM, 0);
            break;

        case FOSSIL_NET_PROTOCOL_UDP:
            socket_fd = is_ipv6 ? socket(AF_INET6, SOCK_DGRAM, 0) : socket(AF_INET, SOCK_DGRAM, 0);
            break;

        default:
            free(server_socket);
            return NULL; // Unsupported protocol
    }

    if (socket_fd < 0) {
        perror("socket");
        free(server_socket);
        return NULL;
    }

    if (set_socket_options(socket_fd) == FOSSIL_NET_ERROR) {
        close(socket_fd);
        free(server_socket);
        return NULL;
    }

    // Bind the socket to the specified IP and port (IPv4 or IPv6)
    if (is_ipv6) {
        memset(&server_addr6, 0, sizeof(server_addr6));
        server_addr6.sin6_family = AF_INET6;
        inet_pton(AF_INET6, ip, &server_addr6.sin6_addr);
        server_addr6.sin6_port = htons(port);

        if (bind(socket_fd, (struct sockaddr*)&server_addr6, sizeof(server_addr6)) < 0) {
            perror("bind");
            close(socket_fd);
            free(server_socket);
            return NULL;
        }
    } else {
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &server_addr.sin_addr);
        server_addr.sin_port = htons(port);

        if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("bind");
            close(socket_fd);
            free(server_socket);
            return NULL;
        }
    }

    if (protocol == FOSSIL_NET_PROTOCOL_TCP || 
        protocol == FOSSIL_NET_PROTOCOL_HTTP || 
        protocol == FOSSIL_NET_PROTOCOL_HTTPS ||
        protocol == FOSSIL_NET_PROTOCOL_FTP || 
        protocol == FOSSIL_NET_PROTOCOL_SFTP) {
        if (listen(socket_fd, SOMAXCONN) < 0) {
            perror("listen");
            close(socket_fd);
            free(server_socket);
            return NULL;
        }
    }

    server_socket->socket_fd = socket_fd;
    server_socket->protocol = protocol;
    strncpy(server_socket->ip, ip, sizeof(server_socket->ip) - 1);
    server_socket->port = port;

    return server_socket;
}

fossil_net_client_socket_t* fossil_net_accept_client(fossil_net_server_socket_t* server_socket) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server_socket->socket_fd, (struct sockaddr*)&client_addr, &addr_len);
    
    if (client_fd < 0) {
        perror("accept");
        return NULL;
    }

    // Future enhancement: create and return a client socket structure
    fossil_net_client_socket_t* client_socket = malloc(sizeof(fossil_net_client_socket_t));
    if (!client_socket) {
        perror("malloc");
        close(client_fd);
        return NULL;
    }

    client_socket->socket_fd = client_fd;
    client_socket->client_addr = client_addr;
    client_socket->addr_len = addr_len;

    return client_socket;
}

int32_t fossil_net_send_to_client(fossil_net_client_socket_t* client_socket, const void* buffer, size_t length) {
    // Send data to the connected client
    int bytes_sent = send(client_socket->socket_fd, buffer, length, 0);
    if (bytes_sent < 0) {
        perror("send");
        return FOSSIL_NET_ERROR;
    }
    return bytes_sent;
}

int32_t fossil_net_receive_from_client(fossil_net_client_socket_t* client_socket, void* buffer, size_t length) {
    // Receive data from the connected client
    int bytes_received = recv(client_socket->socket_fd, buffer, length, 0);
    if (bytes_received < 0) {
        perror("recv");
        return FOSSIL_NET_ERROR;
    }
    return bytes_received;
}

void fossil_net_close_server(fossil_net_server_socket_t* socket) {
    if (socket) {
        close(socket->socket_fd);
        free(socket);
    }
}

void fossil_net_destroy_server(fossil_net_server_socket_t* socket) {
    fossil_net_close_server(socket);
}

int fossil_net_get_last_error(void) {
    return errno;
}

int32_t fossil_net_set_connection_timeout(fossil_net_server_socket_t* server_socket, int timeout) {
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    if (setsockopt(server_socket->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
        perror("setsockopt");
        return FOSSIL_NET_ERROR;
    }
    return FOSSIL_NET_SUCCESS;
}

void fossil_net_shutdown_server(fossil_net_server_socket_t* server_socket) {
    if (server_socket) {
        // Close all client connections and the server socket
        fossil_net_close_server(server_socket);
    }
}

int32_t fossil_net_set_server_option(fossil_net_server_socket_t* server_socket, int option, int value) {
    if (setsockopt(server_socket->socket_fd, SOL_SOCKET, option, &value, sizeof(value)) < 0) {
        perror("setsockopt");
        return FOSSIL_NET_ERROR;
    }
    return FOSSIL_NET_SUCCESS;
}

/**
 * @brief Cleans up resources associated with a client socket.
 * 
 * @param client_socket The client socket to clean up.
 */
void fossil_net_destroy_client(fossil_net_client_socket_t* client_socket) {
    if (client_socket) {
        close(client_socket->socket_fd); // Close the client socket
        free(client_socket);               // Free the client socket structure
    }
}

/**
 * @brief Gets the IP address of the connected client.
 * 
 * @param client_socket The client socket to retrieve the IP address from.
 * @return The IP address as a string. Caller must free the returned string.
 */
const char* fossil_net_get_client_ip(fossil_net_client_socket_t* client_socket) {
    if (client_socket) {
        char* ip_str = malloc(INET_ADDRSTRLEN); // Allocate memory for IP string
        if (ip_str && inet_ntop(AF_INET, &client_socket->client_addr.sin_addr, ip_str, INET_ADDRSTRLEN)) {
            return ip_str; // Return dynamically allocated IP address string
        }
        free(ip_str); // Free if inet_ntop fails
    }
    return NULL; // Return NULL if there's an error
}

/**
 * @brief Gets the port number of the connected client.
 * 
 * @param client_socket The client socket to retrieve the port number from.
 * @return The port number.
 */
uint16_t fossil_net_get_client_port(fossil_net_client_socket_t* client_socket) {
    if (client_socket) {
        return ntohs(client_socket->client_addr.sin_port); // Convert to host byte order and return
    }
    return 0; // Return 0 if the client socket is NULL
}
