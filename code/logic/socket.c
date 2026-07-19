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
 * Date: 04/05/2013
 *
 * Copyright (C) 2013-Current Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/network/socket.h"

#if defined(__APPLE__)
// Must define this **before including any headers** to get getloadavg
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif
#endif

#if defined(_WIN32)
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <process.h>
#include <ws2tcpip.h> // Required for struct addrinfo and getaddrinfo on Windows
#else
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#if defined(__linux__)
#include <netpacket/packet.h>
#endif
#endif

#include <string.h>
#include <stdio.h>

/*=============================================================================
ERROR HANDLING
=============================================================================*/

int fossil_net_socket_error_last(void) {
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

const char* fossil_net_socket_error_string(int err) {
#if defined(_WIN32)
    static char buf[128];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err, 0, buf, sizeof(buf), NULL);
    return buf;
#else
    return strerror(err);
#endif
}

/*=============================================================================
LIFECYCLE
=============================================================================*/

int fossil_net_socket_init(void) {
#if defined(_WIN32)
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) return -1;
#endif
    return 0;
}

int fossil_net_socket_shutdown(void) {
#if defined(_WIN32)
    WSACleanup();
#endif
    return 0;
}

/*=============================================================================
SOCKET MANAGEMENT
=============================================================================*/

static int family_from_string(const char *family) {
    if (!strcmp(family, "ipv4")) return AF_INET;
    if (!strcmp(family, "ipv6")) return AF_INET6;
    return AF_UNSPEC;
}

static int type_from_string(const char *type) {
    if (!strcmp(type, "tcp")) return SOCK_STREAM;
    if (!strcmp(type, "udp")) return SOCK_DGRAM;
    if (!strcmp(type, "raw")) return SOCK_RAW;
    return SOCK_STREAM;
}

int fossil_net_socket_create(fossil_net_socket_t *sock, const char *type, const char *family) {
    if (!sock) return -1;
    memset(sock, 0, sizeof(*sock));

    int af   = family_from_string(family);
    int stype = type_from_string(type);

#if defined(_WIN32)
    sock->handle = (void*)socket(af, stype, 0);
    if ((SOCKET)sock->handle == INVALID_SOCKET) return -1;
#else
    sock->handle = (void*)(intptr_t)socket(af, stype, 0);
    if ((intptr_t)sock->handle < 0) return -1;
#endif

    strncpy(sock->type, type, sizeof(sock->type)-1);
    strncpy(sock->family, family, sizeof(sock->family)-1);
    sock->blocking = true;

    return 0;
}

int fossil_net_socket_close(fossil_net_socket_t *sock) {
    if (!sock) return -1;
#if defined(_WIN32)
    closesocket((SOCKET)sock->handle);
#else
    close((int)(intptr_t)sock->handle);
#endif
    sock->handle = NULL;
    return 0;
}

int fossil_net_socket_set_blocking(fossil_net_socket_t *sock, bool blocking) {
    if (!sock) return -1;
#if defined(_WIN32)
    u_long mode = blocking ? 0 : 1;
    ioctlsocket((SOCKET)sock->handle, FIONBIO, &mode);
#else
    int flags = fcntl((int)(intptr_t)sock->handle, F_GETFL, 0);
    if (!blocking) flags |= O_NONBLOCK;
    else flags &= ~O_NONBLOCK;
    fcntl((int)(intptr_t)sock->handle, F_SETFL, flags);
#endif
    sock->blocking = blocking;
    return 0;
}

/*=============================================================================
CONNECTION
=============================================================================*/

int fossil_net_socket_bind(fossil_net_socket_t *sock, const fossil_net_address_t *addr) {
    if (!sock || !addr) return -1;
    struct sockaddr_storage sa;
    socklen_t salen;

    memset(&sa, 0, sizeof(sa));
    if (!strcmp(addr->family, "ipv4")) {
        struct sockaddr_in *s4 = (struct sockaddr_in*)&sa;
        s4->sin_family = AF_INET;
        s4->sin_port = htons(addr->port);
        inet_pton(AF_INET, addr->ip, &s4->sin_addr);
        salen = sizeof(*s4);
    } else if (!strcmp(addr->family, "ipv6")) {
        struct sockaddr_in6 *s6 = (struct sockaddr_in6*)&sa;
        s6->sin6_family = AF_INET6;
        s6->sin6_port = htons(addr->port);
        inet_pton(AF_INET6, addr->ip, &s6->sin6_addr);
        salen = sizeof(*s6);
    } else {
        return -1;
    }

#if defined(_WIN32)
    return bind((SOCKET)sock->handle, (struct sockaddr*)&sa, salen);
#else
    return bind((int)(intptr_t)sock->handle, (struct sockaddr*)&sa, salen);
#endif
}

