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

#include <string>

namespace fossil {

namespace network {

    class Echo {
    public:
        /**
         * @brief Run a simple echo server for testing.
         */
        static int server(uint16_t port) {
            return fossil_network_test_echo_server(port);
        }

        /**
         * @brief Connect to an echo server and run test exchange.
         */
        static int client(const std::string &host, uint16_t port) {
            return fossil_network_test_echo_client(host.c_str(), port);
        }
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_ECHO_H */
