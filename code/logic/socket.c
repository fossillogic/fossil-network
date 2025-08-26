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
#include "fossil/network/socket.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
    #include <BaseTsd.h>
    typedef SSIZE_T ssize_t;  // Windows equivalent
#elif defined(__APPLE__) || defined(__MACH__) || defined(__unix__) || defined(__unix)
    #include <sys/types.h>    // defines ssize_t
#else    
#endif

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #include <windows.h>

    typedef SOCKET fossil_socket_fd_t;

    // ssize_t is POSIX; Windows lacks it
    #if defined(_MSC_VER)
        #include <BaseTsd.h>
        typedef SSIZE_T ssize_t;
    #elif defined(__MINGW32__)
        // MinGW usually has ssize_t, but ensure fallback
        #ifndef _SSIZE_T_DEFINED
            typedef long ssize_t;
            #define _SSIZE_T_DEFINED
        #endif
    #endif

#else
    #define _GNU_SOURCE
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>
    typedef int fossil_socket_fd_t;
#endif

struct fossil_network_socket {
    fossil_socket_fd_t fd;
    int family;
    int type;
    fossil_protocol_t proto;
};


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

// ---------------------------------------------------------
// Socket Options
// ---------------------------------------------------------
int fossil_network_socket_set_option(fossil_network_socket_t *sock,
                                     int level, int option, int value) {
    if (!sock) return -1;

    int ret;
#if defined(_WIN32)
    ret = setsockopt(sock->fd, level, option,
                     (const char*)&value, sizeof(value));
#else
    ret = setsockopt(sock->fd, level, option,
                     &value, sizeof(value));
#endif
    return (ret == 0) ? 0 : -1;
}

int fossil_network_socket_get_option(fossil_network_socket_t *sock,
                                     int level, int option, int *value) {
    if (!sock || !value) return -1;

    socklen_t len = sizeof(*value);
#if defined(_WIN32)
    int ret = getsockopt(sock->fd, level, option,
                         (char*)value, &len);
#else
    int ret = getsockopt(sock->fd, level, option,
                         value, &len);
#endif
    return (ret == 0) ? 0 : -1;
}

// ---------------------------------------------------------
// Blocking / Non-blocking
// ---------------------------------------------------------
int fossil_network_socket_set_nonblocking(fossil_network_socket_t *sock,
                                          int nonblock) {
    if (!sock) return -1;

#if defined(_WIN32)
    u_long mode = nonblock ? 1 : 0;
    return (ioctlsocket(sock->fd, FIONBIO, &mode) == 0) ? 0 : -1;
#else
    int flags = fcntl(sock->fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (nonblock)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    return (fcntl(sock->fd, F_SETFL, flags) == 0) ? 0 : -1;
#endif
}

// ---------------------------------------------------------
// Hostname Resolution
// ---------------------------------------------------------
int fossil_network_socket_resolve_hostname(const char *hostname,
                                           char *ip_buffer,
                                           size_t ip_buffer_len) {
    if (!hostname || !ip_buffer) return -1;

    struct addrinfo hints, *res = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;    // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(hostname, NULL, &hints, &res);
    if (rc != 0 || !res) return -1;

    void *addr = NULL;
    if (res->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)res->ai_addr;
        addr = &(ipv4->sin_addr);
    } else if (res->ai_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)res->ai_addr;
        addr = &(ipv6->sin6_addr);
    }

    if (!addr) {
        freeaddrinfo(res);
        return -1;
    }

    const char *ntop = inet_ntop(res->ai_family, addr,
                                 ip_buffer, (socklen_t)ip_buffer_len);
    freeaddrinfo(res);
    return (ntop != NULL) ? 0 : -1;
}

// ---------------------------------------------------------
// Local/Remote Address
// ---------------------------------------------------------
int fossil_network_socket_get_address(fossil_network_socket_t *sock,
                                      char *buffer, size_t buffer_len,
                                      int remote) {
    if (!sock || !buffer) return -1;

    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    int rc = remote
        ? getpeername(sock->fd, (struct sockaddr*)&addr, &addr_len)
        : getsockname(sock->fd, (struct sockaddr*)&addr, &addr_len);

    if (rc != 0) return -1;

    void *raw_addr = NULL;
    int family = ((struct sockaddr*)&addr)->sa_family;

    if (family == AF_INET) {
        raw_addr = &(((struct sockaddr_in*)&addr)->sin_addr);
    } else if (family == AF_INET6) {
        raw_addr = &(((struct sockaddr_in6*)&addr)->sin6_addr);
    } else {
        return -1;
    }

    return (inet_ntop(family, raw_addr, buffer, (socklen_t)buffer_len) != NULL)
           ? 0 : -1;
}

