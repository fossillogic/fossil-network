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

FOSSIL_TEST_SUITE(c_network_fixture);

FOSSIL_SETUP(c_network_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_network_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_network_test_socket_init_cleanup) {
    int rc = fossil_network_socket_init();
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_cleanup();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_network_test_proto_name_conversion) {
    fossil_protocol_t proto = fossil_network_socket_proto_from_name("tcp");
    ASSUME_ITS_TRUE(proto != FOSSIL_PROTO_UNKNOWN);
    const char *name = fossil_network_socket_proto_to_name(proto);
    ASSUME_ITS_TRUE(name != NULL);
}

FOSSIL_TEST_CASE(c_network_test_socket_create_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_network_test_socket_bind_listen_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_bind(&sock, "127.0.0.1", 0); // bind to any port
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_listen(&sock, 1);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_network_test_socket_open_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_open(&sock, "tcp", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_network_tests) {
    FOSSIL_TEST_ADD(c_network_fixture, c_network_test_socket_init_cleanup);
    FOSSIL_TEST_ADD(c_network_fixture, c_network_test_proto_name_conversion);
    FOSSIL_TEST_ADD(c_network_fixture, c_network_test_socket_create_close);
    FOSSIL_TEST_ADD(c_network_fixture, c_network_test_socket_bind_listen_close);
    FOSSIL_TEST_ADD(c_network_fixture, c_network_test_socket_open_close);

    FOSSIL_TEST_REGISTER(c_network_fixture);
} // end of tests
