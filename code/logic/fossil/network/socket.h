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
#ifndef FOSSIL_NETWORK_SOCKET_H
#define FOSSIL_NETWORK_SOCKET_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

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
typedef struct fossil_network_socket fossil_network_socket_t;

// ------------------------------
// Poll structure
// ------------------------------
typedef struct {
    fossil_network_socket_t *sock;
    int events;   /**< Bitmask: 1=readable, 2=writeable, 4=error */
    int revents;  /**< Output mask after polling */
} fossil_network_pollfd_t;

// ------------------------------
/**
 * @brief Initialize the network socket subsystem (Windows only).
 *
 * This function must be called before using any socket operations on Windows.
 * On other platforms, it is a no-op.
 *
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_init(void);

/**
 * @brief Cleanup the network socket subsystem (Windows only).
 *
 * This function should be called to release resources allocated by the socket
 * subsystem on Windows. On other platforms, it is a no-op.
 *
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_cleanup(void);

// ------------------------------
/**
 * @brief Convert a protocol name string to a fossil_protocol_t value.
 *
 * @param name The protocol name (e.g., "tcp", "udp").
 * @return The corresponding fossil_protocol_t value, or FOSSIL_PROTO_UNKNOWN if not found.
 */
fossil_protocol_t fossil_network_socket_proto_from_name(const char *name);

/**
 * @brief Convert a fossil_protocol_t value to its protocol name string.
 *
 * @param proto The protocol enumeration value.
 * @return The protocol name string, or "unknown" if not recognized.
 */
const char *fossil_network_socket_proto_to_name(fossil_protocol_t proto);

// ------------------------------
/**
 * @brief Create a new network socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param family Address family (e.g., AF_INET, AF_INET6).
 * @param proto Protocol type (see fossil_protocol_t).
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_create(fossil_network_socket_t *sock,
                                 int family, fossil_protocol_t proto);

/**
 * @brief Bind a socket to a local address and port.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param address Local address to bind to (e.g., "127.0.0.1").
 * @param port Local port number.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_bind(fossil_network_socket_t *sock,
                               const char *address, uint16_t port);

/**
 * @brief Set a socket to listen for incoming connections.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param backlog Maximum number of pending connections.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_listen(fossil_network_socket_t *sock, int backlog);

/**
 * @brief Accept an incoming connection on a listening socket.
 *
 * @param server Pointer to the listening fossil_network_socket_t structure.
 * @param client Pointer to a fossil_network_socket_t structure for the new connection.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_accept(fossil_network_socket_t *server,
                                 fossil_network_socket_t *client);

/**
 * @brief Connect a socket to a remote address and port.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param address Remote address to connect to.
 * @param port Remote port number.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_connect(fossil_network_socket_t *sock,
                                  const char *address, uint16_t port);

/**
 * @brief Close a network socket and release its resources.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_close(fossil_network_socket_t *sock);

// ------------------------------
/**
 * @brief Send data through a network socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param buffer Pointer to the data buffer to send.
 * @param length Number of bytes to send.
 * @return Number of bytes sent, or -1 on error.
 */
ssize_t fossil_network_socket_send(fossil_network_socket_t *sock,
                                   const void *buffer, size_t length);

/**
 * @brief Receive data from a network socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param buffer Pointer to the buffer to receive data into.
 * @param length Maximum number of bytes to receive.
 * @return Number of bytes received, or -1 on error.
 */
ssize_t fossil_network_socket_recv(fossil_network_socket_t *sock,
                                   void *buffer, size_t length);

// ------------------------------
/**
 * @brief Open and optionally connect or bind a socket using protocol name and address.
 *
 * This convenience function creates a socket, sets its protocol, and binds or connects
 * it to the specified address and port as appropriate.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param proto_name Protocol name string (e.g., "tcp", "udp").
 * @param address Address to bind or connect to.
 * @param port Port number.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_open(fossil_network_socket_t *sock,
                               const char *proto_name,
                               const char *address, uint16_t port);

/**
 * @brief Set an integer socket option.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param level Protocol level (e.g., SOL_SOCKET, IPPROTO_TCP).
 * @param option Option name (e.g., SO_REUSEADDR).
 * @param value Integer value to set.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_set_option(fossil_network_socket_t *sock,
                                     int level, int option, int value);

/**
 * @brief Get an integer socket option.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param level Protocol level (e.g., SOL_SOCKET, IPPROTO_TCP).
 * @param option Option name (e.g., SO_ERROR).
 * @param value Output pointer for the integer value.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_get_option(fossil_network_socket_t *sock,
                                     int level, int option, int *value);

/**
 * @brief Set the socket to blocking or non-blocking mode.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param nonblock 1 to enable non-blocking, 0 to disable.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_set_nonblocking(fossil_network_socket_t *sock,
                                          int nonblock);

/**
 * @brief Resolve a hostname to an IP address.
 *
 * @param hostname The hostname (e.g., "example.com").
 * @param ip_buffer Buffer to store the resulting IP address string.
 * @param ip_buffer_len Length of ip_buffer.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_resolve_hostname(const char *hostname,
                                           char *ip_buffer,
                                           size_t ip_buffer_len);

/**
 * @brief Get the local or remote address of a socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param buffer Buffer to store the address string.
 * @param buffer_len Length of buffer.
 * @param remote 1 for remote address, 0 for local address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_get_address(fossil_network_socket_t *sock,
                                      char *buffer, size_t buffer_len,
                                      int remote);

/**
 * @brief Get the last socket error code.
 *
 * @return Platform-specific error code.
 */
int fossil_network_socket_last_error(void);

/**
 * @brief Convert a socket error code to a human-readable string.
 *
 * @param err Error code.
 * @return Constant string description of the error.
 */
const char *fossil_network_socket_error_string(int err);

/**
 * @brief Poll multiple sockets for readiness.
 *
 * @param fds Array of pollfd structures.
 * @param nfds Number of sockets in fds.
 * @param timeout Timeout in milliseconds (-1 for infinite).
 * @return Number of ready sockets, 0 on timeout, or -1 on error.
 */
int fossil_network_socket_poll(fossil_network_pollfd_t *fds,
                               size_t nfds, int timeout);

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

#endif /* FOSSIL_NETWORK_SOCKET_H */
