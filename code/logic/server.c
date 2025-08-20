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
#include <stdlib.h>
#include <string.h>

struct fossil_network_server {
    fossil_network_socket_t sock;
    fossil_protocol_t proto;
    char *host;
    uint16_t port;
};

fossil_network_server_t *
fossil_network_server_create(const char *host,
                             const char *service,
                             fossil_protocol_t proto) {
    fossil_network_server_t *srv = calloc(1, sizeof(*srv));
    if (!srv) return NULL;

    srv->proto = proto;
    srv->host = strdup(host ? host : "");
    srv->port = (uint16_t)atoi(service ? service : "0");

    if (fossil_network_socket_create(&srv->sock, AF_INET, proto) != 0) {
        free(srv->host);
        free(srv);
        return NULL;
    }
    return srv;
}

int fossil_network_server_listen(fossil_network_server_t *srv, int backlog) {
    if (!srv) return -1;
    if (fossil_network_socket_bind(&srv->sock, srv->host, srv->port) != 0)
        return -1;
    return fossil_network_socket_listen(&srv->sock, backlog);
}

fossil_network_socket_t *
fossil_network_server_accept(fossil_network_server_t *srv) {
    if (!srv) return NULL;
    fossil_network_socket_t *client = calloc(1, sizeof(*client));
    if (!client) return NULL;
    if (fossil_network_socket_accept(&srv->sock, client) != 0) {
        free(client);
        return NULL;
    }
    return client;
}

ssize_t fossil_network_server_send(fossil_network_socket_t *client,
                                   const void *buf,
                                   size_t len,
                                   int flags) {
    if (!client) return -1;
    (void)flags;
    return fossil_network_socket_send(client, buf, len);
}

ssize_t fossil_network_server_recv(fossil_network_socket_t *client,
                                   void *buf,
                                   size_t len,
                                   int flags) {
    if (!client) return -1;
    (void)flags;
    return fossil_network_socket_recv(client, buf, len);
}

void fossil_network_server_destroy(fossil_network_server_t *srv) {
    if (!srv) return;
    fossil_network_socket_close(&srv->sock);
    free(srv->host);
    free(srv);
}
