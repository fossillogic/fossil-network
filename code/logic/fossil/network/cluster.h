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

#ifdef __cplusplus
}

namespace fossil {

namespace network {

    class Cluster {
    public:
        /**
         * @brief Join a cluster of nodes.
         *
         * @param self Node info for this instance.
         * @param seeds Array of known seed nodes.
         * @param count Number of seeds.
         * @return 0 on success, non-zero on failure.
         */
        static int join(fossil_network_cluster_node_t *self,
                        const fossil_network_cluster_node_t *seeds,
                        int count) {
            return fossil_network_cluster_join(self, seeds, count);
        }

        /**
         * @brief Broadcast a message to all nodes.
         */
        static int broadcast(const void *buf, size_t len) {
            return fossil_network_cluster_broadcast(buf, len);
        }
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_CLUSTER_H */
