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

FOSSIL_SUITE(cpp_server_fixture);

FOSSIL_SETUP(cpp_server_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_server_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::net::Server;

FOSSIL_TEST(cpp_server_test_server_create_and_destroy) {
    Server server("tcp", "ipv4", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(server.is_valid());
    // Destructor will clean up
}

FOSSIL_TEST(cpp_server_test_server_create_invalid_params) {
    Server server1("invalid", "ipv4", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(!server1.is_valid());
    Server server2("tcp", "invalid", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(!server2.is_valid());
}

FOSSIL_TEST(cpp_server_test_server_listen_and_get_address) {
    Server server("tcp", "ipv4", "", 0);
    ASSUME_ITS_TRUE(server.is_valid());
    int rc = server.listen(1);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_address_t addr;
    rc = server.get_address(&addr);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST(cpp_server_test_server_set_blocking) {
    Server server("tcp", "ipv4", "", 0);
    ASSUME_ITS_TRUE(server.is_valid());
    int rc = server.set_blocking(false);
    ASSUME_ITS_TRUE(rc == 0);
    rc = server.set_blocking(true);
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST(cpp_server_test_server_accept_fail) {
    Server server("tcp", "ipv4", "", 0);
    ASSUME_ITS_TRUE(server.is_valid());
    int rc = server.listen(1);
    ASSUME_ITS_TRUE(rc == 0);
    fossil_net_socket_t client_sock;
    fossil_net_address_t client_addr;
    // No client will connect, so accept should fail or timeout
    rc = server.accept(&client_sock, &client_addr);
    ASSUME_ITS_TRUE(rc != 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_server_tests) {
    FOSSIL_ADD_TEST(cpp_server_fixture, cpp_server_test_server_create_and_destroy);
    FOSSIL_ADD_TEST(cpp_server_fixture, cpp_server_test_server_create_invalid_params);
    FOSSIL_ADD_TEST(cpp_server_fixture, cpp_server_test_server_listen_and_get_address);
    FOSSIL_ADD_TEST(cpp_server_fixture, cpp_server_test_server_set_blocking);
    FOSSIL_ADD_TEST(cpp_server_fixture, cpp_server_test_server_accept_fail);

    FOSSIL_ADD_SUITE(cpp_server_fixture);
} // end of tests
