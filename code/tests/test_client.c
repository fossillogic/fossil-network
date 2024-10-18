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

#include <fossil/unittest/framework.h>  // Includes the Fossil Unit Test Framework
#include <fossil/mockup/framework.h>    // Includes the Fossil Mockup Framework
#include <fossil/unittest/assume.h>     // Includes the Fossil Assume Framework
#include "fossil/network/framework.h"

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Squid Network Client Tests
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test client creation and connection to the server
FOSSIL_TEST(test_client_create_and_connect) {
    // Create the client socket
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Ensure client was created successfully

    // Connect to the server
    int connect_status = fossil_net_client_connect(client, "127.0.0.1", 8080);
    ASSUME_ITS_EQUAL_I32(0, connect_status);  // Ensure the client connected successfully

    // Clean up
    fossil_net_destroy_client(client);
}

// Test client sending and receiving messages
FOSSIL_TEST(test_client_send_and_receive) {
    // Create and connect the client socket to the server
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation
    int connect_status = fossil_net_client_connect(client, "127.0.0.1", 8080);
    ASSUME_ITS_EQUAL_I32(0, connect_status);  // Ensure the client connected successfully

    // Send a message to the server
    const char *message = "Hello Server";
    int bytes_sent = fossil_net_send_client(client, message, strlen(message));
    ASSUME_ITS_EQUAL_I32(strlen(message), bytes_sent);  // Verify the message was sent

    // Receive response from the server
    char buffer[128];
    int bytes_received = fossil_net_receive_client(client, buffer, sizeof(buffer));
    ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Ensure some bytes were received
    buffer[bytes_received] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR("Hello Client", buffer);  // Verify the response content

    // Clean up
    fossil_net_destroy_client(client);
}

// Test client handling disconnection from the server
FOSSIL_TEST(test_client_disconnect) {
    // Create and connect the client socket to the server
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation
    int connect_status = fossil_net_client_connect(client, "127.0.0.1", 8080);
    ASSUME_ITS_EQUAL_I32(0, connect_status);  // Ensure the client connected successfully

    // Disconnect the client
    fossil_net_close_client(client);

    // Verify the client is no longer connected
    ASSUME_ITS_EQUAL_I32(FOSSIL_NET_ERROR, fossil_net_send_client(client, "Test", 4)); // Ensure send fails

    // Clean up (client should already be cleaned up in close_client)
}

// Test handling multiple messages between client and server
FOSSIL_TEST(test_client_multiple_messages) {
    // Create and connect the client socket to the server
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation
    int connect_status = fossil_net_client_connect(client, "127.0.0.1", 8080);
    ASSUME_ITS_EQUAL_I32(0, connect_status);  // Ensure the client connected successfully

    const char *messages[] = { "Message 1", "Message 2", "Message 3" };
    for (int i = 0; i < 3; ++i) {
        int bytes_sent = fossil_net_send_client(client, messages[i], strlen(messages[i]));
        ASSUME_ITS_EQUAL_I32(strlen(messages[i]), bytes_sent);  // Verify each message was sent

        // Prepare to receive a response
        char buffer[128];
        int bytes_received = fossil_net_receive_client(client, buffer, sizeof(buffer));
        ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Ensure some bytes were received
        buffer[bytes_received] = '\0';  // Null-terminate the buffer
        ASSUME_ITS_EQUAL_CSTR("Response", buffer);  // Verify the expected response
    }

    // Clean up
    fossil_net_destroy_client(client);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_network_client_tests) {
    ADD_TEST(test_client_create_and_connect);         // Test for client creation and connection
    ADD_TEST(test_client_send_and_receive);            // Test for sending and receiving messages
    ADD_TEST(test_client_disconnect);                  // Test for handling client disconnection
    ADD_TEST(test_client_multiple_messages);           // Test for multiple message handling
}
