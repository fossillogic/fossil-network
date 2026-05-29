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

FOSSIL_SUITE(c_server_fixture);

FOSSIL_SETUP(c_server_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_server_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_server_test_server_create_and_destroy) {
    fossil_net_server_t *server = fossil_net_server_create("tcp", "ipv4", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(server != NULL);
    fossil_net_server_destroy(server);
}

FOSSIL_TEST(c_server_test_server_create_invalid_params) {
    fossil_net_server_t *server = fossil_net_server_create("invalid", "ipv4", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(server == NULL);
    server = fossil_net_server_create("tcp", "invalid", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(server == NULL);
}

FOSSIL_TEST(c_server_test_server_listen_and_get_address) {
    fossil_net_server_t *server = fossil_net_server_create("tcp", "ipv4", NULL, 0);
    ASSUME_ITS_TRUE(server != NULL);
    int rc = fossil_net_server_listen(server, 1);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_address_t addr;
    rc = fossil_net_server_get_address(server, &addr);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_server_destroy(server);
}

FOSSIL_TEST(c_server_test_server_set_blocking) {
    fossil_net_server_t *server = fossil_net_server_create("tcp", "ipv4", NULL, 0);
    ASSUME_ITS_TRUE(server != NULL);
    int rc = fossil_net_server_set_blocking(server, false);
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil_net_server_set_blocking(server, true);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_server_destroy(server);
}

FOSSIL_TEST(c_server_test_server_accept_fail) {
    fossil_net_server_t *server = fossil_net_server_create("tcp", "ipv4", NULL, 0);
    ASSUME_ITS_TRUE(server != NULL);
    int rc = fossil_net_server_listen(server, 1);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_socket_t client_sock;
    fossil_net_address_t client_addr;
    // No client will connect, so accept should fail or timeout
    rc = fossil_net_server_accept(server, &client_sock, &client_addr);
    ASSUME_ITS_TRUE(rc != 0);
    fossil_net_server_destroy(server);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_server_tests) {
    FOSSIL_TEST_ADD(c_server_fixture, c_server_test_server_create_and_destroy);
    FOSSIL_TEST_ADD(c_server_fixture, c_server_test_server_create_invalid_params);
    FOSSIL_TEST_ADD(c_server_fixture, c_server_test_server_listen_and_get_address);
    FOSSIL_TEST_ADD(c_server_fixture, c_server_test_server_set_blocking);
    FOSSIL_TEST_ADD(c_server_fixture, c_server_test_server_accept_fail);

    FOSSIL_TEST_REGISTER(c_server_fixture);
} // end of tests