int fossil_net_socket_listen(fossil_net_socket_t *sock, int backlog) {
    if (!sock) return -1;
#if defined(_WIN32)
    return listen((SOCKET)sock->handle, backlog);
#else
    return listen((int)(intptr_t)sock->handle, backlog);
#endif
}

int fossil_net_socket_accept(fossil_net_socket_t *server, fossil_net_socket_t *client, fossil_net_address_t *addr) {
    if (!server || !client) return -1;

    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

#if defined(_WIN32)
    u_long mode = 1;
    ioctlsocket((SOCKET)server->handle, FIONBIO, &mode); // set non-blocking
    SOCKET s = accept((SOCKET)server->handle, (struct sockaddr*)&sa, &salen);
    mode = server->blocking ? 0 : 1;
    ioctlsocket((SOCKET)server->handle, FIONBIO, &mode); // restore blocking mode
    if (s == INVALID_SOCKET) return -1;
    client->handle = (void*)s;
#else
    int flags = fcntl((int)(intptr_t)server->handle, F_GETFL, 0);
    fcntl((int)(intptr_t)server->handle, F_SETFL, flags | O_NONBLOCK); // set non-blocking
    int s = accept((int)(intptr_t)server->handle, (struct sockaddr*)&sa, &salen);
    fcntl((int)(intptr_t)server->handle, F_SETFL, flags); // restore original flags
    if (s < 0) return -1;
    client->handle = (void*)(intptr_t)s;
#endif

    if (addr) {
        memset(addr, 0, sizeof(*addr));
        if (sa.ss_family == AF_INET) {
            struct sockaddr_in *s4 = (struct sockaddr_in*)&sa;
            inet_ntop(AF_INET, &s4->sin_addr, addr->ip, sizeof(addr->ip));
            addr->port = ntohs(s4->sin_port);
            strncpy(addr->family, "ipv4", sizeof(addr->family)-1);
        }
#if defined(AF_INET6)
        else if (sa.ss_family == AF_INET6) {
            struct sockaddr_in6 *s6 = (struct sockaddr_in6*)&sa;
            inet_ntop(AF_INET6, &s6->sin6_addr, addr->ip, sizeof(addr->ip));
            addr->port = ntohs(s6->sin6_port);
            strncpy(addr->family, "ipv6", sizeof(addr->family)-1);
        }
#endif
    }
    return 0;
}

int fossil_net_socket_connect(fossil_net_socket_t *sock, const fossil_net_address_t *addr) {
    if (!sock || !addr) return -1;
    struct sockaddr_storage sa;
    socklen_t salen;

    memset(&sa, 0, sizeof(sa));
    if (!strcmp(addr->family, "ipv4")) {
        struct sockaddr_in *s4 = (struct sockaddr_in*)&sa;
        s4->sin_family = AF_INET;
        s4->sin_port = htons(addr->port);
        inet_pton(AF_INET, addr->ip, &s4->sin_addr);
        salen = sizeof(*s4);
    } else if (!strcmp(addr->family, "ipv6")) {
        struct sockaddr_in6 *s6 = (struct sockaddr_in6*)&sa;
        s6->sin6_family = AF_INET6;
        s6->sin6_port = htons(addr->port);
        inet_pton(AF_INET6, addr->ip, &s6->sin6_addr);
        salen = sizeof(*s6);
    } else {
        return -1;
    }

#if defined(_WIN32)
    return connect((SOCKET)sock->handle, (struct sockaddr*)&sa, salen);
#else
    return connect((int)(intptr_t)sock->handle, (struct sockaddr*)&sa, salen);
#endif
}

/*=============================================================================
DATA TRANSFER
=============================================================================*/

int fossil_net_socket_send(fossil_net_socket_t *sock, const void *data, uint32_t size, uint32_t *sent) {
    if (!sock || !data) return -1;
#if defined(_WIN32)
    int s = send((SOCKET)sock->handle, (const char*)data, size, 0);
#else
    int s = send((int)(intptr_t)sock->handle, data, size, 0);
#endif
    if (sent) *sent = s < 0 ? 0 : (uint32_t)s;
    return s < 0 ? -1 : 0;
}

