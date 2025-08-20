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
#include <stdlib.h>
#include <string.h>

struct fossil_network_client {
    fossil_network_socket_t sock;
    fossil_protocol_t proto;
    char *host;
    uint16_t port;
};

fossil_network_client_t *
fossil_network_client_create(const char *host,
                             const char *service,
                             fossil_protocol_t proto) {
    fossil_network_client_t *cli = calloc(1, sizeof(*cli));
    if (!cli) return NULL;

    cli->proto = proto;
    cli->host = strdup(host ? host : "");
    cli->port = (uint16_t)atoi(service ? service : "0");

    if (fossil_network_socket_create(&cli->sock, AF_INET, proto) != 0) {
        free(cli->host);
        free(cli);
        return NULL;
    }
    return cli;
}

int fossil_network_client_connect(fossil_network_client_t *cli) {
    if (!cli) return -1;
    return fossil_network_socket_connect(&cli->sock, cli->host, cli->port);
}

ssize_t fossil_network_client_send(fossil_network_client_t *cli,
                                   const void *buf,
                                   size_t len,
                                   int flags) {
    if (!cli) return -1;
    (void)flags; // not yet used
    return fossil_network_socket_send(&cli->sock, buf, len);
}

ssize_t fossil_network_client_recv(fossil_network_client_t *cli,
                                   void *buf,
                                   size_t len,
                                   int flags) {
    if (!cli) return -1;
    (void)flags; // not yet used
    return fossil_network_socket_recv(&cli->sock, buf, len);
}

void fossil_network_client_destroy(fossil_network_client_t *cli) {
    if (!cli) return;
    fossil_network_socket_close(&cli->sock);
    free(cli->host);
    free(cli);
}
