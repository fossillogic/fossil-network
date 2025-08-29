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

FOSSIL_TEST_SUITE(c_protocol_fixture);

FOSSIL_SETUP(c_protocol_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_protocol_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_protocol_test_send_length_null_sock) {
    const char msg[] = "hello";
    int rc = fossil_network_proto_send_length(NULL, msg, sizeof(msg));
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(c_protocol_test_send_length_null_buf) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_proto_send_length(&sock, NULL, 5);
    ASSUME_ITS_TRUE(rc == -1);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_protocol_test_recv_length_null_sock) {
    char buf[32];
    int rc = fossil_network_proto_recv_length(NULL, buf, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(c_protocol_test_recv_length_null_buf) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_proto_recv_length(&sock, NULL, 10);
    ASSUME_ITS_TRUE(rc == -1);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_protocol_test_recv_length_too_small_buffer) {
#ifdef _WIN32
    // skip on Windows
    ASSUME_ITS_TRUE(1);
#else
    int sv[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_network_socket_t sock1 = { .fd = sv[0] };
    fossil_network_socket_t sock2 = { .fd = sv[1] };

    const char msg[] = "0123456789abcdef";
    rc = fossil_network_proto_send_length(&sock1, msg, sizeof(msg));
    ASSUME_ITS_TRUE(rc == 0);

    char small_buf[4];
    rc = fossil_network_proto_recv_length(&sock2, small_buf, sizeof(small_buf));
    ASSUME_ITS_TRUE(rc == -1);

    close(sv[0]);
    close(sv[1]);
#endif
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_protocol_tests) {
    FOSSIL_TEST_ADD(c_protocol_fixture, c_protocol_test_send_length_null_sock);
    FOSSIL_TEST_ADD(c_protocol_fixture, c_protocol_test_send_length_null_buf);
    FOSSIL_TEST_ADD(c_protocol_fixture, c_protocol_test_recv_length_null_sock);
    FOSSIL_TEST_ADD(c_protocol_fixture, c_protocol_test_recv_length_null_buf);
    FOSSIL_TEST_ADD(c_protocol_fixture, c_protocol_test_recv_length_too_small_buffer);

    FOSSIL_TEST_REGISTER(c_protocol_fixture);
} // end of tests
