/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/network/framework.h"
#include <fossil/maip/framework.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_SUITE(cpp_client_fixture);

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

FOSSIL_TEST(cpp_client_test_create_and_destroy) {
    using fossil::net::Client;

    Client client_tcp("tcp", "ipv4");
    ASSUME_ITS_TRUE(client_tcp.native_handle() != nullptr);

    Client client_udp("udp", "ipv6");
    ASSUME_ITS_TRUE(client_udp.native_handle() != nullptr);

    // Invalid type/family
    Client client_invalid("invalid", "ipv4");
    ASSUME_ITS_TRUE(client_invalid.native_handle() == nullptr);
}

FOSSIL_TEST(cpp_client_test_connect_and_disconnect_loopback) {
    using fossil::net::Client;

    Client client("tcp", "ipv4");
    ASSUME_ITS_TRUE(client.native_handle() != nullptr);

    fossil_net_address_t addr;
    int rc = fossil_net_socket_address_parse(&addr, "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t server;
    rc = fossil_net_socket_create(&server, "tcp", "ipv4");
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_bind(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_get_local_address(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_listen(&server, 1);
    ASSUME_ITS_TRUE(rc == 0);

    rc = client.connect(&addr);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t accepted;
    rc = fossil_net_socket_accept(&server, &accepted, NULL);
    ASSUME_ITS_TRUE(rc == 0);

    rc = client.disconnect();
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_close(&accepted);
    fossil_net_socket_close(&server);
}

FOSSIL_TEST(cpp_client_test_send_and_receive) {
    using fossil::net::Client;

    Client client("tcp", "ipv4");
    ASSUME_ITS_TRUE(client.native_handle() != nullptr);

    fossil_net_address_t addr;
    int rc = fossil_net_socket_address_parse(&addr, "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t server;
    rc = fossil_net_socket_create(&server, "tcp", "ipv4");
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_bind(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_get_local_address(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_listen(&server, 1);
    ASSUME_ITS_TRUE(rc == 0);

    rc = client.connect(&addr);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t accepted;
    rc = fossil_net_socket_accept(&server, &accepted, NULL);
    ASSUME_ITS_TRUE(rc == 0);

    // Send from client to server
    const char msg[] = "hello";
    uint32_t sent = 0;
    rc = client.send(msg, strlen(msg) + 1, &sent);
    ASSUME_ITS_TRUE(rc == 0 && sent == strlen(msg) + 1);

    char buf[32] = {0};
    uint32_t recvd = 0;
    rc = fossil_net_socket_receive(&accepted, buf, sizeof(buf), &recvd);
    ASSUME_ITS_TRUE(rc == 0 && recvd == strlen(msg) + 1);
    ASSUME_ITS_TRUE(strcmp(buf, msg) == 0);

    // Send from server to client
    const char reply[] = "world";
    rc = fossil_net_socket_send(&accepted, reply, strlen(reply) + 1, &sent);
    ASSUME_ITS_TRUE(rc == 0 && sent == strlen(reply) + 1);

    memset(buf, 0, sizeof(buf));
    rc = client.receive(buf, sizeof(buf), &recvd);
    ASSUME_ITS_TRUE(rc == 0 && recvd == sizeof(reply));
    ASSUME_ITS_TRUE(strcmp(buf, reply) == 0);

    client.disconnect();
    fossil_net_socket_close(&accepted);
    fossil_net_socket_close(&server);
}

FOSSIL_TEST(cpp_client_test_get_addresses) {
    using fossil::net::Client;

    Client client("tcp", "ipv4");
    ASSUME_ITS_TRUE(client.native_handle() != nullptr);

    fossil_net_address_t addr;
    int rc = fossil_net_socket_address_parse(&addr, "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t server;
    rc = fossil_net_socket_create(&server, "tcp", "ipv4");
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_bind(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_get_local_address(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_listen(&server, 1);
    ASSUME_ITS_TRUE(rc == 0);

    rc = client.connect(&addr);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_address_t local, remote;
    rc = client.get_local_address(&local);
    ASSUME_ITS_TRUE(rc == 0);
    rc = client.get_remote_address(&remote);
    ASSUME_ITS_TRUE(rc == 0);

    char local_str[128], remote_str[128];
    rc = fossil_net_socket_address_to_string(&local, local_str, sizeof(local_str));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_address_to_string(&remote, remote_str, sizeof(remote_str));
    ASSUME_ITS_TRUE(rc == 0);

    ASSUME_ITS_TRUE(strlen(local_str) > 0);
    ASSUME_ITS_TRUE(strlen(remote_str) > 0);

    client.disconnect();
    fossil_net_socket_close(&server);
}

FOSSIL_TEST(cpp_client_test_set_blocking_and_error) {
    using fossil::net::Client;

    Client client("tcp", "ipv4");
    ASSUME_ITS_TRUE(client.native_handle() != nullptr);

    int rc = client.set_blocking(false);
    ASSUME_ITS_TRUE(rc == 0);
    rc = client.set_blocking(true);
    ASSUME_ITS_TRUE(rc == 0);

    // Try to connect to an invalid address to produce an error
    fossil_net_address_t addr;
    rc = fossil_net_socket_address_parse(&addr, "256.256.256.256", 1234);
    ASSUME_ITS_TRUE(rc == -1 || rc == 0);

    rc = client.connect(&addr);
    ASSUME_ITS_TRUE(rc != 0);

    int err = client.error_last();
    const char *errstr = Client::error_string(err);
    ASSUME_ITS_TRUE(errstr != nullptr);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_client_tests) {
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_create_and_destroy);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_connect_and_disconnect_loopback);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_send_and_receive);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_get_addresses);
    FOSSIL_TEST_ADD(cpp_client_fixture, cpp_client_test_set_blocking_and_error);

    FOSSIL_TEST_REGISTER(cpp_client_fixture);
} // end of tests
