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
#ifndef FOSSIL_NETWORK_DNS_H
#define FOSSIL_NETWORK_DNS_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Resolve a hostname into IPv4/IPv6 addresses.
 *
 * @param host Hostname string.
 * @param addrs Output array of strings (caller frees).
 * @param max_addrs Maximum addresses to retrieve.
 * @return Number of addresses resolved, or -1 on error.
 */
int fossil_network_dns_resolve(const char *host,
                               char addrs[][64],
                               int max_addrs);

#ifdef __cplusplus
}
#include <string>
#include <vector>
#include <memory>

namespace fossil {

namespace network {

    class Dns {
    public:
        /**
         * @brief Resolve a hostname into IPv4/IPv6 addresses.
         *
         * @param host Hostname string.
         * @param addrs Output array of strings (caller frees).
         * @param max_addrs Maximum addresses to retrieve.
         * @return Number of addresses resolved, or -1 on error.
         */
        static int resolve(const std::string &host, std::vector<std::string> &addrs) {
            char addr_buf[64][64];
            int num_addrs = fossil_network_dns_resolve(host.c_str(), addr_buf, 64);
            addrs.clear();
            if (num_addrs > 0) {
                for (int i = 0; i < num_addrs; ++i) {
                    addrs.emplace_back(addr_buf[i]);
                }
            }
            return num_addrs;
        }
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_DNS_H */