int fossil_net_socket_receive(fossil_net_socket_t *sock, void *buffer, uint32_t size, uint32_t *received) {
    if (!sock || !buffer) return -1;
#if defined(_WIN32)
    int r = recv((SOCKET)sock->handle, (char*)buffer, size, 0);
#else
    int r = recv((int)(intptr_t)sock->handle, buffer, size, 0);
#endif
    if (received) *received = r < 0 ? 0 : (uint32_t)r;
    return r < 0 ? -1 : 0;
}

/*=============================================================================
ADDRESS UTILITIES
=============================================================================*/

/**
 * @brief Parse an IP address and port into an address structure.
 *
 * Converts a string IP and port number into a fossil_net_address_t structure.
 *
 * @param addr Pointer to address structure to fill.
 * @param ip   String representation of IP address.
 * @param port Port number.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_address_parse(
    fossil_net_address_t *addr,
    const char *ip,
    uint16_t port)
{
    if (!addr || !ip) return -1;
    memset(addr, 0, sizeof(*addr));
    strncpy(addr->ip, ip, sizeof(addr->ip) - 1);
    addr->port = port;

    // Try IPv4 first
    struct in_addr ipv4;
    if (inet_pton(AF_INET, ip, &ipv4) == 1) {
        strncpy(addr->family, "ipv4", sizeof(addr->family) - 1);
        return 0;
    }
#if defined(AF_INET6)
    struct in6_addr ipv6;
    if (inet_pton(AF_INET6, ip, &ipv6) == 1) {
        strncpy(addr->family, "ipv6", sizeof(addr->family) - 1);
        return 0;
    }
#endif
    return -1;
}

int fossil_net_socket_address_to_string(
    const fossil_net_address_t *addr,
    char *buffer,
    uint32_t size)
{
    if (!addr || !buffer || size == 0) return -1;

    // Prefer addr->addr if set, otherwise use addr->ip
    const char *ipstr = addr->addr[0] ? addr->addr : addr->ip;

    if (!strcmp(addr->family, "ipv4")) {
        // IPv4: "x.x.x.x:port"
        int n = snprintf(buffer, size, "%s:%u", ipstr, addr->port);
        if (n < 0 || (uint32_t)n >= size) return -1;
        return 0;
    }
#if defined(AF_INET6)
    else if (!strcmp(addr->family, "ipv6")) {
        // IPv6: "[addr]:port"
        int n = snprintf(buffer, size, "[%s]:%u", ipstr, addr->port);
        if (n < 0 || (uint32_t)n >= size) return -1;
        return 0;
    }
#endif
    return -1;
}

int fossil_net_socket_get_local_address(
    fossil_net_socket_t *sock,
    fossil_net_address_t *addr)
{
    if (!sock || !addr) return -1;

    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    memset(&sa, 0, sizeof(sa));

#if defined(_WIN32)
    int fd = (int)(intptr_t)sock->handle;
#else
    int fd = (int)(intptr_t)sock->handle;
#endif

    if (getsockname(fd, (struct sockaddr *)&sa, &salen) != 0)
        return -1;

    memset(addr, 0, sizeof(*addr));
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s4 = (struct sockaddr_in *)&sa;
        inet_ntop(AF_INET, &s4->sin_addr, addr->ip, sizeof(addr->ip));
        inet_ntop(AF_INET, &s4->sin_addr, addr->addr, sizeof(addr->addr));
        addr->port = ntohs(s4->sin_port);
        strncpy(addr->family, "ipv4", sizeof(addr->family) - 1);
        return 0;
    }
#if defined(AF_INET6)
    else if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)&sa;
        inet_ntop(AF_INET6, &s6->sin6_addr, addr->ip, sizeof(addr->ip));
        inet_ntop(AF_INET6, &s6->sin6_addr, addr->addr, sizeof(addr->addr));
        addr->port = ntohs(s6->sin6_port);
        strncpy(addr->family, "ipv6", sizeof(addr->family) - 1);
        return 0;
    }
#endif
    return -1;
}

/*=============================================================================
UTILITY
=============================================================================*/

int fossil_net_socket_sleep(uint32_t ms) {
#if defined(_WIN32)
    Sleep(ms);
#else
    sleep(ms*1000);
#endif
    return 0;
}

/*=============================================================================
MAC ADDRESS
=============================================================================*/

