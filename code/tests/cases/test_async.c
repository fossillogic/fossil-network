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

FOSSIL_TEST_SUITE(c_async_fixture);

FOSSIL_SETUP(c_async_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_async_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_async_test_create_and_free) {
    fossil_network_async_t *loop = fossil_network_async_create();
    ASSUME_ITS_TRUE(loop != NULL);
    fossil_network_async_free(loop);
}

FOSSIL_TEST_CASE(c_async_test_add_null_loop) {
    fossil_network_socket_t dummy_sock = {0};
    int ret = fossil_network_async_add(NULL, &dummy_sock, 1, NULL);
    ASSUME_ITS_TRUE(ret == -1);
}

FOSSIL_TEST_CASE(c_async_test_add_null_sock) {
    fossil_network_async_t *loop = fossil_network_async_create();
    int ret = fossil_network_async_add(loop, NULL, 1, NULL);
    ASSUME_ITS_TRUE(ret == -1);
    fossil_network_async_free(loop);
}

FOSSIL_TEST_CASE(c_async_test_run_null_loop) {
    int ret = fossil_network_async_run(NULL, 100);
    ASSUME_ITS_TRUE(ret == -1);
}

FOSSIL_TEST_CASE(c_async_test_run_empty_loop) {
    fossil_network_async_t *loop = fossil_network_async_create();
    int ret = fossil_network_async_run(loop, 100);
    ASSUME_ITS_TRUE(ret == -1);
    fossil_network_async_free(loop);
}

// Note: The following test is a stub, as it requires a valid socket.
// In a real test, you would create a socket, bind/listen, and add it.
FOSSIL_TEST_CASE(c_async_test_add_and_run_stub) {
    fossil_network_async_t *loop = fossil_network_async_create();
    fossil_network_socket_t dummy_sock = {0};
    int ret_add = fossil_network_async_add(loop, &dummy_sock, 1, (void*)0x1234);
    ASSUME_ITS_TRUE(ret_add == 0);
    // This will likely return 0 or -1 since dummy_sock is not a real socket.
    int ret_run = fossil_network_async_run(loop, 10);
    ASSUME_ITS_TRUE(ret_run <= 0);
    fossil_network_async_free(loop);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_async_tests) {
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_add_and_run_stub);
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_create_and_free);
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_add_null_loop);
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_add_null_sock);
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_run_null_loop);
    FOSSIL_TEST_ADD(c_async_fixture, c_async_test_run_empty_loop);

    FOSSIL_TEST_REGISTER(c_async_fixture);
} // end of tests
