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
#include "fossil/network/framework.h"
#include <fossil/pizza/framework.h>


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_SUITE(c_cluster_fixture);

FOSSIL_SETUP(c_cluster_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_cluster_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Helper for Node Initialization
// * * * * * * * * * * * * * * * * * * * * * * * *

static inline fossil_network_cluster_node_t
make_node(const char *id, const char *addr, uint16_t port, int active) {
    fossil_network_cluster_node_t node;
    memset(&node, 0, sizeof(node));
    strncpy(node.node_id, id, sizeof(node.node_id) - 1);
    strncpy(node.address, addr, sizeof(node.address) - 1);
    node.port = port;
    node.is_active = active;
    node.last_heartbeat = (uint64_t)time(NULL);
    snprintf(node.metadata, sizeof(node.metadata),
             "Node:%s Address:%s Port:%u", id, addr, port);
    return node;
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_cluster_test_join_with_valid_self_and_seeds) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[2] = {
        make_node("node2", "127.0.0.2", 9002, 1),
        make_node("node3", "127.0.0.3", 9003, 1)
    };
    int result = fossil_network_cluster_join(&self, seeds, 2);
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_join_with_null_self) {
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int result = fossil_network_cluster_join(NULL, seeds, 1);
    ASSUME_ITS_TRUE(result == -1);
}

FOSSIL_TEST_CASE(c_cluster_test_join_with_zero_seeds) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    int result = fossil_network_cluster_join(&self, NULL, 0);
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_broadcast_with_null_buffer) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    int result = fossil_network_cluster_broadcast(NULL, 10);
    ASSUME_ITS_TRUE(result == -1);
}

FOSSIL_TEST_CASE(c_cluster_test_broadcast_with_zero_length) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    char dummy[4] = "abc";
    int result = fossil_network_cluster_broadcast(dummy, 0);
    ASSUME_ITS_TRUE(result == -1);
}

FOSSIL_TEST_CASE(c_cluster_test_leave_with_valid_self) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    int leave_result = fossil_network_cluster_leave(&self);
    ASSUME_ITS_TRUE(leave_result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_leave_with_null_self) {
    int result = fossil_network_cluster_leave(NULL);
    ASSUME_ITS_TRUE(result == -1);
}

FOSSIL_TEST_CASE(c_cluster_test_heartbeat_with_valid_self) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    int heartbeat_result = fossil_network_cluster_heartbeat(&self);
    ASSUME_ITS_TRUE(heartbeat_result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_heartbeat_with_null_self) {
    int result = fossil_network_cluster_heartbeat(NULL);
    ASSUME_ITS_TRUE(result == -1);
}

FOSSIL_TEST_CASE(c_cluster_test_get_active_nodes_with_valid_nodes) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[2] = {
        make_node("node2", "127.0.0.2", 9002, 1),
        make_node("node3", "127.0.0.3", 9003, 0)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 2);
    ASSUME_ITS_TRUE(join_result == 0);

    fossil_network_cluster_node_t nodes[4];
    int active_count = fossil_network_cluster_get_active_nodes(nodes, 4);
    ASSUME_ITS_TRUE(active_count >= 1);
}

FOSSIL_TEST_CASE(c_cluster_test_get_active_nodes_with_null_nodes) {
    int result = fossil_network_cluster_get_active_nodes(NULL, 4);
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_get_active_nodes_with_zero_max_nodes) {
    fossil_network_cluster_node_t self = make_node("node1", "127.0.0.1", 9001, 1);
    fossil_network_cluster_node_t seeds[1] = {
        make_node("node2", "127.0.0.2", 9002, 1)
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    fossil_network_cluster_node_t nodes[2];
    int result = fossil_network_cluster_get_active_nodes(nodes, 0);
    ASSUME_ITS_TRUE(result == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_cluster_tests) {
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_valid_self_and_seeds);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_null_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_zero_seeds);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_broadcast_with_null_buffer);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_broadcast_with_zero_length);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_leave_with_valid_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_leave_with_null_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_heartbeat_with_valid_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_heartbeat_with_null_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_get_active_nodes_with_valid_nodes);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_get_active_nodes_with_null_nodes);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_get_active_nodes_with_zero_max_nodes);

    FOSSIL_TEST_REGISTER(c_cluster_fixture);
}
