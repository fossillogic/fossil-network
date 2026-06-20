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

FOSSIL_SUITE(c_client_fixture);

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

FOSSIL_TEST(c_client_test_create_and_destroy) {
    fossil_net_client_t *client = fossil_net_client_create("tcp", "ipv4");
    ASSUME_ITS_TRUE(client != NULL);
    fossil_net_client_destroy(client);

    client = fossil_net_client_create("udp", "ipv6");
    ASSUME_ITS_TRUE(client != NULL);
    fossil_net_client_destroy(client);

    // Invalid type/family
    client = fossil_net_client_create("invalid", "ipv4");
    ASSUME_ITS_TRUE(client == NULL);
}

FOSSIL_TEST(c_client_test_connect_and_disconnect_loopback) {
    fossil_net_client_t *client = fossil_net_client_create("tcp", "ipv4");
    ASSUME_ITS_TRUE(client != NULL);

    fossil_net_address_t addr;
    int rc = fossil_net_socket_address_parse(&addr, "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);

    // Bind a server socket to accept connection
    fossil_net_socket_t server;
    rc = fossil_net_socket_create(&server, "tcp", "ipv4");
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_bind(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_get_local_address(&server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_listen(&server, 1);
    ASSUME_ITS_TRUE(rc == 0);

    // Connect client to server
    rc = fossil_net_client_connect(client, &addr);
    ASSUME_ITS_TRUE(rc == 0);

    // Accept connection on server side
    fossil_net_socket_t accepted;
    rc = fossil_net_socket_accept(&server, &accepted, NULL);
    ASSUME_ITS_TRUE(rc == 0);

    // Disconnect client
    rc = fossil_net_client_disconnect(client);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_close(&accepted);
    fossil_net_socket_close(&server);
    fossil_net_client_destroy(client);
}

FOSSIL_TEST(c_client_test_send_and_receive) {
    fossil_net_client_t *client = fossil_net_client_create("tcp", "ipv4");
    ASSUME_ITS_TRUE(client != NULL);

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

    rc = fossil_net_client_connect(client, &addr);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_socket_t accepted;
    rc = fossil_net_socket_accept(&server, &accepted, NULL);
    ASSUME_ITS_TRUE(rc == 0);

    // Send from client to server
    const char msg[] = "hello";
    uint32_t sent = 0;
    rc = fossil_net_client_send(client, msg, strlen(msg), &sent);
    ASSUME_ITS_TRUE(rc == 0 && sent == strlen(msg));

    char buf[32] = {0};
    uint32_t recvd = 0;
    rc = fossil_net_socket_receive(&accepted, buf, sizeof(buf), &recvd);
    ASSUME_ITS_TRUE(rc == 0 && recvd == strlen(msg));
    ASSUME_ITS_TRUE(strncmp(buf, msg, strlen(msg)) == 0);

    // Send from server to client
    const char reply[] = "world";
    rc = fossil_net_socket_send(&accepted, reply, strlen(reply), &sent);
    ASSUME_ITS_TRUE(rc == 0 && sent == strlen(reply));

    memset(buf, 0, sizeof(buf));
    rc = fossil_net_client_receive(client, buf, sizeof(buf), &recvd);
    ASSUME_ITS_TRUE(rc == 0 && recvd == strlen(reply));
    ASSUME_ITS_TRUE(strncmp(buf, reply, strlen(reply)) == 0);

    fossil_net_client_disconnect(client);
    fossil_net_socket_close(&accepted);
    fossil_net_socket_close(&server);
    fossil_net_client_destroy(client);
}

FOSSIL_TEST(c_client_test_get_addresses) {
    fossil_net_client_t *client = fossil_net_client_create("tcp", "ipv4");
    ASSUME_ITS_TRUE(client != NULL);

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

    rc = fossil_net_client_connect(client, &addr);
    ASSUME_ITS_TRUE(rc == 0);

    fossil_net_address_t local, remote;
    rc = fossil_net_client_get_local_address(client, &local);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_client_get_remote_address(client, &remote);
    ASSUME_ITS_TRUE(rc == 0);

    char local_str[128], remote_str[128];
    rc = fossil_net_socket_address_to_string(&local, local_str, sizeof(local_str));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_socket_address_to_string(&remote, remote_str, sizeof(remote_str));
    ASSUME_ITS_TRUE(rc == 0);

    ASSUME_ITS_TRUE(strlen(local_str) > 0);
    ASSUME_ITS_TRUE(strlen(remote_str) > 0);

    fossil_net_client_disconnect(client);
    fossil_net_socket_close(&server);
    fossil_net_client_destroy(client);
}

FOSSIL_TEST(c_client_test_set_blocking_and_error) {
    fossil_net_client_t *client = fossil_net_client_create("tcp", "ipv4");
    ASSUME_ITS_TRUE(client != NULL);

    int rc = fossil_net_client_set_blocking(client, false);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_client_set_blocking(client, true);
    ASSUME_ITS_TRUE(rc == 0);

    // Try to connect to an invalid address to produce an error
    fossil_net_address_t addr;
    rc = fossil_net_socket_address_parse(&addr, "256.256.256.256", 1234);
    ASSUME_ITS_TRUE(rc == -1 || rc == 0); // parse may fail

    rc = fossil_net_client_connect(client, &addr);
    ASSUME_ITS_TRUE(rc != 0);

    int err = fossil_net_client_error_last(client);
    const char *errstr = fossil_net_client_error_string(err);
    ASSUME_ITS_TRUE(errstr != NULL);

    fossil_net_client_destroy(client);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_client_tests) {
    FOSSIL_ADD_TEST(c_client_fixture, c_client_test_create_and_destroy);
    FOSSIL_ADD_TEST(c_client_fixture, c_client_test_connect_and_disconnect_loopback);
    FOSSIL_ADD_TEST(c_client_fixture, c_client_test_send_and_receive);
    FOSSIL_ADD_TEST(c_client_fixture, c_client_test_get_addresses);
    FOSSIL_ADD_TEST(c_client_fixture, c_client_test_set_blocking_and_error);

    FOSSIL_ADD_SUITE(c_client_fixture);
} // end of tests
