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
#include "fossil/network/network.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#ifdef _WIN32
// Windows specifics
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <netdb.h>
#endif

int fossil_strcasecmp(const char *s1, const char *s2) {
    if (!s1 && !s2) return 0;
    if (!s1) return -1;
    if (!s2) return 1;

    while (*s1 && *s2) {
        int c1 = tolower((unsigned char)*s1);
        int c2 = tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        ++s1;
        ++s2;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

// -----------------------------------------
// Protocol lookup table
// -----------------------------------------
typedef struct {
    const char *name;
    fossil_protocol_t proto;
} proto_entry_t;

static const proto_entry_t proto_table[] = {
    {"tcp",   FOSSIL_PROTO_TCP},
    {"udp",   FOSSIL_PROTO_UDP},
    {"raw",   FOSSIL_PROTO_RAW},
    {"icmp",  FOSSIL_PROTO_ICMP},
    {"sctp",  FOSSIL_PROTO_SCTP},
    {"http",  FOSSIL_PROTO_HTTP},
    {"https", FOSSIL_PROTO_HTTPS},
    {"ftp",   FOSSIL_PROTO_FTP},
    {"ssh",   FOSSIL_PROTO_SSH},
    {"dns",   FOSSIL_PROTO_DNS},
    {"ntp",   FOSSIL_PROTO_NTP},
    {"smtp",  FOSSIL_PROTO_SMTP},
    {"pop3",  FOSSIL_PROTO_POP3},
    {"imap",  FOSSIL_PROTO_IMAP},
    {"ldap",  FOSSIL_PROTO_LDAP},
    {"mqtt",  FOSSIL_PROTO_MQTT},
    {NULL,    FOSSIL_PROTO_UNKNOWN}
};

// -----------------------------------------
// Init / Cleanup (Windows only)
// -----------------------------------------
int fossil_network_socket_init(void) {
#ifdef _WIN32
    WSADATA wsa;
    return (WSAStartup(MAKEWORD(2,2), &wsa) == 0) ? 0 : -1;
#else
    return 0; // no-op for POSIX
#endif
}

int fossil_network_socket_cleanup(void) {
#ifdef _WIN32
    return WSACleanup() == 0 ? 0 : -1;
#else
    return 0; // no-op for POSIX
#endif
}

// -----------------------------------------
// Protocol utilities
// -----------------------------------------
fossil_protocol_t fossil_network_socket_proto_from_name(const char *name) {
    if (!name) return FOSSIL_PROTO_UNKNOWN;
    for (const proto_entry_t *p = proto_table; p->name; ++p) {
#ifdef _WIN32
        if (_stricmp(name, p->name) == 0)
#else
        if (fossil_strcasecmp(name, p->name) == 0)
#endif
            return p->proto;
    }
    return FOSSIL_PROTO_UNKNOWN;
}

const char *fossil_network_socket_proto_to_name(fossil_protocol_t proto) {
    for (const proto_entry_t *p = proto_table; p->name; ++p) {
        if (p->proto == proto) return p->name;
    }
    return "unknown";
}

// -----------------------------------------
// Socket creation
// -----------------------------------------
int fossil_network_socket_create(fossil_network_socket_t *sock,
                                 int family, fossil_protocol_t proto) {
    if (!sock) return -1;

    int type = 0;
    int sys_proto = 0;

    switch (proto) {
        case FOSSIL_PROTO_TCP:   type = SOCK_STREAM; sys_proto = IPPROTO_TCP; break;
        case FOSSIL_PROTO_UDP:   type = SOCK_DGRAM;  sys_proto = IPPROTO_UDP; break;
        case FOSSIL_PROTO_RAW:   type = SOCK_RAW;    sys_proto = IPPROTO_RAW; break;
        case FOSSIL_PROTO_ICMP:  type = SOCK_RAW;    sys_proto = IPPROTO_ICMP; break;
#ifdef IPPROTO_SCTP
        case FOSSIL_PROTO_SCTP:  type = SOCK_STREAM; sys_proto = IPPROTO_SCTP; break;
#endif
        // Higher-level protocols just use TCP
        default: type = SOCK_STREAM; sys_proto = IPPROTO_TCP; break;
    }

    fossil_socket_fd_t fd = socket(family, type, sys_proto);
#ifdef _WIN32
    if (fd == INVALID_SOCKET) return -1;
#else
    if (fd < 0) return -1;
#endif

    sock->fd = fd;
    sock->family = family;
    sock->type = type;
    sock->proto = proto;
    return 0;
}

// -----------------------------------------
// Lifecycle wrappers
// -----------------------------------------
int fossil_network_socket_bind(fossil_network_socket_t *sock,
                               const char *address, uint16_t port) {
    if (!sock) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = sock->family;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    return bind(sock->fd, (struct sockaddr*)&addr, sizeof(addr));
}

int fossil_network_socket_listen(fossil_network_socket_t *sock, int backlog) {
    if (!sock) return -1;
    return listen(sock->fd, backlog);
}

int fossil_network_socket_accept(fossil_network_socket_t *server,
                                 fossil_network_socket_t *client) {
    if (!server || !client) return -1;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);

    fossil_socket_fd_t fd = accept(server->fd, (struct sockaddr*)&addr, &len);
#ifdef _WIN32
    if (fd == INVALID_SOCKET) return -1;
#else
    if (fd < 0) return -1;
#endif

    client->fd = fd;
    client->family = addr.sin_family;
    client->type = server->type;
    client->proto = server->proto;
    return 0;
}

int fossil_network_socket_connect(fossil_network_socket_t *sock,
                                  const char *address, uint16_t port) {
    if (!sock) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = sock->family;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    return connect(sock->fd, (struct sockaddr*)&addr, sizeof(addr));
}

int fossil_network_socket_close(fossil_network_socket_t *sock) {
    if (!sock) return -1;
#ifdef _WIN32
    closesocket(sock->fd);
#else
    close(sock->fd);
#endif
    sock->fd = -1;
    return 0;
}

// -----------------------------------------
// Data transmission
// -----------------------------------------
ssize_t fossil_network_socket_send(fossil_network_socket_t *sock,
                                   const void *buffer, size_t length) {
    if (!sock) return -1;
#ifdef _WIN32
    return send(sock->fd, (const char*)buffer, (int)length, 0);
#else
    return send(sock->fd, buffer, length, 0);
#endif
}

ssize_t fossil_network_socket_recv(fossil_network_socket_t *sock,
                                   void *buffer, size_t length) {
    if (!sock) return -1;
#ifdef _WIN32
    return recv(sock->fd, (char*)buffer, (int)length, 0);
#else
    return recv(sock->fd, buffer, length, 0);
#endif
}

// -----------------------------------------
// Convenience: open by name + connect
// -----------------------------------------
int fossil_network_socket_open(fossil_network_socket_t *sock,
                               const char *proto_name,
                               const char *address, uint16_t port) {
    fossil_protocol_t proto = fossil_network_socket_proto_from_name(proto_name);
    if (proto == FOSSIL_PROTO_UNKNOWN) return -1;

    if (fossil_network_socket_create(sock, AF_INET, proto) < 0) return -1;
    if (fossil_network_socket_connect(sock, address, port) < 0) {
        fossil_network_socket_close(sock);
        return -1;
    }
    return 0;
}
