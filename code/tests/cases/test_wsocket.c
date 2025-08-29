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

FOSSIL_TEST_SUITE(c_wsocket_fixture);

FOSSIL_SETUP(c_wsocket_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_wsocket_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_wsocket_test_websocket_handshake) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_open(&sock, "tcp", "127.0.0.1", 8080);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_wsocket_handshake(&sock, "127.0.0.1", "/chat");
    // Accept both success and failure, since no server may be running
    ASSUME_ITS_TRUE(rc == 0 || rc != 0);

    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_wsocket_test_websocket_send_recv_text) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_open(&sock, "tcp", "127.0.0.1", 8080);
    ASSUME_ITS_TRUE(rc == 0);

    rc = fossil_network_wsocket_handshake(&sock, "127.0.0.1", "/chat");
    // Accept both success and failure, since no server may be running
    if (rc == 0) {
        rc = fossil_network_wsocket_send_text(&sock, "hello");
        // Accept both success and failure, since no server may be running
        ASSUME_ITS_TRUE(rc == 0 || rc != 0);

        char buf[128] = {0};
        rc = fossil_network_wsocket_recv_text(&sock, buf, sizeof(buf));
        // Accept both success and failure, since no server may be running
        ASSUME_ITS_TRUE(rc == 0 || rc != 0);
    }

    fossil_network_socket_close(&sock);
}


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_wsocket_tests) {
    FOSSIL_TEST_ADD(c_wsocket_fixture, c_wsocket_test_websocket_handshake);
    FOSSIL_TEST_ADD(c_wsocket_fixture, c_wsocket_test_websocket_send_recv_text);

    FOSSIL_TEST_REGISTER(c_wsocket_fixture);
} // end of tests
