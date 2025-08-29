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

FOSSIL_TEST_SUITE(c_udp_fixture);

FOSSIL_SETUP(c_udp_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_udp_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_udp_test_udp_bind_success) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_init();
    ASSUME_ITS_TRUE(rc == 0);

    // Bind to any address, ephemeral port
    rc = fossil_network_udp_bind(&sock, NULL, 0);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_socket_cleanup();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_udp_test_udp_bind_invalid_sock) {
    int rc = fossil_network_udp_bind(NULL, "127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST_CASE(c_udp_test_udp_set_broadcast_enable_disable) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_init();
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_udp_bind(&sock, NULL, 0);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_udp_set_broadcast(&sock, 1);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_udp_set_broadcast(&sock, 0);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_socket_cleanup();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_udp_test_udp_set_broadcast_invalid_sock) {
    int rc = fossil_network_udp_set_broadcast(NULL, 1);
    ASSUME_ITS_TRUE(rc != 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_udp_tests) {
    FOSSIL_TEST_ADD(c_udp_fixture, c_udp_test_udp_bind_success);
    FOSSIL_TEST_ADD(c_udp_fixture, c_udp_test_udp_bind_invalid_sock);
    FOSSIL_TEST_ADD(c_udp_fixture, c_udp_test_udp_set_broadcast_enable_disable);
    FOSSIL_TEST_ADD(c_udp_fixture, c_udp_test_udp_set_broadcast_invalid_sock);

    FOSSIL_TEST_REGISTER(c_udp_fixture);
} // end of tests
