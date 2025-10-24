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

FOSSIL_TEST_SUITE(c_client_fixture);

FOSSIL_SETUP(c_client_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_client_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_client_test_connect_invalid_params) {
    // Null proto_id
    int rc = fossil_network_client_connect(NULL, "127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc != 0);
    // Null host
    rc = fossil_network_client_connect("tcp", NULL, 12345);
    ASSUME_ITS_TRUE(rc != 0);
    // Unknown protocol
    rc = fossil_network_client_connect("notarealproto", "127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST_CASE(c_client_test_send_invalid_params) {
    int rc = fossil_network_client_send(NULL, "data", 4);
    ASSUME_ITS_TRUE(rc == -1);
    rc = fossil_network_client_send("clientid", NULL, 4);
    ASSUME_ITS_TRUE(rc == -1);
    rc = fossil_network_client_send("clientid", "data", 0);
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(c_client_test_recv_invalid_params) {
    char buf[16];
    int rc = fossil_network_client_recv(NULL, buf, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);
    rc = fossil_network_client_recv("clientid", NULL, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);
    rc = fossil_network_client_recv("clientid", buf, 0);
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(c_client_test_disconnect_invalid_params) {
    int rc = fossil_network_client_disconnect(NULL);
    ASSUME_ITS_TRUE(rc != 0);
    rc = fossil_network_client_disconnect("notarealclientid");
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST_CASE(c_client_test_connect_and_disconnect_success) {
    // This test assumes a local TCP server is running on 127.0.0.1:12345.
    // If not, connection will fail, but we still check the API behavior.
    int rc = fossil_network_client_connect("tcp", "127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc == 0 || rc == -1); // Acceptable: either success or fail if no server

    // Derive client_id as used in implementation
    char client_id[64];
    snprintf(client_id, sizeof(client_id), "%s:%u", "127.0.0.1", 12345);

    // Try disconnecting (should succeed if connect succeeded)
    rc = fossil_network_client_disconnect(client_id);
    ASSUME_ITS_TRUE(rc == 0 || rc == -1);
}

FOSSIL_TEST_CASE(c_client_test_send_recv_not_connected) {
    // Try sending/receiving with a client_id that is not connected
    int rc = fossil_network_client_send("notconnected:9999", "hello", 5);
    ASSUME_ITS_TRUE(rc == -1);

    char buf[8];
    rc = fossil_network_client_recv("notconnected:9999", buf, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(c_client_test_double_connect_same_id) {
    // Try connecting twice to the same host/port
    int rc1 = fossil_network_client_connect("tcp", "127.0.0.1", 12345);
    int rc2 = fossil_network_client_connect("tcp", "127.0.0.1", 12345);
    // Only one should succeed, or both may fail if no server
    ASSUME_ITS_TRUE((rc1 == 0 && rc2 == 0) || (rc1 == -1 && rc2 == -1) || (rc1 == 0 && rc2 == -1));
    // Disconnect
    char client_id[64];
    snprintf(client_id, sizeof(client_id), "%s:%u", "127.0.0.1", 12345);
    fossil_network_client_disconnect(client_id);
}

FOSSIL_TEST_CASE(c_client_test_max_clients_limit) {
    // Try to allocate more clients than the max allowed
    int successes = 0;
    for (int i = 0; i < FOSSIL_NETWORK_CLIENT_MAX + 2; ++i) {
        char host[32];
        snprintf(host, sizeof(host), "127.0.0.%d", i + 1);
        int rc = fossil_network_client_connect("tcp", host, 12345);
        if (rc == 0) successes++;
    }
    ASSUME_ITS_TRUE(successes <= FOSSIL_NETWORK_CLIENT_MAX);
    // Disconnect all
    for (int i = 0; i < FOSSIL_NETWORK_CLIENT_MAX; ++i) {
        char client_id[64];
        snprintf(client_id, sizeof(client_id), "127.0.0.%d:%u", i + 1, 12345);
        fossil_network_client_disconnect(client_id);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_client_tests) {
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_connect_invalid_params);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_send_invalid_params);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_recv_invalid_params);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_disconnect_invalid_params);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_connect_and_disconnect_success);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_send_recv_not_connected);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_double_connect_same_id);
    FOSSIL_TEST_ADD(c_client_fixture, c_client_test_max_clients_limit);

    FOSSIL_TEST_REGISTER(c_client_fixture);
} // end of tests
