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
#include "fossil/network/dns.h"


int fossil_network_dns_resolve(const char *host,
                               char addrs[][64],
                               int max_addrs) {
    if (!host || !addrs || max_addrs <= 0) {
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *res = NULL, *p;
    int count = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // doesn't matter, just want addresses

    int rv = getaddrinfo(host, NULL, &hints, &res);
    if (rv != 0) {
#ifdef _WIN32
        fprintf(stderr, "DNS resolve failed for %s: %d\n", host, WSAGetLastError());
#else
        fprintf(stderr, "DNS resolve failed for %s: %s\n", host, gai_strerror(rv));
#endif
        return -1;
    }

    for (p = res; p != NULL && count < max_addrs; p = p->ai_next) {
        void *addr_ptr = NULL;

        if (p->ai_family == AF_INET) {
            // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr_ptr = &(ipv4->sin_addr);
        } else if (p->ai_family == AF_INET6) {
            // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr_ptr = &(ipv6->sin6_addr);
        }

        if (addr_ptr) {
            if (inet_ntop(p->ai_family, addr_ptr, addrs[count], 64) != NULL) {
                count++;
            }
        }
    }

    freeaddrinfo(res);
    return count; // number of resolved addresses
}
