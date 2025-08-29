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

FOSSIL_TEST_SUITE(c_tcp_fixture);

FOSSIL_SETUP(c_tcp_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_tcp_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_tcp_test_connect_invalid_host) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_tcp_connect(&sock, "256.256.256.256", 12345); // Invalid IP
    ASSUME_ITS_TRUE(rc != 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_tcp_test_listen_and_accept) {
    fossil_network_socket_t server, client;
    int rc = fossil_network_socket_create(&server, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_tcp_listen(&server, "127.0.0.1", 0, 1); // Bind to any port
    ASSUME_ITS_TRUE(rc == 0);

    // Get the port assigned by the OS
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    rc = getsockname(server.fd, (struct sockaddr *)&addr, &addrlen);
    ASSUME_ITS_TRUE(rc == 0);
    // uint16_t port = ntohs(addr.sin_port); // Removed unused variable

    // Start a client in another thread/process would be ideal, but here we just test accept fails with no connection
    rc = fossil_network_tcp_accept(&server, &client);
    ASSUME_ITS_TRUE(rc != 0);

    fossil_network_socket_close(&server);
}

// FOSSIL_TEST_CASE(c_tcp_test_listen_invalid_port) {
//     fossil_network_socket_t sock;
//     int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
//     ASSUME_ITS_TRUE(rc == 0);
//     rc = fossil_network_tcp_listen(&sock, "127.0.0.1", (uint16_t)65536, 1); // Invalid port (explicit cast to suppress warning)
//     ASSUME_ITS_TRUE(rc != 0);
//     fossil_network_socket_close(&sock);
// }

// FOSSIL_TEST_CASE(c_tcp_test_connect_null_params) {
//     int rc = fossil_network_tcp_connect(NULL, "127.0.0.1", 80);
//     ASSUME_ITS_TRUE(rc != 0);
//     fossil_network_socket_t sock;
//     rc = fossil_network_tcp_connect(&sock, NULL, 80);
//     ASSUME_ITS_TRUE(rc != 0);
// }

// FOSSIL_TEST_CASE(c_tcp_test_listen_null_sock) {
//     int rc = fossil_network_tcp_listen(NULL, "127.0.0.1", 8080, 1);
//     ASSUME_ITS_TRUE(rc != 0);
// }

// FOSSIL_TEST_CASE(c_tcp_test_accept_null_params) {
//     fossil_network_socket_t sock;
//     int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
//     ASSUME_ITS_TRUE(rc == 0);
//     rc = fossil_network_tcp_accept(NULL, &sock);
//     ASSUME_ITS_TRUE(rc != 0);
//     rc = fossil_network_tcp_accept(&sock, NULL);
//     ASSUME_ITS_TRUE(rc != 0);
//     fossil_network_socket_close(&sock);
// }

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_tcp_tests) {
    FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_connect_invalid_host);
    FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_listen_and_accept);
    // FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_listen_invalid_port);
    // FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_connect_null_params);
    // FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_listen_null_sock);
    // FOSSIL_TEST_ADD(c_tcp_fixture, c_tcp_test_accept_null_params);

    FOSSIL_TEST_REGISTER(c_tcp_fixture);
} // end of tests
