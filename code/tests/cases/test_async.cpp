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

FOSSIL_TEST_SUITE(cpp_asyncpp_fixture);

FOSSIL_SETUP(cpp_asyncpp_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_asyncpp_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_asyncpp_test_create_and_free) {
    auto *loop = fossil::network::Async::create();
    ASSUME_ITS_TRUE(loop != nullptr);
    fossil::network::Async::free(loop);
}

FOSSIL_TEST_CASE(cpp_asyncpp_test_add_null_sock) {
    auto *loop = fossil::network::Async::create();
    int ret = fossil::network::Async::add(loop, nullptr, 1, nullptr);
    ASSUME_ITS_TRUE(ret == -1);
    fossil::network::Async::free(loop);
}

FOSSIL_TEST_CASE(cpp_asyncpp_test_run_null_loop) {
    int ret = fossil::network::Async::run(nullptr, 100);
    ASSUME_ITS_TRUE(ret == -1);
}

FOSSIL_TEST_CASE(cpp_asyncpp_test_run_empty_loop) {
    auto *loop = fossil::network::Async::create();
    int ret = fossil::network::Async::run(loop, 100);
    ASSUME_ITS_TRUE(ret == -1);
    fossil::network::Async::free(loop);
}


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_asyncpp_tests) {
    FOSSIL_TEST_ADD(cpp_asyncpp_fixture, cpp_asyncpp_test_create_and_free);
    FOSSIL_TEST_ADD(cpp_asyncpp_fixture, cpp_asyncpp_test_add_null_sock);
    FOSSIL_TEST_ADD(cpp_asyncpp_fixture, cpp_asyncpp_test_run_null_loop);
    FOSSIL_TEST_ADD(cpp_asyncpp_fixture, cpp_asyncpp_test_run_empty_loop);

    FOSSIL_TEST_REGISTER(cpp_asyncpp_fixture);
} // end of tests