// ---------------------------------------------------------
// Error Handling
// ---------------------------------------------------------
int fossil_network_socket_last_error(void) {
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

const char *fossil_network_socket_error_string(int err) {
#if defined(_WIN32)
    static char msg[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS,
                   NULL, err,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   msg, sizeof(msg), NULL);
    return msg;
#else
    return strerror(err);
#endif
}

// ---------------------------------------------------------
// Poll Support
// ---------------------------------------------------------
int fossil_network_socket_poll(fossil_network_pollfd_t *fds,
                               size_t nfds, int timeout) {
    if (!fds || nfds == 0) return -1;

#if defined(_WIN32)
    // Map to WSAPOLLFD
    WSAPOLLFD *wsa_fds = (WSAPOLLFD*)calloc(nfds, sizeof(WSAPOLLFD));
    if (!wsa_fds) return -1;

    for (size_t i = 0; i < nfds; i++) {
        wsa_fds[i].fd = fds[i].sock->fd;
        wsa_fds[i].events = fds[i].events;
    }

    int ret = WSAPoll(wsa_fds, (ULONG)nfds, timeout);

    if (ret >= 0) {
        for (size_t i = 0; i < nfds; i++) {
            fds[i].revents = wsa_fds[i].revents;
        }
    }
    free(wsa_fds);
    return ret;
#else
    struct pollfd *pfd = (struct pollfd*)calloc(nfds, sizeof(struct pollfd));
    if (!pfd) return -1;

    for (size_t i = 0; i < nfds; i++) {
        pfd[i].fd = fds[i].sock->fd;
        pfd[i].events = fds[i].events;
    }

    int ret = poll(pfd, nfds, timeout);

    if (ret >= 0) {
        for (size_t i = 0; i < nfds; i++) {
            fds[i].revents = pfd[i].revents;
        }
    }
    free(pfd);
    return ret;
#endif
}

// ------------------------------
// Socket Shutdown
// ------------------------------
int fossil_network_socket_shutdown(fossil_network_socket_t *sock, int how) {
    if (!sock) return -1;

#if defined(_WIN32)
    return (shutdown(sock->fd, how) == 0) ? 0 : -1;
#else
    return (shutdown(sock->fd, how) == 0) ? 0 : -1;
#endif
}

// ------------------------------
// Datagram Support
// ------------------------------
ssize_t fossil_network_socket_sendto(fossil_network_socket_t *sock,
                                     const void *buf, size_t len,
                                     const char *address, uint16_t port) {
    if (!sock || !buf || !address) return -1;

    struct addrinfo hints, *res = NULL;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = sock->family;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(address, port_str, &hints, &res) != 0 || !res) {
        return -1;
    }

    ssize_t sent = sendto(sock->fd, (const char*)buf, (int)len, 0,
                          res->ai_addr, (int)res->ai_addrlen);

    freeaddrinfo(res);
    return sent;
}

ssize_t fossil_network_socket_recvfrom(fossil_network_socket_t *sock,
                                       void *buf, size_t len,
                                       char *address, size_t addr_len,
                                       uint16_t *port) {
    if (!sock || !buf) return -1;

    struct sockaddr_storage src_addr;
    socklen_t src_len = sizeof(src_addr);

    ssize_t recvd = recvfrom(sock->fd, (char*)buf, (int)len, 0,
                             (struct sockaddr*)&src_addr, &src_len);
    if (recvd < 0) return -1;

    if (address) {
        void *raw_addr = NULL;
        if (src_addr.ss_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in*)&src_addr;
            raw_addr = &sin->sin_addr;
            if (port) *port = ntohs(sin->sin_port);
        } else if (src_addr.ss_family == AF_INET6) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6*)&src_addr;
            raw_addr = &sin6->sin6_addr;
            if (port) *port = ntohs(sin6->sin6_port);
        }
        if (raw_addr) {
            inet_ntop(src_addr.ss_family, raw_addr,
                      address, (socklen_t)addr_len);
        }
    }

    return recvd;
}

