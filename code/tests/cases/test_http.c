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

FOSSIL_TEST_SUITE(c_http_fixture);

FOSSIL_SETUP(c_http_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_http_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_http_test_simple_get_request) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_open(&sock, "tcp", "127.0.0.1", 8080);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_http_get(&sock, "/");
    ASSUME_ITS_TRUE(rc == 0);

    fossil_network_http_response_t resp;
    rc = fossil_network_http_read_response(&sock, &resp);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_http_tests) {
    FOSSIL_TEST_ADD(c_http_fixture, c_http_test_simple_get_request);

    FOSSIL_TEST_REGISTER(c_http_fixture);
} // end of tests
