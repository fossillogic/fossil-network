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

FOSSIL_TEST_SUITE(cpp_socket_fixture);

FOSSIL_SETUP(cpp_socket_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_socket_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_socket_test_socket_init_cleanup) {
    int rc = fossil::network::Socket::init();
    ASSUME_ITS_TRUE(rc == 0);
    rc = fossil::network::Socket::cleanup();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(cpp_socket_test_proto_name_conversion) {
    fossil_protocol_t proto = fossil::network::Socket::proto_from_name("tcp");
    ASSUME_ITS_TRUE(proto != FOSSIL_PROTO_UNKNOWN);
    std::string name = fossil::network::Socket::proto_to_name(proto);
    ASSUME_ITS_TRUE(!name.empty());
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_create_close) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.close();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_bind_listen_close) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.bind("127.0.0.1", 0); // bind to any port
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.listen(1);
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.close();
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(cpp_socket_test_proto_from_name_unknown) {
    fossil_protocol_t proto = fossil::network::Socket::proto_from_name("notarealproto");
    ASSUME_ITS_TRUE(proto == FOSSIL_PROTO_UNKNOWN);
}

FOSSIL_TEST_CASE(cpp_socket_test_proto_to_name_unknown) {
    std::string name = fossil::network::Socket::proto_to_name(FOSSIL_PROTO_UNKNOWN);
    ASSUME_ITS_TRUE(name == "unknown");
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_set_get_option) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.set_option(SOL_SOCKET, SO_REUSEADDR, 1);
    ASSUME_ITS_TRUE(rc == 0);
    int value = 0;
    rc = sock.get_option(SOL_SOCKET, SO_REUSEADDR, &value);
    ASSUME_ITS_TRUE(rc == 0);
    sock.close();
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_set_nonblocking) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.set_nonblocking(1);
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.set_nonblocking(0);
    ASSUME_ITS_TRUE(rc == 0);
    sock.close();
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_is_ipv6) {
    fossil::network::Socket sock4, sock6;
    int rc = sock4.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock6.create(AF_INET6, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(sock4.is_ipv6() == 0);
    ASSUME_ITS_TRUE(sock6.is_ipv6() == 1);
    sock4.close();
    sock6.close();
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_open_close) {
    fossil::network::Socket sock;
    int rc = sock.open("tcp", "127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == -1 || rc == 0);
    sock.close();
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_resolve_hostname) {
    char ip[64];
    int rc = fossil::network::Socket::resolve_hostname("localhost", ip, sizeof(ip));
    ASSUME_ITS_TRUE(rc == 0);
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_get_address_local) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.bind("127.0.0.1", 0);
    ASSUME_ITS_TRUE(rc == 0);
    char addr[64];
    rc = sock.get_address(addr, sizeof(addr), 0);
    ASSUME_ITS_TRUE(rc == 0);
    sock.close();
}

FOSSIL_TEST_CASE(cpp_socket_test_socket_set_timeout) {
    fossil::network::Socket sock;
    int rc = sock.create(AF_INET, fossil::network::Socket::proto_from_name("tcp"));
    ASSUME_ITS_TRUE(rc == 0);
    rc = sock.set_timeout(100, 100);
    ASSUME_ITS_TRUE(rc == 0);
    sock.close();
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_socket_tests) {
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_init_cleanup);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_proto_name_conversion);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_create_close);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_bind_listen_close);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_proto_from_name_unknown);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_proto_to_name_unknown);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_set_get_option);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_set_nonblocking);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_is_ipv6);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_open_close);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_resolve_hostname);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_get_address_local);
    FOSSIL_TEST_ADD(cpp_socket_fixture, cpp_socket_test_socket_set_timeout);

    FOSSIL_TEST_REGISTER(cpp_socket_fixture);
} // end of tests
