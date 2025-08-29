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

FOSSIL_TEST_CASE(c_cluster_test_join_with_valid_self_and_seeds) {
    fossil_network_cluster_node_t self = { "node1", "127.0.0.1", 9001 };
    fossil_network_cluster_node_t seeds[2] = {
        { "node2", "127.0.0.2", 9002 },
        { "node3", "127.0.0.3", 9003 }
    };
    int result = fossil_network_cluster_join(&self, seeds, 2);
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST_CASE(c_cluster_test_join_with_null_self) {
    fossil_network_cluster_node_t seeds[1] = {
        { "node2", "127.0.0.2", 9002 }
    };
    int result = fossil_network_cluster_join(NULL, seeds, 1);
    ASSUME_ITS_TRUE(result != 0);
}

FOSSIL_TEST_CASE(c_cluster_test_join_with_zero_seeds) {
    fossil_network_cluster_node_t self = { "node1", "127.0.0.1", 9001 };
    int result = fossil_network_cluster_join(&self, NULL, 0);
    ASSUME_ITS_TRUE(result == 0);
}

// FOSSIL_TEST_CASE(c_cluster_test_broadcast_with_valid_buffer) {
//     fossil_network_cluster_node_t self = { "node1", "127.0.0.1", 9001 };
//     fossil_network_cluster_node_t seeds[1] = {
//         { "node2", "127.0.0.2", 9002 }
//     };
//     int join_result = fossil_network_cluster_join(&self, seeds, 1);
//     ASSUME_ITS_TRUE(join_result == 0);

//     const char msg[] = "hello cluster";
//     int result = fossil_network_cluster_broadcast(msg, sizeof(msg));
//     ASSUME_ITS_TRUE(result == 0);
// }

FOSSIL_TEST_CASE(c_cluster_test_broadcast_with_null_buffer) {
    fossil_network_cluster_node_t self = { "node1", "127.0.0.1", 9001 };
    fossil_network_cluster_node_t seeds[1] = {
        { "node2", "127.0.0.2", 9002 }
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    int result = fossil_network_cluster_broadcast(NULL, 10);
    ASSUME_ITS_TRUE(result != 0);
}

FOSSIL_TEST_CASE(c_cluster_test_broadcast_with_zero_length) {
    fossil_network_cluster_node_t self = { "node1", "127.0.0.1", 9001 };
    fossil_network_cluster_node_t seeds[1] = {
        { "node2", "127.0.0.2", 9002 }
    };
    int join_result = fossil_network_cluster_join(&self, seeds, 1);
    ASSUME_ITS_TRUE(join_result == 0);

    char dummy[4] = "abc";
    int result = fossil_network_cluster_broadcast(dummy, 0);
    ASSUME_ITS_TRUE(result != 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_cluster_tests) {
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_valid_self_and_seeds);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_null_self);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_join_with_zero_seeds);
    // FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_broadcast_with_valid_buffer);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_broadcast_with_null_buffer);
    FOSSIL_TEST_ADD(c_cluster_fixture, c_cluster_test_broadcast_with_zero_length);

    FOSSIL_TEST_REGISTER(c_cluster_fixture);
} // end of tests
