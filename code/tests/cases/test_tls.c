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

FOSSIL_TEST_SUITE(c_tls_fixture);

FOSSIL_SETUP(c_tls_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_tls_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_tls_test_create_and_free_ctx) {
    fossil_network_tls_ctx_t *ctx = fossil_network_tls_create(0); // client mode
    ASSUME_ITS_TRUE(ctx != NULL);
    fossil_network_tls_free(ctx);

    ctx = fossil_network_tls_create(1); // server mode
    ASSUME_ITS_TRUE(ctx != NULL);
    fossil_network_tls_free(ctx);
}

FOSSIL_TEST_CASE(c_tls_test_wrap_null_socket) {
    fossil_network_tls_ctx_t *ctx = fossil_network_tls_create(0);
    int rc = fossil_network_tls_wrap(ctx, NULL);
    ASSUME_ITS_TRUE(rc != 0);
    fossil_network_tls_free(ctx);
}

FOSSIL_TEST_CASE(c_tls_test_wrap_null_ctx) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_tls_wrap(NULL, &sock);
    ASSUME_ITS_TRUE(rc != 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_tls_test_send_recv_null_ctx) {
    char buf[16] = {0};
    ssize_t sent = fossil_network_tls_send(NULL, "abc", 3);
    ASSUME_ITS_TRUE(sent == -1);
    ssize_t recvd = fossil_network_tls_recv(NULL, buf, sizeof(buf));
    ASSUME_ITS_TRUE(recvd == -1);
}

FOSSIL_TEST_CASE(c_tls_test_send_recv_null_sock) {
    fossil_network_tls_ctx_t *ctx = fossil_network_tls_create(0);
    char buf[16] = {0};
    ssize_t sent = fossil_network_tls_send(ctx, "abc", 3);
    ASSUME_ITS_TRUE(sent == -1);
    ssize_t recvd = fossil_network_tls_recv(ctx, buf, sizeof(buf));
    ASSUME_ITS_TRUE(recvd == -1);
    fossil_network_tls_free(ctx);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_tls_tests) {
    FOSSIL_TEST_ADD(c_tls_fixture, c_tls_test_create_and_free_ctx);
    FOSSIL_TEST_ADD(c_tls_fixture, c_tls_test_wrap_null_socket);
    FOSSIL_TEST_ADD(c_tls_fixture, c_tls_test_wrap_null_ctx);
    FOSSIL_TEST_ADD(c_tls_fixture, c_tls_test_send_recv_null_ctx);
    FOSSIL_TEST_ADD(c_tls_fixture, c_tls_test_send_recv_null_sock);

    FOSSIL_TEST_REGISTER(c_tls_fixture);
} // end of tests
