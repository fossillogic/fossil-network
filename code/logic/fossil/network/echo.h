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
#ifndef FOSSIL_NETWORK_ECHO_H
#define FOSSIL_NETWORK_ECHO_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Run a simple echo server for testing.
 *
 * @param port TCP port to bind.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_test_echo_server(uint16_t port);

/**
 * @brief Connect to an echo server and run test exchange.
 */
int fossil_network_test_echo_client(const char *host, uint16_t port);

#ifdef __cplusplus
}

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_ECHO_H */
