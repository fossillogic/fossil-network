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
#include <fossil/unittest/framework.h>
#include <fossil/mockup/framework.h>
#include <fossil/unittest/assume.h>

#include "fossil/network/framework.h"

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test: Squid Network Client
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(test_client_create_and_destroy) {
    // Create client socket
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation succeeded

    // Close and destroy client
    fossil_net_close_client(client);
    fossil_net_destroy_client(client);
}

FOSSIL_TEST(test_client_send_and_receive) {
    // Create client socket and connect
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation succeeded

    // Send message to server
    const char *message = "Hello Server";
    int bytes_sent = fossil_net_send_client(client, message, strlen(message));
    ASSUME_ITS_EQUAL_I32(strlen(message), bytes_sent);  // Verify all bytes were sent

    // Receive message from server
    char buffer[128];
    int bytes_received = fossil_net_receive_client(client, buffer, sizeof(buffer));
    ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Verify some bytes were received
    buffer[bytes_received] = '\0';  // Null-terminate the received string

    // Check received message
    ASSUME_ITS_EQUAL_CSTR("Hello Client", buffer);

    // Close and destroy client
    fossil_net_close_client(client);
    fossil_net_destroy_client(client);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool for Network Client
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_network_client_tests) {
    ADD_TEST(test_client_create_and_destroy);
    ADD_TEST(test_client_send_and_receive);
}
