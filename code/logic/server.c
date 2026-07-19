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
#include "fossil/network/server.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

struct fossil_net_server {
    fossil_net_socket_t sock;
    fossil_net_address_t addr;
};

fossil_net_server_t *fossil_net_server_create(
    const char *type,
    const char *family,
    const char *addr,
    uint16_t port)
{
    fossil_net_server_t *server = calloc(1, sizeof(fossil_net_server_t));
    if (!server ||
        ((strcmp(family, "ipv4") != 0) && (strcmp(family, "ipv6") != 0)) ||
        ((strcmp(type, "tcp") != 0) && (strcmp(type, "udp") != 0) && (strcmp(type, "raw") != 0)))
    {
        free(server);
        return NULL;
    }

    // Initialize address structure
    memset(&server->addr, 0, sizeof(fossil_net_address_t));
    strncpy(server->addr.family, family, sizeof(server->addr.family) - 1);

    // If addr is NULL, bind to any address (INADDR_ANY or in6addr_any)
    if (addr == NULL) {
        if (strcmp(family, "ipv4") == 0) {
            strncpy(server->addr.addr, "0.0.0.0", sizeof(server->addr.addr) - 1);
        } else if (strcmp(family, "ipv6") == 0) {
            strncpy(server->addr.addr, "::", sizeof(server->addr.addr) - 1);
        } else {
            // Unknown family, fallback to empty string
            server->addr.ip[0] = '\0';
        }
        server->addr.port = port;
    } else {
        if (fossil_net_socket_address_parse(&server->addr, addr, port) != 0) {
            free(server);
            return NULL;
        }
    }

    // Create socket with type/family string IDs
    if (fossil_net_socket_create(&server->sock, type, family) != 0) {
        free(server);
        return NULL;
    }

    // Bind socket to address
    if (fossil_net_socket_bind(&server->sock, &server->addr) != 0) {
        fossil_net_socket_close(&server->sock);
        free(server);
        return NULL;
    }

    return server;
}

void fossil_net_server_destroy(fossil_net_server_t *server)
{
    if (!server)
        return;
    fossil_net_socket_close(&server->sock);
    free(server);
}

int fossil_net_server_listen(
    fossil_net_server_t *server,
    int backlog)
{
    if (!server)
        return -1;
    return fossil_net_socket_listen(&server->sock, backlog);
}

int fossil_net_server_accept(
    fossil_net_server_t *server,
    fossil_net_socket_t *client_sock,
    fossil_net_address_t *client_addr)
{
    if (!server || !client_sock)
        return -1;
    return fossil_net_socket_accept(&server->sock, client_sock, client_addr);
}

int fossil_net_server_get_address(
    fossil_net_server_t *server,
    fossil_net_address_t *addr)
{
    if (!server || !addr)
        return -1;
    memcpy(addr, &server->addr, sizeof(fossil_net_address_t));
    return 0;
}

int fossil_net_server_set_blocking(
    fossil_net_server_t *server,
    bool blocking)
{
    if (!server)
        return -1;
    return fossil_net_socket_set_blocking(&server->sock, blocking);
}
