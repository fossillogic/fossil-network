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
#include "fossil/network/cluster.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// *****************************************************************************
// Internal cluster state
// *****************************************************************************
#define FOSSIL_CLUSTER_MAX_NODES 32

static fossil_network_cluster_node_t g_nodes[FOSSIL_CLUSTER_MAX_NODES];
static int g_node_count = 0;
static fossil_network_cluster_node_t g_self;

// *****************************************************************************
// Helpers
// *****************************************************************************
static int fossil_cluster_add_node(const fossil_network_cluster_node_t *node) {
    if (g_node_count >= FOSSIL_CLUSTER_MAX_NODES) return -1;

    // Prevent duplicates by node_id
    for (int i = 0; i < g_node_count; i++) {
        if (strcmp(g_nodes[i].node_id, node->node_id) == 0) {
            return 0; // already present
        }
    }

    g_nodes[g_node_count++] = *node;
    return 0;
}

// *****************************************************************************
// Public API
// *****************************************************************************
int fossil_network_cluster_join(fossil_network_cluster_node_t *self,
                                const fossil_network_cluster_node_t *seeds,
                                int count) {
    if (!self) return -1;

    // Save self
    g_self = *self;
    fossil_cluster_add_node(self);

    // Attempt to add seeds
    for (int i = 0; i < count; i++) {
        fossil_cluster_add_node(&seeds[i]);
        // In a real system we’d connect and exchange membership info here
        printf("[cluster] Discovered seed node: %s (%s:%u)\n",
               seeds[i].node_id, seeds[i].address, seeds[i].port);
    }

    return 0;
}

int fossil_network_cluster_broadcast(const void *buf, size_t len) {
    if (!buf || len == 0) return -1;

    for (int i = 0; i < g_node_count; i++) {
        // Skip self
        if (strcmp(g_nodes[i].node_id, g_self.node_id) == 0) continue;

        fossil_network_socket_t sock;
        if (fossil_network_socket_create(&sock, AF_INET, SOCK_DGRAM, 0) != 0) {
            continue;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(g_nodes[i].port);
        inet_pton(AF_INET, g_nodes[i].address, &addr.sin_addr);

        sendto(sock.fd, (const char*)buf, (int)len, 0,
               (struct sockaddr*)&addr, sizeof(addr));

        fossil_network_socket_close(&sock);
    }

    return 0;
}
