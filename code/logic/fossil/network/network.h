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
#ifndef FOSSIL_NETWORK_CORE_H
#define FOSSIL_NETWORK_CORE_H

#include <stddef.h>
#include <stdint.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #include <iphlpapi.h>
    typedef SOCKET fossil_socket_fd_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int fossil_socket_fd_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// ------------------------------
// Protocol enumeration
// ------------------------------
typedef enum {
    FOSSIL_PROTO_UNKNOWN = 0,
    FOSSIL_PROTO_TCP,
    FOSSIL_PROTO_UDP,
    FOSSIL_PROTO_RAW,
    FOSSIL_PROTO_ICMP,
    FOSSIL_PROTO_SCTP,
    FOSSIL_PROTO_HTTP,
    FOSSIL_PROTO_HTTPS,
    FOSSIL_PROTO_FTP,
    FOSSIL_PROTO_SSH,
    FOSSIL_PROTO_DNS,
    FOSSIL_PROTO_NTP,
    FOSSIL_PROTO_SMTP,
    FOSSIL_PROTO_POP3,
    FOSSIL_PROTO_IMAP,
    FOSSIL_PROTO_LDAP,
    FOSSIL_PROTO_MQTT
} fossil_protocol_t;

// ------------------------------
// Socket structure
// ------------------------------
typedef struct {
    fossil_socket_fd_t fd;
    int family;
    int type;
    fossil_protocol_t proto;
} fossil_network_socket_t;

// ------------------------------
// Init/Cleanup (Windows only)
// ------------------------------
int fossil_network_socket_init(void);
int fossil_network_socket_cleanup(void);

// ------------------------------
// Protocol utilities
// ------------------------------
fossil_protocol_t fossil_network_socket_proto_from_name(const char *name);
const char *fossil_network_socket_proto_to_name(fossil_protocol_t proto);

// ------------------------------
// Socket lifecycle
// ------------------------------
int fossil_network_socket_create(fossil_network_socket_t *sock,
                                 int family, fossil_protocol_t proto);

int fossil_network_socket_bind(fossil_network_socket_t *sock,
                               const char *address, uint16_t port);

int fossil_network_socket_listen(fossil_network_socket_t *sock, int backlog);

int fossil_network_socket_accept(fossil_network_socket_t *server,
                                 fossil_network_socket_t *client);

int fossil_network_socket_connect(fossil_network_socket_t *sock,
                                  const char *address, uint16_t port);

int fossil_network_socket_close(fossil_network_socket_t *sock);

// ------------------------------
// Data transmission
// ------------------------------
ssize_t fossil_network_socket_send(fossil_network_socket_t *sock,
                                   const void *buffer, size_t length);

ssize_t fossil_network_socket_recv(fossil_network_socket_t *sock,
                                   void *buffer, size_t length);

// ------------------------------
// Convenience
// ------------------------------
int fossil_network_socket_open(fossil_network_socket_t *sock,
                               const char *proto_name,
                               const char *address, uint16_t port);

#ifdef __cplusplus
}
#include <stdexcept>
#include <vector>
#include <string>

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_CORE_H */
