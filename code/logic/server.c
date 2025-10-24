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

// ============================================================================
// Fossil Network Server
// Simplified server abstraction built on fossil_network_socket_*
// ============================================================================

typedef struct fossil_network_server_t {
    char id[64];
    fossil_network_socket_t socket;
    int active;
    fossil_network_socket_t clients[16]; // Small static pool for simplicity
    size_t client_count;
} fossil_network_server_t;

// ============================================================================
// Simple server registry
// ============================================================================
#define FOSSIL_NETWORK_SERVER_MAX 16
static fossil_network_server_t fossil_servers[FOSSIL_NETWORK_SERVER_MAX];

// ---------------------------------------------------------------------------
// Internal: find a server by ID
// ---------------------------------------------------------------------------
static fossil_network_server_t *fossil_network_server_find(const char *server_id) {
    if (!server_id) return NULL;
    for (size_t i = 0; i < FOSSIL_NETWORK_SERVER_MAX; ++i) {
        if (fossil_servers[i].active && strcmp(fossil_servers[i].id, server_id) == 0)
            return &fossil_servers[i];
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// Internal: allocate a new server slot
// ---------------------------------------------------------------------------
static fossil_network_server_t *fossil_network_server_alloc(const char *server_id) {
    for (size_t i = 0; i < FOSSIL_NETWORK_SERVER_MAX; ++i) {
        if (!fossil_servers[i].active) {
            memset(&fossil_servers[i], 0, sizeof(fossil_network_server_t));
            strncpy(fossil_servers[i].id, server_id, sizeof(fossil_servers[i].id) - 1);
            return &fossil_servers[i];
        }
    }
    return NULL;
}

// ============================================================================
// Listen
// ============================================================================
int fossil_network_server_listen(const char *proto_id, const char *address, uint16_t port) {
    if (!proto_id || !address)
        return -1;

    char server_id[64];
    snprintf(server_id, sizeof(server_id), "%s:%u", address, port);

    fossil_network_server_t *server = fossil_network_server_alloc(server_id);
    if (!server)
        return -1;

    fossil_protocol_t proto = fossil_network_socket_proto_from_name(proto_id);
    if (proto == FOSSIL_PROTO_UNKNOWN)
        return -1;

    if (fossil_network_socket_create(&server->socket, AF_INET, proto) != 0)
        return -1;

    // Allow address reuse
    fossil_network_socket_set_option(&server->socket, SOL_SOCKET, SO_REUSEADDR, 1);

    if (fossil_network_socket_bind(&server->socket, address, port) != 0) {
        fossil_network_socket_close(&server->socket);
        return -1;
    }

    if (proto == FOSSIL_PROTO_TCP || proto == FOSSIL_PROTO_HTTP) {
        if (fossil_network_socket_listen(&server->socket, 8) != 0) {
            fossil_network_socket_close(&server->socket);
            return -1;
        }
    }

    server->active = 1;
    return 0;
}

// ============================================================================
// Accept
// ============================================================================
int fossil_network_server_accept(const char *server_id) {
    if (!server_id)
        return -1;

    fossil_network_server_t *server = fossil_network_server_find(server_id);
    if (!server || !server->active)
        return -1;

    if (server->client_count >= 16)
        return -1; // no space for new clients

    fossil_network_socket_t *client_sock = &server->clients[server->client_count];
    if (fossil_network_socket_accept(&server->socket, client_sock) != 0)
        return -1;

    server->client_count++;
    return (int)(server->client_count - 1); // return client index
}

// ============================================================================
// Broadcast
// ============================================================================
int fossil_network_server_broadcast(const char *server_id, const void *data, size_t len) {
    if (!server_id || !data || len == 0)
        return -1;

    fossil_network_server_t *server = fossil_network_server_find(server_id);
    if (!server || !server->active)
        return -1;

    size_t sent_count = 0;
    for (size_t i = 0; i < server->client_count; ++i) {
        ssize_t sent = fossil_network_socket_send(&server->clients[i], data, len);
        if (sent >= 0)
            sent_count++;
    }
    return (int)sent_count; // number of clients successfully sent to
}

// ============================================================================
// Close
// ============================================================================
int fossil_network_server_close(const char *server_id) {
    if (!server_id)
        return -1;

    fossil_network_server_t *server = fossil_network_server_find(server_id);
    if (!server)
        return -1;

    // Close all client sockets
    for (size_t i = 0; i < server->client_count; ++i)
        fossil_network_socket_close(&server->clients[i]);

    fossil_network_socket_close(&server->socket);
    server->active = 0;
    server->client_count = 0;
    memset(server->id, 0, sizeof(server->id));
    return 0;
}
