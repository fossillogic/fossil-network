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
#include "fossil/network/socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// Fossil Network Client
// Simplified client-side abstraction built on fossil_network_socket_*
// ============================================================================

typedef struct fossil_network_client_t {
    char id[64];
    fossil_network_socket_t socket;
    int connected;
} fossil_network_client_t;

// ============================================================================
// Simple client registry (static, expandable later)
// ============================================================================
#define FOSSIL_NETWORK_CLIENT_MAX 32
static fossil_network_client_t fossil_clients[FOSSIL_NETWORK_CLIENT_MAX];

// ---------------------------------------------------------------------------
// Internal: find a client by string ID
// ---------------------------------------------------------------------------
static fossil_network_client_t *fossil_network_client_find(const char *client_id) {
    if (!client_id) return NULL;
    for (size_t i = 0; i < FOSSIL_NETWORK_CLIENT_MAX; ++i) {
        if (fossil_clients[i].connected &&
            strcmp(fossil_clients[i].id, client_id) == 0)
            return &fossil_clients[i];
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// Internal: allocate a new client entry
// ---------------------------------------------------------------------------
static fossil_network_client_t *fossil_network_client_alloc(const char *client_id) {
    for (size_t i = 0; i < FOSSIL_NETWORK_CLIENT_MAX; ++i) {
        if (!fossil_clients[i].connected) {
            memset(&fossil_clients[i], 0, sizeof(fossil_network_client_t));
            strncpy(fossil_clients[i].id, client_id, sizeof(fossil_clients[i].id) - 1);
            fossil_clients[i].id[sizeof(fossil_clients[i].id) - 1] = '\0';
            return &fossil_clients[i];
        }
    }
    return NULL;
}

// ============================================================================
// Connect
// ============================================================================
int fossil_network_client_connect(const char *proto_id, const char *host, uint16_t port) {
    if (!proto_id || !host)
        return -1;

    // Derive client ID from host:port
    char client_id[64];
    snprintf(client_id, sizeof(client_id), "%s:%u", host, port);

    fossil_network_client_t *client = fossil_network_client_alloc(client_id);
    if (!client)
        return -1;

    fossil_protocol_t proto = fossil_network_socket_proto_from_name(proto_id);
    if (proto == FOSSIL_PROTO_UNKNOWN)
        return -1;

    if (fossil_network_socket_create(&client->socket, AF_INET, proto) != 0)
        return -1;

    if (fossil_network_socket_connect(&client->socket, host, port) != 0) {
        fossil_network_socket_close(&client->socket);
        return -1;
    }

    client->connected = 1;
    return 0;
}

// ============================================================================
// Send
// ============================================================================
int fossil_network_client_send(const char *client_id, const void *buf, size_t len) {
    if (!client_id || !buf || len == 0)
        return -1;

    fossil_network_client_t *client = fossil_network_client_find(client_id);
    if (!client || !client->connected)
        return -1;

    ssize_t sent = fossil_network_socket_send(&client->socket, buf, len);
    return (sent >= 0) ? (int)sent : -1;
}

// ============================================================================
// Receive
// ============================================================================
int fossil_network_client_recv(const char *client_id, void *buf, size_t len) {
    if (!client_id || !buf || len == 0)
        return -1;

    fossil_network_client_t *client = fossil_network_client_find(client_id);
    if (!client || !client->connected)
        return -1;

    ssize_t recvd = fossil_network_socket_recv(&client->socket, buf, len);
    return (recvd >= 0) ? (int)recvd : -1;
}

// ============================================================================
// Disconnect
// ============================================================================
int fossil_network_client_disconnect(const char *client_id) {
    if (!client_id)
        return -1;

    fossil_network_client_t *client = fossil_network_client_find(client_id);
    if (!client)
        return -1;

    fossil_network_socket_close(&client->socket);
    client->connected = 0;
    memset(client->id, 0, sizeof(client->id));
    return 0;
}
