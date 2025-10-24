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
#include <fossil/pizza/framework.h>


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_SUITE(c_socket_fixture);

FOSSIL_SETUP(c_socket_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_socket_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_socket_test_socket_init_cleanup) {
    int rc = fossil_network_socket_init();
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_cleanup();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_socket_test_proto_name_conversion) {
    fossil_protocol_t proto = fossil_network_socket_proto_from_name("tcp");
    ASSUME_ITS_TRUE(proto != FOSSIL_PROTO_UNKNOWN);
    const char *name = fossil_network_socket_proto_to_name(proto);
    ASSUME_ITS_TRUE(name != NULL);
}

FOSSIL_TEST_CASE(c_socket_test_socket_create_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_socket_test_socket_bind_listen_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_bind(&sock, "127.0.0.1", 0); // bind to any port
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_listen(&sock, 1);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_close(&sock);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_socket_test_proto_from_name_unknown) {
    fossil_protocol_t proto = fossil_network_socket_proto_from_name("notarealproto");
    ASSUME_ITS_TRUE(proto == FOSSIL_PROTO_UNKNOWN);
}

FOSSIL_TEST_CASE(c_socket_test_proto_to_name_unknown) {
    const char *name = fossil_network_socket_proto_to_name(FOSSIL_PROTO_UNKNOWN);
    ASSUME_ITS_TRUE(strcmp(name, "unknown") == 0);
}

FOSSIL_TEST_CASE(c_socket_test_socket_set_get_option) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_set_option(&sock, SOL_SOCKET, SO_REUSEADDR, 1);
    ASSUME_ITS_TRUE(rc == 0);
    int value = 0;
    rc = fossil_network_socket_get_option(&sock, SOL_SOCKET, SO_REUSEADDR, &value);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_socket_test_socket_set_nonblocking) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_set_nonblocking(&sock, 1);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_set_nonblocking(&sock, 0);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_socket_test_socket_is_ipv6) {
    fossil_network_socket_t sock4, sock6;
    int rc = fossil_network_socket_create(&sock4, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_create(&sock6, AF_INET6, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(fossil_network_socket_is_ipv6(&sock4) == 0);
    ASSUME_ITS_TRUE(fossil_network_socket_is_ipv6(&sock6) == 1);
    fossil_network_socket_close(&sock4);
    fossil_network_socket_close(&sock6);
}

FOSSIL_TEST_CASE(c_socket_test_socket_open_close) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_open(&sock, "tcp", "127.0.0.1", 0);
    // Should succeed in creating, even if connect fails (port 0 is invalid for connect)
    ASSUME_ITS_TRUE(rc == -1 || rc == 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_socket_test_socket_resolve_hostname) {
    char ip[64];
    int rc = fossil_network_socket_resolve_hostname("localhost", ip, sizeof(ip));
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(c_socket_test_socket_get_address_local) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_bind(&sock, "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);
    char addr[64];
    rc = fossil_network_socket_get_address(&sock, addr, sizeof(addr), 0);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_network_socket_close(&sock);
}

FOSSIL_TEST_CASE(c_socket_test_socket_set_timeout) {
    fossil_network_socket_t sock;
    int rc = fossil_network_socket_create(&sock, AF_INET, fossil_network_socket_proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_network_socket_set_timeout(&sock, 100, 100);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_network_socket_close(&sock);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_socket_tests) {
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_init_cleanup);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_proto_name_conversion);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_create_close);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_bind_listen_close);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_proto_from_name_unknown);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_proto_to_name_unknown);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_set_get_option);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_set_nonblocking);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_is_ipv6);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_open_close);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_resolve_hostname);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_get_address_local);
    FOSSIL_TEST_ADD(c_socket_fixture, c_socket_test_socket_set_timeout);

    FOSSIL_TEST_REGISTER(c_socket_fixture);
} // end of tests