int fossil_net_socket_mac_get(fossil_net_mac_t *mac) {
    if (!mac) return -1;
#if defined(_WIN32)
    IP_ADAPTER_ADDRESSES *adapters = NULL;
    ULONG outBufLen = 0;
    if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapters, &outBufLen) != ERROR_BUFFER_OVERFLOW) return -1;
    adapters = (IP_ADAPTER_ADDRESSES*)malloc(outBufLen);
    if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapters, &outBufLen) != 0) { free(adapters); return -1; }
    memcpy(mac->bytes, adapters->PhysicalAddress, 6);
    sprintf(mac->string,"%02X:%02X:%02X:%02X:%02X:%02X",
        mac->bytes[0],mac->bytes[1],mac->bytes[2],mac->bytes[3],mac->bytes[4],mac->bytes[5]);
    free(adapters);
#else
#if defined(__linux__)
    struct ifaddrs *ifap, *ifa;
    if (getifaddrs(&ifap) != 0) return -1;
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || ifa->ifa_addr->sa_family != AF_PACKET) continue;
        struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
        if (s->sll_halen == 6) {
            memcpy(mac->bytes, s->sll_addr, 6);
            sprintf(mac->string,"%02X:%02X:%02X:%02X:%02X:%02X",
                mac->bytes[0],mac->bytes[1],mac->bytes[2],mac->bytes[3],mac->bytes[4],mac->bytes[5]);
            break;
        }
    }
    freeifaddrs(ifap);
    return 0;
#else
    // Not supported on this platform
    return -1;
#endif
#endif
    return 0;
}

int fossil_net_socket_mac_to_string(const fossil_net_mac_t *mac, char *buffer, uint32_t size) {
    if (!mac || !buffer) return -1;
    strncpy(buffer, mac->string, size-1);
    buffer[size-1] = 0;
    return 0;
}

int fossil_net_socket_mac_parse(
    fossil_net_mac_t *mac,
    const char *string)
{
    if (!mac || !string) return -1;
    unsigned int b[6];
    int n = sscanf(string, "%2x:%2x:%2x:%2x:%2x:%2x",
                   &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]);
    if (n != 6) return -1;
    for (int i = 0; i < 6; ++i)
        mac->bytes[i] = (uint8_t)b[i];
    snprintf(mac->string, sizeof(mac->string),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac->bytes[0], mac->bytes[1], mac->bytes[2],
             mac->bytes[3], mac->bytes[4], mac->bytes[5]);
    return 0;
}

bool fossil_net_socket_mac_equal(
    const fossil_net_mac_t *mac1,
    const fossil_net_mac_t *mac2)
{
    if (!mac1 || !mac2) return false;
    return memcmp(mac1->bytes, mac2->bytes, 6) == 0;
}

/*=============================================================================
DNS / HOST
=============================================================================*/

int fossil_net_socket_resolve(const char *hostname, fossil_net_address_t *out_addr) {
    if (!hostname || !out_addr) return -1;

    memset(out_addr, 0, sizeof(*out_addr));

    // Try IPv4 first
    struct in_addr ipv4;
    if (inet_pton(AF_INET, hostname, &ipv4) == 1) {
        strncpy(out_addr->ip, hostname, sizeof(out_addr->ip) - 1);
        strncpy(out_addr->addr, hostname, sizeof(out_addr->addr) - 1);
        out_addr->port = 0;
        strncpy(out_addr->family, "ipv4", sizeof(out_addr->family) - 1);
        return 0;
    }
#if defined(AF_INET6)
    struct in6_addr ipv6;
    if (inet_pton(AF_INET6, hostname, &ipv6) == 1) {
        strncpy(out_addr->ip, hostname, sizeof(out_addr->ip) - 1);
        strncpy(out_addr->addr, hostname, sizeof(out_addr->addr) - 1);
        out_addr->port = 0;
        strncpy(out_addr->family, "ipv6", sizeof(out_addr->family) - 1);
        return 0;
    }
#endif

    // Fallback: use gethostbyname (IPv4 only, legacy)
    struct hostent *he = gethostbyname(hostname);
    if (he && he->h_addrtype == AF_INET && he->h_length == 4) {
        struct in_addr *addr_in = (struct in_addr *)he->h_addr_list[0];
        if (addr_in) {
            inet_ntop(AF_INET, addr_in, out_addr->ip, sizeof(out_addr->ip));
            strncpy(out_addr->addr, out_addr->ip, sizeof(out_addr->addr) - 1);
            out_addr->addr[sizeof(out_addr->addr) - 1] = '\0';
            out_addr->port = 0;
            strncpy(out_addr->family, "ipv4", sizeof(out_addr->family) - 1);
            return 0;
        }
    }

    // No IPv6 fallback for gethostbyname; gethostbyname2 is not portable

    return -1;
}

