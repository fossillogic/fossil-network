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

    g_nodes[g_node_count] = *node;
    g_nodes[g_node_count].is_active = 1; // Mark node as active when added
    g_node_count++;
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
        // Only add valid seed nodes (non-empty node_id, address, and port)
        if (seeds[i].node_id[0] != '\0' &&
            seeds[i].address[0] != '\0' &&
            seeds[i].port != 0) {
            fossil_cluster_add_node(&seeds[i]);
            printf("[cluster] Discovered seed node: %s (%s:%u) Metadata: %s\n",
                   seeds[i].node_id, seeds[i].address, seeds[i].port, seeds[i].metadata);
        }
    }

    return 0;
}

int fossil_network_cluster_broadcast(const void *buf, size_t len) {
    if (!buf || len == 0) return -1;

    for (int i = 0; i < g_node_count; i++) {
        // Skip self and inactive nodes
        if (strcmp(g_nodes[i].node_id, g_self.node_id) == 0 || !g_nodes[i].is_active) continue;

        fossil_network_socket_t sock;
        if (fossil_network_socket_create(&sock, AF_INET, FOSSIL_PROTO_TCP) != 0) {
            continue;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(g_nodes[i].port);
        if (inet_pton(AF_INET, g_nodes[i].address, &addr.sin_addr) <= 0) {
            fossil_network_socket_close(&sock);
            continue;
        }

        ssize_t sent = sendto(sock.fd, (const char*)buf, (int)len, 0,
                              (struct sockaddr*)&addr, sizeof(addr));
        if (sent >= 0) broadcasted++;

        fossil_network_socket_close(&sock);
    }

    // If there were no other nodes, consider broadcast successful
    return 0;
}

int fossil_network_cluster_leave(fossil_network_cluster_node_t *self) {
    if (!self) return -1;

    // Mark self as inactive in node list
    for (int i = 0; i < g_node_count; i++) {
        if (strcmp(g_nodes[i].node_id, self->node_id) == 0) {
            g_nodes[i].is_active = 0;
            printf("[cluster] Node left: %s\n", self->node_id);
            break;
        }
    }

    return 0;
}

int fossil_network_cluster_heartbeat(fossil_network_cluster_node_t *self) {
    if (!self) return -1;

    // Update last heartbeat timestamp in global node list
    for (int i = 0; i < g_node_count; i++) {
        if (strcmp(g_nodes[i].node_id, self->node_id) == 0) {
            g_nodes[i].last_heartbeat = (uint64_t)time(NULL);
            break;
        }
    }

    // In a real implementation, this would notify other nodes
    printf("[cluster] Heartbeat sent from node: %s\n", self->node_id);
    return 0;
}

int fossil_network_cluster_get_active_nodes(fossil_network_cluster_node_t *nodes, int max_nodes) {
    if (!nodes || max_nodes <= 0) return 0;

    int count = (g_node_count < max_nodes) ? g_node_count : max_nodes;
    for (int i = 0; i < count; i++) {
        nodes[i] = g_nodes[i];
    }
    return count;
}
