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
#include "fossil/network/client.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct fossil_net_client {
    fossil_net_socket_t sock;
    int last_error;
    bool connected;
    fossil_net_address_t local_addr;
    fossil_net_address_t remote_addr;
} fossil_net_client_t;

fossil_net_client_t *fossil_net_client_create(const char *type, const char *family) {
    fossil_net_client_t *client = calloc(1, sizeof(fossil_net_client_t));
    if (!client ||
        ((strcmp(family, "ipv4") != 0) && (strcmp(family, "ipv6") != 0)) ||
        ((strcmp(type, "tcp") != 0) && (strcmp(type, "udp") != 0) && (strcmp(type, "raw") != 0))) {
        free(client);
        return NULL;
    }
    if (fossil_net_socket_create(&client->sock, type, family) != 0) {
        free(client);
        return NULL;
    }
    client->connected = false;
    client->last_error = 0;
    return client;
}

void fossil_net_client_destroy(fossil_net_client_t *client) {
    if (!client) return;
    fossil_net_socket_close(&client->sock);
    free(client);
}

int fossil_net_client_connect(fossil_net_client_t *client, const fossil_net_address_t *addr) {
    if (!client || !addr) return -1;
    int ret = fossil_net_socket_connect(&client->sock, addr);
    if (ret != 0) {
        client->last_error = fossil_net_socket_error_last();
        return -1;
    }
    client->connected = true;
    memcpy(&client->remote_addr, addr, sizeof(fossil_net_address_t));
    if (fossil_net_socket_get_local_address(&client->sock, &client->local_addr) != 0) {
        // Not fatal, but record error
        client->last_error = fossil_net_socket_error_last();
    }
    return 0;
}

int fossil_net_client_disconnect(fossil_net_client_t *client) {
    if (!client) return -1;
    int ret = fossil_net_socket_close(&client->sock);
    if (ret == 0) {
        client->connected = false;
    }
    return ret;
}

int fossil_net_client_send(fossil_net_client_t *client, const void *data, uint32_t size, uint32_t *sent) {
    if (!client || !data) return -1;
    int ret = fossil_net_socket_send(&client->sock, data, size, sent);
    if (ret != 0) {
        client->last_error = fossil_net_socket_error_last();
        if (sent) *sent = 0;
        return -1;
    }
    return 0;
}

int fossil_net_client_receive(fossil_net_client_t *client, void *buffer, uint32_t size, uint32_t *received) {
    if (!client || !buffer) return -1;
    int ret = fossil_net_socket_receive(&client->sock, buffer, size, received);
    if (ret != 0) {
        client->last_error = fossil_net_socket_error_last();
        if (received) *received = 0;
        return -1;
    }
    return 0;
}

int fossil_net_client_get_local_address(fossil_net_client_t *client, fossil_net_address_t *addr) {
    if (!client || !addr) return -1;
    return fossil_net_socket_get_local_address(&client->sock, addr);
}

int fossil_net_client_get_remote_address(fossil_net_client_t *client, fossil_net_address_t *addr) {
    if (!client || !addr) return -1;
    memcpy(addr, &client->remote_addr, sizeof(fossil_net_address_t));
    return 0;
}

int fossil_net_client_set_blocking(fossil_net_client_t *client, bool blocking) {
    if (!client) return -1;
    int ret = fossil_net_socket_set_blocking(&client->sock, blocking);
    if (ret != 0) {
        client->last_error = fossil_net_socket_error_last();
        return -1;
    }
    client->sock.blocking = blocking;
    return 0;
}

int fossil_net_client_error_last(fossil_net_client_t *client) {
    if (!client) return 0;
    return client->last_error;
}

const char *fossil_net_client_error_string(int err) {
    return fossil_net_socket_error_string(err);
}
