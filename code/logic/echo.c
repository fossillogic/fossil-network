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
#include "fossil/network/echo.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #define CLOSESOCKET closesocket
#else
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #define CLOSESOCKET close
#endif

// -----------------------------------------------------------------------------
// Simple Echo Server
// -----------------------------------------------------------------------------
int fossil_network_test_echo_server(uint16_t port) {
    fossil_network_socket_t server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    if (fossil_network_socket_create(&server, AF_INET, SOCK_STREAM, 0) != 0) {
        fprintf(stderr, "[echo-server] Failed to create socket\n");
        return -1;
    }

    if (fossil_network_socket_bind(&server, NULL, port) != 0) {
        fprintf(stderr, "[echo-server] Failed to bind\n");
        fossil_network_socket_close(&server);
        return -1;
    }

    if (fossil_network_socket_listen(&server, 5) != 0) {
        fprintf(stderr, "[echo-server] Failed to listen\n");
        fossil_network_socket_close(&server);
        return -1;
    }

    printf("[echo-server] Listening on port %u\n", port);

    for (;;) {
        if (fossil_network_socket_accept(&server, &client) != 0) {
            fprintf(stderr, "[echo-server] Accept failed\n");
            continue;
        }

        printf("[echo-server] Client connected\n");

        char buf[512];
        for (;;) {
            ssize_t n = recv(client.fd, buf, sizeof(buf), 0);
            if (n <= 0) {
                printf("[echo-server] Client disconnected\n");
                break;
            }
            send(client.fd, buf, (size_t)n, 0); // echo back
        }

        fossil_network_socket_close(&client);
    }

    fossil_network_socket_close(&server);
    return 0;
}

// -----------------------------------------------------------------------------
// Simple Echo Client
// -----------------------------------------------------------------------------
int fossil_network_test_echo_client(const char *host, uint16_t port) {
    fossil_network_socket_t sock;
    memset(&sock, 0, sizeof(sock));

    if (fossil_network_socket_create(&sock, AF_INET, SOCK_STREAM, 0) != 0) {
        fprintf(stderr, "[echo-client] Failed to create socket\n");
        return -1;
    }

    if (fossil_network_socket_connect(&sock, host, port) != 0) {
        fprintf(stderr, "[echo-client] Failed to connect to %s:%u\n", host, port);
        fossil_network_socket_close(&sock);
        return -1;
    }

    printf("[echo-client] Connected to %s:%u\n", host, port);

    const char *msg = "Hello, Echo Server!";
    send(sock.fd, msg, strlen(msg), 0);

    char buf[512];
    ssize_t n = recv(sock.fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) {
        buf[n] = '\0';
        printf("[echo-client] Received: %s\n", buf);
    } else {
        fprintf(stderr, "[echo-client] No response from server\n");
    }

    fossil_network_socket_close(&sock);
    return 0;
}
