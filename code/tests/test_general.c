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
// * Fossil Logic Network General Usage Tests
// * * * * * * * * * * * * * * * * * * * * * * * *

// Test creating a server and connecting multiple clients
FOSSIL_TEST(test_server_multiple_clients) {
    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Create and connect multiple clients
    const int num_clients = 5;
    fossil_net_client_socket_t *clients[num_clients];
    for (int i = 0; i < num_clients; ++i) {
        clients[i] = fossil_net_create_client("127.0.0.1", 8080);
        ASSUME_NOT_CNULL(clients[i]);  // Ensure each client was created successfully
        int connect_status = fossil_net_client_connect(clients[i], "127.0.0.1", 8080);
        ASSUME_ITS_EQUAL_I32(0, connect_status);  // Ensure each client connected successfully
    }

    // Accept all clients on the server
    for (int i = 0; i < num_clients; ++i) {
        fossil_net_client_socket_t accepted_client;
        int32_t accept_status = fossil_net_accept_client(server, &accepted_client);
        ASSUME_ITS_EQUAL_I32(0, accept_status);  // Ensure each client was accepted
        fossil_net_close_client(&accepted_client); // Disconnect accepted client
    }

    // Clean up clients and server
    for (int i = 0; i < num_clients; ++i) {
        fossil_net_close_client(clients[i]);
    }
    fossil_net_destroy_server(server);
}

// Test sending and receiving messages from multiple clients
FOSSIL_TEST(test_multiple_clients_communication) {
    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Create and connect two clients
    fossil_net_client_socket_t *client1 = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client1);  // Ensure client1 was created successfully
    fossil_net_client_connect(client1, "127.0.0.1", 8080);  // Connect client1

    fossil_net_client_socket_t *client2 = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client2);  // Ensure client2 was created successfully
    fossil_net_client_connect(client2, "127.0.0.1", 8080);  // Connect client2

    // Server accepts both clients
    fossil_net_client_socket_t accepted_client1;
    fossil_net_client_socket_t accepted_client2;
    ASSUME_ITS_EQUAL_I32(0, fossil_net_accept_client(server, &accepted_client1));
    ASSUME_ITS_EQUAL_I32(0, fossil_net_accept_client(server, &accepted_client2));

    // Client 1 sends a message to the server
    const char *message1 = "Hello from Client 1";
    fossil_net_send_client(client1, message1, strlen(message1));

    // Client 2 sends a message to the server
    const char *message2 = "Hello from Client 2";
    fossil_net_send_client(client2, message2, strlen(message2));

    // Server receives messages from both clients
    char buffer[128];

    // Receiving message from Client 1
    int bytes_received1 = fossil_net_receive_client(&accepted_client1, buffer, sizeof(buffer));
    buffer[bytes_received1] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR(message1, buffer);  // Verify the message content

    // Receiving message from Client 2
    int bytes_received2 = fossil_net_receive_client(&accepted_client2, buffer, sizeof(buffer));
    buffer[bytes_received2] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR(message2, buffer);  // Verify the message content

    // Clean up
    fossil_net_close_client(&accepted_client1);
    fossil_net_close_client(&accepted_client2);
    fossil_net_close_client(client1);
    fossil_net_close_client(client2);
    fossil_net_destroy_server(server);
}

// Test server response to client request
FOSSIL_TEST(test_server_response) {
    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Create and connect a client
    fossil_net_client_socket_t *client = fossil_net_create_client("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(client);  // Ensure client was created successfully
    fossil_net_client_connect(client, "127.0.0.1", 8080);  // Connect client

    // Server accepts the client connection
    fossil_net_client_socket_t accepted_client;
    ASSUME_ITS_EQUAL_I32(0, fossil_net_accept_client(server, &accepted_client));  // Accept client

    // Client sends a request message
    const char *request = "Request Data";
    fossil_net_send_client(client, request, strlen(request));

    // Server receives the request and sends a response
    const char *response = "Response Data";
    fossil_net_send_client(&accepted_client, response, strlen(response));

    // Client receives the response
    char buffer[128];
    int bytes_received = fossil_net_receive_client(client, buffer, sizeof(buffer));
    buffer[bytes_received] = '\0';  // Null-terminate the buffer
    ASSUME_ITS_EQUAL_CSTR(response, buffer);  // Verify the response content

    // Clean up
    fossil_net_close_client(&accepted_client);
    fossil_net_close_client(client);
    fossil_net_destroy_server(server);
}

// Test server error handling for invalid connections
FOSSIL_TEST(test_invalid_connection_handling) {
    // Attempt to create an invalid client
    fossil_net_client_socket_t *invalid_client = fossil_net_create_client("invalid_ip", 8080);
    ASSUME_CNULL(invalid_client);  // Ensure client creation fails

    // Create the server socket
    fossil_net_server_socket_t *server = fossil_net_create_server("127.0.0.1", 8080);
    ASSUME_NOT_CNULL(server);  // Ensure the server was created successfully

    // Attempt to accept a client without any connected clients
    fossil_net_client_socket_t accepted_client;
    int32_t accept_status = fossil_net_accept_client(server, &accepted_client);
    ASSUME_ITS_NOT_EQUAL_I32(0, accept_status);  // Ensure accepting fails

    // Clean up
    fossil_net_destroy_server(server);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_network_general_usage_tests) {
    ADD_TEST(test_server_multiple_clients);               // Test for server handling multiple clients
    ADD_TEST(test_multiple_clients_communication);        // Test for communication between multiple clients
    ADD_TEST(test_server_response);                        // Test for server responding to client requests
    ADD_TEST(test_invalid_connection_handling);           // Test for handling invalid connections
}