int fossil_net_socket_hostname(char *buffer, uint32_t size) {
    if (!buffer) return -1;
    if (gethostname(buffer, size) != 0) return -1;
    buffer[size-1] = 0;
    return 0;
}

//
int fossil_net_socket_set_reuseaddr(
    fossil_net_socket_t *sock,
    bool enabled)
{
    if (!sock) return -1;
    int optval = enabled ? 1 : 0;
#if defined(_WIN32)
    if (setsockopt((SOCKET)sock->handle, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval)) != 0)
        return -1;
#else
    if (setsockopt((int)(intptr_t)sock->handle, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
        return -1;
#endif
    return 0;
}

int fossil_net_socket_get_peer_address(
    fossil_net_socket_t *sock,
    fossil_net_address_t *addr)
{
    if (!sock || !addr) return -1;

    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    memset(&sa, 0, sizeof(sa));

#if defined(_WIN32)
    SOCKET fd = (SOCKET)sock->handle;
#else
    int fd = (int)(intptr_t)sock->handle;
#endif

    if (getpeername(fd, (struct sockaddr *)&sa, &salen) != 0)
        return -1;

    memset(addr, 0, sizeof(*addr));
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in *s4 = (struct sockaddr_in *)&sa;
        inet_ntop(AF_INET, &s4->sin_addr, addr->ip, sizeof(addr->ip));
        inet_ntop(AF_INET, &s4->sin_addr, addr->addr, sizeof(addr->addr));
        addr->port = ntohs(s4->sin_port);
        strncpy(addr->family, "ipv4", sizeof(addr->family) - 1);
        return 0;
    }
#if defined(AF_INET6)
    else if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)&sa;
        inet_ntop(AF_INET6, &s6->sin6_addr, addr->ip, sizeof(addr->ip));
        inet_ntop(AF_INET6, &s6->sin6_addr, addr->addr, sizeof(addr->addr));
        addr->port = ntohs(s6->sin6_port);
        strncpy(addr->family, "ipv6", sizeof(addr->family) - 1);
        return 0;
    }
#endif
    return -1;
}

/*=============================================================================
POLLING / MULTIPLEXING
=============================================================================*/

int fossil_net_socket_poll(fossil_net_socket_t **sockets, uint32_t count, uint32_t timeout_ms) {
    if (!sockets || count == 0) return -1;

#if defined(_WIN32)
    fd_set readfds;
    FD_ZERO(&readfds);
    SOCKET max_fd = 0;
    for (uint32_t i=0;i<count;i++) {
        FD_SET((SOCKET)sockets[i]->handle,&readfds);
        if ((SOCKET)sockets[i]->handle>max_fd) max_fd = (SOCKET)sockets[i]->handle;
    }
    struct timeval tv = {timeout_ms/1000, (timeout_ms%1000)*1000};
    int r = select(max_fd+1, &readfds, NULL, NULL, &tv);
    return r;
#else
    fd_set readfds;
    FD_ZERO(&readfds);
    int max_fd = 0;
    for (uint32_t i=0;i<count;i++) {
        int fd = (int)(intptr_t)sockets[i]->handle;
        FD_SET(fd,&readfds);
        if (fd>max_fd) max_fd = fd;
    }
    struct timeval tv = {timeout_ms/1000, (timeout_ms%1000)*1000};
    int r = select(max_fd+1, &readfds, NULL, NULL, &tv);
    return r;
#endif
}

//
int fossil_net_socket_poll_add(
    fossil_net_socket_t **poll_set,
    uint32_t *count,
    uint32_t max,
    fossil_net_socket_t *sock)
{
    if (!poll_set || !count || !sock) return -1;
    if (*count >= max) return -1;
    poll_set[*count] = sock;
    (*count)++;
    return 0;
}

int fossil_net_socket_poll_remove(
    fossil_net_socket_t **poll_set,
    uint32_t *count,
    fossil_net_socket_t *sock)
{
    if (!poll_set || !count || !sock) return -1;
    for (uint32_t i = 0; i < *count; ++i) {
        if (poll_set[i] == sock) {
            for (uint32_t j = i; j < *count - 1; ++j) {
                poll_set[j] = poll_set[j + 1];
            }
            (*count)--;
            return 0;
        }
    }
    return -1;
}

int fossil_net_socket_poll_clear(
    fossil_net_socket_t **poll_set,
    uint32_t *count)
{
    if (!poll_set || !count) return -1;
    *count = 0;
    return 0;
}