// ------------------------------
// Timeout Helpers
// ------------------------------
int fossil_network_socket_set_timeout(fossil_network_socket_t *sock,
                                      int send_ms, int recv_ms) {
    if (!sock) return -1;

#if defined(_WIN32)
    DWORD s = (send_ms > 0) ? (DWORD)send_ms : 0;
    DWORD r = (recv_ms > 0) ? (DWORD)recv_ms : 0;
    if (send_ms > 0 &&
        setsockopt(sock->fd, SOL_SOCKET, SO_SNDTIMEO,
                   (const char*)&s, sizeof(s)) != 0)
        return -1;
    if (recv_ms > 0 &&
        setsockopt(sock->fd, SOL_SOCKET, SO_RCVTIMEO,
                   (const char*)&r, sizeof(r)) != 0)
        return -1;
#else
    struct timeval tv;
    if (send_ms > 0) {
        tv.tv_sec = send_ms / 1000;
        tv.tv_usec = (send_ms % 1000) * 1000;
        if (setsockopt(sock->fd, SOL_SOCKET, SO_SNDTIMEO,
                       &tv, sizeof(tv)) != 0)
            return -1;
    }
    if (recv_ms > 0) {
        tv.tv_sec = recv_ms / 1000;
        tv.tv_usec = (recv_ms % 1000) * 1000;
        if (setsockopt(sock->fd, SOL_SOCKET, SO_RCVTIMEO,
                       &tv, sizeof(tv)) != 0)
            return -1;
    }
#endif
    return 0;
}

// ------------------------------
// Single-Socket Wait
// ------------------------------
int fossil_network_socket_wait(fossil_network_socket_t *sock,
                               int events, int timeout) {
    if (!sock) return -1;

#if defined(_WIN32)
    WSAPOLLFD pfd;
    pfd.fd = sock->fd;
    pfd.events = 0;
    if (events & 1) pfd.events |= POLLRDNORM; // readable
    if (events & 2) pfd.events |= POLLWRNORM; // writable
    pfd.revents = 0;

    int ret = WSAPoll(&pfd, 1, timeout);
    if (ret <= 0) return ret;
    int ready = 0;
    if (pfd.revents & (POLLRDNORM | POLLIN)) ready |= 1;
    if (pfd.revents & (POLLWRNORM | POLLOUT)) ready |= 2;
    return ready;
#else
    struct pollfd pfd;
    pfd.fd = sock->fd;
    pfd.events = 0;
    if (events & 1) pfd.events |= POLLIN;
    if (events & 2) pfd.events |= POLLOUT;
    pfd.revents = 0;

    int ret = poll(&pfd, 1, timeout);
    if (ret <= 0) return ret;
    int ready = 0;
    if (pfd.revents & POLLIN) ready |= 1;
    if (pfd.revents & POLLOUT) ready |= 2;
    return ready;
#endif
}

// ------------------------------
// Address Helpers
// ------------------------------
int fossil_network_socket_is_ipv6(fossil_network_socket_t *sock) {
    if (!sock) return -1;
    if (sock->family == AF_INET6) return 1;
    if (sock->family == AF_INET) return 0;
    return -1;
}

// ------------------------------
// Error Translation
// ------------------------------
fossil_network_error_t fossil_network_socket_translate_error(void) {
    int err = fossil_network_socket_last_error();

#if defined(_WIN32)
    switch (err) {
        case WSAEWOULDBLOCK: return FOSSIL_NET_ERR_WOULDBLOCK;
        case WSAECONNRESET:  return FOSSIL_NET_ERR_CONNRESET;
        case WSAETIMEDOUT:   return FOSSIL_NET_ERR_TIMEDOUT;
        case WSAECONNREFUSED:return FOSSIL_NET_ERR_REFUSED;
        case WSAEADDRINUSE:  return FOSSIL_NET_ERR_ADDRINUSE;
        case WSAENETDOWN:    return FOSSIL_NET_ERR_NETDOWN;
        case WSAENETUNREACH: return FOSSIL_NET_ERR_NETUNREACH;
        case WSAEHOSTUNREACH:return FOSSIL_NET_ERR_HOSTUNREACH;
        default:             return FOSSIL_NET_ERR_UNKNOWN;
    }
#else
    switch (err) {
        case EWOULDBLOCK: return FOSSIL_NET_ERR_WOULDBLOCK;
        case ECONNRESET:  return FOSSIL_NET_ERR_CONNRESET;
        case ETIMEDOUT:   return FOSSIL_NET_ERR_TIMEDOUT;
        case ECONNREFUSED:return FOSSIL_NET_ERR_REFUSED;
        case EADDRINUSE:  return FOSSIL_NET_ERR_ADDRINUSE;
        case ENETDOWN:    return FOSSIL_NET_ERR_NETDOWN;
        case ENETUNREACH: return FOSSIL_NET_ERR_NETUNREACH;
        case EHOSTUNREACH:return FOSSIL_NET_ERR_HOSTUNREACH;
        default:          return FOSSIL_NET_ERR_UNKNOWN;
    }
#endif
}
