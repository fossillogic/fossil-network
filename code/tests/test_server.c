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
// * Fossil Logic Squid Network Server Tests
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test server creation and destruction
FOSSIL_TEST(test_server_create_and_destroy) {
    // Create a server socket on port 8080
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Destroy the server and release resources
    fossil_net_destroy_server(server);
}

// Test server accepting and disconnecting a client
FOSSIL_TEST(test_server_accept_and_disconnect_client) {
    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Verify server creation

    // Create a client socket and connect to the server
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation
    fossil_net_client_connect(client, "127.0.0.1", 8080);  // Connect client to server

    // Server accepts the client connection
    fossil_net_client_socket_t accepted_client;
    int32_t accept_status = fossil_net_accept_client(server, &accepted_client);
    ASSUME_ITS_EQUAL_I32(0, accept_status);  // Ensure the client was accepted

    // Disconnect the accepted client
    fossil_net_destroy_client(&accepted_client); // Cleanup accepted client

    // Clean up client and server sockets
    fossil_net_destroy_client(client);
    fossil_net_destroy_server(server);
}

// Test server receive and send operations
FOSSIL_TEST(test_server_receive_and_send) {
    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Verify server creation

    // Create and connect the client socket to the server
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Verify client creation
    fossil_net_client_connect(client, "127.0.0.1", 8080);  // Connect client to server

    // Server accepts the client connection
    fossil_net_client_socket_t accepted_client;
    int32_t accept_status = fossil_net_accept_client(server, &accepted_client);
    ASSUME_ITS_EQUAL_I32(0, accept_status);  // Ensure the client was accepted

    // Client sends a message to the server
    const char *message = "Hello Server";
    int bytes_sent = fossil_net_send_client(client, message, strlen(message));
    ASSUME_ITS_EQUAL_I32(strlen(message), bytes_sent);  // Verify the message was sent

    // Server receives the message
    char buffer[128];
    int bytes_received = fossil_net_receive_client(&accepted_client, buffer, sizeof(buffer));
    ASSUME_ITS_MORE_THAN_I32(0, bytes_received);  // Ensure some bytes were received
    buffer[bytes_received] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR("Hello Server", buffer);  // Verify the message content

    // Server sends a response to the client
    const char *response = "Hello Client";
    bytes_sent = fossil_net_send_client(&accepted_client, response, strlen(response));
    ASSUME_ITS_EQUAL_I32(strlen(response), bytes_sent);  // Verify the response was sent

    // Clean up sockets
    fossil_net_destroy_client(&accepted_client);
    fossil_net_destroy_client(client);
    fossil_net_destroy_server(server);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_network_server_tests) {
    ADD_TEST(test_server_create_and_destroy);             // Test for server creation/destruction
    ADD_TEST(test_server_accept_and_disconnect_client);   // Test for client acceptance/disconnection
    ADD_TEST(test_server_receive_and_send);               // Test for message send/receive operations
}
