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

FOSSIL_TEST_SUITE(c_dns_fixture);

FOSSIL_SETUP(c_dns_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_dns_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_dns_test_resolve_hostname) {
    char addrs[4][64];
    int count = fossil_network_dns_resolve("localhost", addrs, 4);
    ASSUME_ITS_TRUE(count > 0);
    for (int i = 0; i < count; ++i) {
        ASSUME_ITS_TRUE(addrs[i][0] != '\0');
    }
}

FOSSIL_TEST_CASE(c_dns_test_resolve_invalid_hostname) {
    char addrs[2][64];
    int count = fossil_network_dns_resolve("invalid.hostname.example", addrs, 2);
    ASSUME_ITS_TRUE(count == -1);
}

FOSSIL_TEST_CASE(c_dns_test_resolve_null_hostname) {
    char addrs[2][64];
    int count = fossil_network_dns_resolve(NULL, addrs, 2);
    ASSUME_ITS_TRUE(count == -1);
}

FOSSIL_TEST_CASE(c_dns_test_resolve_zero_max_addrs) {
    char addrs[2][64];
    int count = fossil_network_dns_resolve("localhost", addrs, 0);
    ASSUME_ITS_TRUE(count == -1);
}

FOSSIL_TEST_CASE(c_dns_test_resolve_null_addrs) {
    int count = fossil_network_dns_resolve("localhost", NULL, 2);
    ASSUME_ITS_TRUE(count == -1);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_dns_tests) {
    FOSSIL_TEST_ADD(c_dns_fixture, c_dns_test_resolve_hostname);
    FOSSIL_TEST_ADD(c_dns_fixture, c_dns_test_resolve_invalid_hostname);
    FOSSIL_TEST_ADD(c_dns_fixture, c_dns_test_resolve_null_hostname);
    FOSSIL_TEST_ADD(c_dns_fixture, c_dns_test_resolve_zero_max_addrs);
    FOSSIL_TEST_ADD(c_dns_fixture, c_dns_test_resolve_null_addrs);

    FOSSIL_TEST_REGISTER(c_dns_fixture);
} // end of tests
