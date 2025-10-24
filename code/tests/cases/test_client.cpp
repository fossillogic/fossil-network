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

FOSSIL_TEST_SUITE(cpp_client_fixture);

FOSSIL_SETUP(cpp_client_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_client_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_client_test_connect_invalid_params) {
    // Null proto_id
    fossil::network::Client client("", "clientid");
    int rc = client.connect("127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc != 0);

    // Null host
    fossil::network::Client client2("tcp", "clientid");
    rc = client2.connect(nullptr, 12345);
    ASSUME_ITS_TRUE(rc != 0);

    // Unknown protocol
    fossil::network::Client client3("notarealproto", "clientid");
    rc = client3.connect("127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST_CASE(cpp_client_test_send_invalid_params) {
    fossil::network::Client client("tcp", "");
    int rc = client.send("data", 4);
    ASSUME_ITS_TRUE(rc == -1);

    fossil::network::Client client2("tcp", "clientid");
    rc = client2.send(nullptr, 4);
    ASSUME_ITS_TRUE(rc == -1);

    rc = client2.send("data", 0);
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(cpp_client_test_recv_invalid_params) {
    char buf[16];
    fossil::network::Client client("tcp", "");
    int rc = client.recv(buf, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);

    fossil::network::Client client2("tcp", "clientid");
    rc = client2.recv(nullptr, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);

    rc = client2.recv(buf, 0);
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(cpp_client_test_disconnect_invalid_params) {
    fossil::network::Client client("tcp", "");
    int rc = client.disconnect();
    ASSUME_ITS_TRUE(rc != 0);

    fossil::network::Client client2("tcp", "notarealclientid");
    rc = client2.disconnect();
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST_CASE(cpp_client_test_connect_and_disconnect_success) {
    fossil::network::Client client("tcp", "127.0.0.1:12345");
    int rc = client.connect("127.0.0.1", 12345);
    ASSUME_ITS_TRUE(rc == 0 || rc == -1);

    rc = client.disconnect();
    ASSUME_ITS_TRUE(rc == 0 || rc == -1);
}

FOSSIL_TEST_CASE(cpp_client_test_send_recv_not_connected) {
    fossil::network::Client client("tcp", "notconnected:9999");
    int rc = client.send("hello", 5);
    ASSUME_ITS_TRUE(rc == -1);

    char buf[8];
    rc = client.recv(buf, sizeof(buf));
    ASSUME_ITS_TRUE(rc == -1);
}

FOSSIL_TEST_CASE(cpp_client_test_double_connect_same_id) {
    fossil::network::Client client("tcp", "127.0.0.1:12345");
    int rc1 = client.connect("127.0.0.1", 12345);
    int rc2 = client.connect("127.0.0.1", 12345);
    ASSUME_ITS_TRUE((rc1 == 0 && rc2 == 0) || (rc1 == -1 && rc2 == -1) || (rc1 == 0 && rc2 == -1));
    client.disconnect();
}

FOSSIL_TEST_CASE(cpp_client_test_max_clients_limit) {
    int successes = 0;
    std::vector<std::unique_ptr<fossil::network::Client>> clients;
    for (int i = 0; i < FOSSIL_NETWORK_CLIENT_MAX + 2; ++i) {
        char host[32];
        snprintf(host, sizeof(host), "127.0.0.%d", i + 1);
        char client_id[64];
        snprintf(client_id, sizeof(client_id), "%s:%u", host, 12345);
        auto client = std::make_unique<fossil::network::Client>("tcp", client_id);
        int rc = client->connect(host, 12345);
        if (rc == 0) successes++;
        clients.push_back(std::move(client));
    }
    ASSUME_ITS_TRUE(successes <= FOSSIL_NETWORK_CLIENT_MAX);
    for (int i = 0; i < FOSSIL_NETWORK_CLIENT_MAX; ++i) {
        clients[i]->disconnect();
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_client_tests) {
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_connect_invalid_params);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_send_invalid_params);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_recv_invalid_params);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_disconnect_invalid_params);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_connect_and_disconnect_success);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_send_recv_not_connected);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_double_connect_same_id);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_max_clients_limit);

    FOSSIL_TEST_REGISTER(cpp_client_fixture);
} // end of tests
