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

// Test client connection to the server
FOSSIL_TEST(test_client_connect) {
    // Create the server socket
    fossil_net_server_socket_t* server = fossil_net_create_server("127.0.0.1", 8080, FOSSIL_NET_PROTOCOL_TCP);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Simulate accepting a client connection
    fossil_net_client_socket_t* client = fossil_net_accept_client(server);
    ASSUME_NOT_CNULL(client);  // Verify client connection

    // Clean up
    fossil_net_destroy_client(client);
    fossil_net_destroy_server(server);
}

// Test client sending and receiving messages
FOSSIL_TEST(test_client_send_and_receive) {
    // Create the server socket
    fossil_net_server_socket_t* server = fossil_net_create_server("127.0.0.1", 8080, FOSSIL_NET_PROTOCOL_TCP);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Simulate client connection
    fossil_net_client_socket_t* client = fossil_net_accept_client(server);
    ASSUME_NOT_CNULL(client);  // Verify client connection

    // Send a message to the client
    const char* message = "Hello Server";
    int bytes_sent = fossil_net_send_to_client(client, message, strlen(message));
    ASSUME_ITS_EQUAL_I32(strlen(message), bytes_sent);  // Verify the message was sent

    // Prepare to receive a response
    char buffer[128];
    int bytes_received = fossil_net_receive_from_client(client, buffer, sizeof(buffer));
    ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Ensure data was received
    buffer[bytes_received] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR(message, buffer);  // Verify the response content

    // Clean up
    fossil_net_destroy_client(client);
    fossil_net_destroy_server(server);
}

// Test client disconnection handling
FOSSIL_TEST(test_client_disconnect) {
    // Create the server socket
    fossil_net_server_socket_t* server = fossil_net_create_server("127.0.0.1", 8080, FOSSIL_NET_PROTOCOL_TCP);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Simulate client connection
    fossil_net_client_socket_t* client = fossil_net_accept_client(server);
    ASSUME_NOT_CNULL(client);  // Verify client connection

    // Disconnect the client
    fossil_net_destroy_client(client);

    // Verify the client is no longer connected
    ASSUME_ITS_EQUAL_I32(FOSSIL_NET_ERROR, fossil_net_send_to_client(server, "Test", 4)); // Ensure sending fails

    // Clean up
    fossil_net_destroy_server(server);
}

// Test handling multiple messages between client and server
FOSSIL_TEST(test_client_multiple_messages) {
    // Create the server socket
    fossil_net_server_socket_t* server = fossil_net_create_server("127.0.0.1", 8080, FOSSIL_NET_PROTOCOL_TCP);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Simulate client connection
    fossil_net_client_socket_t* client = fossil_net_accept_client(server);
    ASSUME_NOT_CNULL(client);  // Verify client connection

    const char* messages[] = { "Message 1", "Message 2", "Message 3" };
    for (int i = 0; i < 3; ++i) {
        int bytes_sent = fossil_net_send_to_client(client, messages[i], strlen(messages[i]));
        ASSUME_ITS_EQUAL_I32(strlen(messages[i]), bytes_sent);  // Verify each message was sent

        // Prepare to receive a response
        char buffer[128];
        int bytes_received = fossil_net_receive_from_client(client, buffer, sizeof(buffer));
        ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Ensure data was received
        buffer[bytes_received] = '\0';  // Null-terminate the buffer
        ASSUME_ITS_EQUAL_CSTR(messages[i], buffer);  // Verify the response content
    }

    // Clean up
    fossil_net_destroy_client(client);
    fossil_net_destroy_server(server);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_network_client_tests) {
    ADD_TEST(test_client_connect);           // Test client connection
    ADD_TEST(test_client_send_and_receive);  // Test client sending/receiving messages
    ADD_TEST(test_client_disconnect);        // Test client disconnection
    ADD_TEST(test_client_multiple_messages); // Test handling multiple messages
}
