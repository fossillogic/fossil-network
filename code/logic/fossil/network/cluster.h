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
#ifndef FOSSIL_NETWORK_CLUSTER_H
#define FOSSIL_NETWORK_CLUSTER_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    char node_id[64];
    char address[64];
    uint16_t port;
    int is_active;                // 1 if node is active, 0 otherwise
    uint64_t last_heartbeat;      // Timestamp of last heartbeat
    char metadata[128];           // Optional metadata for the node
} fossil_network_cluster_node_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Join a cluster of nodes.
 *
 * @param self Node info for this instance.
 * @param seeds Array of known seed nodes.
 * @param count Number of seeds.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_cluster_join(fossil_network_cluster_node_t *self,
                                const fossil_network_cluster_node_t *seeds,
                                int count);

/**
 * @brief Broadcast a message to all nodes.
 */
int fossil_network_cluster_broadcast(const void *buf, size_t len);

/**
 * @brief Leave the cluster.
 *
 * @param self Node info for this instance.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_cluster_leave(fossil_network_cluster_node_t *self);

/**
 * @brief Send a heartbeat to the cluster.
 *
 * @param self Node info for this instance.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_cluster_heartbeat(fossil_network_cluster_node_t *self);

/**
 * @brief Get the list of active nodes in the cluster.
 *
 * @param nodes Output array for active nodes.
 * @param max_nodes Maximum number of nodes to retrieve.
 * @return Number of active nodes found.
 */
int fossil_network_cluster_get_active_nodes(fossil_network_cluster_node_t *nodes, int max_nodes);

#ifdef __cplusplus
}

namespace fossil {

    namespace network {

        /**
         * @class Cluster
         * @brief Provides a C++ interface for cluster management operations.
         *
         * The Cluster class wraps the C cluster management functions, offering
         * static methods for joining, broadcasting, leaving, sending heartbeats,
         * and retrieving active nodes in a cluster.
         */
        class Cluster {
        public:
            /**
             * @brief Join a cluster of nodes.
             *
             * Attempts to join the cluster using the provided node information and
             * a list of known seed nodes.
             *
             * @param self Pointer to the node information for this instance.
             * @param seeds Array of known seed nodes.
             * @param count Number of seed nodes in the array.
             * @return 0 on success, non-zero on failure.
             */
            static int join(fossil_network_cluster_node_t *self,
                            const fossil_network_cluster_node_t *seeds,
                            int count) {
                return fossil_network_cluster_join(self, seeds, count);
            }

            /**
             * @brief Broadcast a message to all nodes in the cluster.
             *
             * Sends the specified buffer to all nodes currently in the cluster.
             *
             * @param buf Pointer to the message buffer.
             * @param len Length of the message buffer in bytes.
             * @return 0 on success, non-zero on failure.
             */
            static int broadcast(const void *buf, size_t len) {
                return fossil_network_cluster_broadcast(buf, len);
            }

            /**
             * @brief Leave the cluster.
             *
             * Removes the node from the cluster, updating cluster membership.
             *
             * @param self Pointer to the node information for this instance.
             * @return 0 on success, non-zero on failure.
             */
            static int leave(fossil_network_cluster_node_t *self) {
                return fossil_network_cluster_leave(self);
            }

            /**
             * @brief Send a heartbeat to the cluster.
             *
             * Notifies the cluster of this node's continued presence and activity.
             *
             * @param self Pointer to the node information for this instance.
             * @return 0 on success, non-zero on failure.
             */
            static int heartbeat(fossil_network_cluster_node_t *self) {
                return fossil_network_cluster_heartbeat(self);
            }

            /**
             * @brief Get the list of active nodes in the cluster.
             *
             * Retrieves information about currently active nodes in the cluster.
             *
             * @param nodes Output array for active node information.
             * @param max_nodes Maximum number of nodes to retrieve.
             * @return Number of active nodes found.
             */
            static int get_active_nodes(fossil_network_cluster_node_t *nodes, int max_nodes) {
                return fossil_network_cluster_get_active_nodes(nodes, max_nodes);
            }
        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_CLUSTER_H */
