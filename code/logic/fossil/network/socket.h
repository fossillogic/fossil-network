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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <ctype.h>

// --- Portable type definitions for macOS / BSD headers ---
#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/cdefs.h>
#ifndef _U_INT_DEFINED
typedef unsigned int   u_int;
typedef unsigned char  u_char;
typedef unsigned short u_short;
#define _U_INT_DEFINED
#endif
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
    #include <bcrypt.h>
    #include <iphlpapi.h>
    #include <windows.h>

    typedef SOCKET fossil_socket_fd_t;

    // ssize_t is POSIX; Windows lacks it
    #if defined(_MSC_VER)
        #include <BaseTsd.h>
        typedef SSIZE_T ssize_t;
    #elif defined(__MINGW32__)
        #ifndef _SSIZE_T_DEFINED
            typedef long ssize_t;
            #define _SSIZE_T_DEFINED
        #endif
    #endif

#else
    // POSIX / Unix-like
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

#ifdef __cplusplus
extern "C" {
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
typedef struct fossil_network_socket_t {
    fossil_socket_fd_t fd;
    int family;
    int type;
    fossil_protocol_t proto;
} fossil_network_socket_t;

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

// ------------------------------
// Socket Shutdown
// ------------------------------
/**
 * @brief Shut down part or all of a network socket connection.
 *
 * Allows a socket to be gracefully closed in one direction (read, write)
 * or both, depending on the 'how' argument.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param how  Shutdown mode:
 *             0 = disable receives,
 *             1 = disable sends,
 *             2 = disable both.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_shutdown(fossil_network_socket_t *sock, int how);

// ------------------------------
// Datagram Support
// ------------------------------
/**
 * @brief Send a datagram (UDP or raw) to a specific address and port.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param buf Pointer to the data buffer to send.
 * @param len Number of bytes to send.
 * @param address Destination address (IPv4 or IPv6 string).
 * @param port Destination port number.
 * @return Number of bytes sent, or -1 on error.
 */
ssize_t fossil_network_socket_sendto(fossil_network_socket_t *sock,
                                     const void *buf, size_t len,
                                     const char *address, uint16_t port);

/**
 * @brief Receive a datagram (UDP or raw) from a socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param buf Buffer to store received data.
 * @param len Maximum number of bytes to receive.
 * @param address Output buffer for source address string.
 * @param addr_len Length of the address buffer.
 * @param port Output pointer for the source port.
 * @return Number of bytes received, or -1 on error.
 */
ssize_t fossil_network_socket_recvfrom(fossil_network_socket_t *sock,
                                       void *buf, size_t len,
                                       char *address, size_t addr_len,
                                       uint16_t *port);

// ------------------------------
// Timeout Helpers
// ------------------------------
/**
 * @brief Set send and receive timeouts for a socket.
 *
 * When a timeout is set, send/recv operations will return an error
 * if they block longer than the specified number of milliseconds.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param send_ms Timeout for send operations in milliseconds (0 = no timeout).
 * @param recv_ms Timeout for receive operations in milliseconds (0 = no timeout).
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_socket_set_timeout(fossil_network_socket_t *sock,
                                      int send_ms, int recv_ms);

// ------------------------------
// Single-Socket Wait
// ------------------------------
/**
 * @brief Wait for a socket to become readable or writable.
 *
 * This is a convenience wrapper around poll/select for one socket.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @param events Bitmask: 1=readable, 2=writeable.
 * @param timeout Timeout in milliseconds (-1 for infinite).
 * @return Number of events ready (>0), 0 on timeout, or -1 on error.
 */
int fossil_network_socket_wait(fossil_network_socket_t *sock,
                               int events, int timeout);

// ------------------------------
// Address Helpers
// ------------------------------
/**
 * @brief Check if a socket uses IPv6.
 *
 * @param sock Pointer to a fossil_network_socket_t structure.
 * @return 1 if IPv6, 0 if IPv4, -1 on error.
 */
int fossil_network_socket_is_ipv6(fossil_network_socket_t *sock);

// ------------------------------
// Cross-Platform Error Codes
// ------------------------------
/**
 * @brief Normalized cross-platform error codes for sockets.
 *
 * These map common platform-specific error values (errno/WSAGetLastError)
 * into a consistent set used by Fossil sockets.
 */
typedef enum {
    FOSSIL_NET_OK = 0,            /**< No error */
    FOSSIL_NET_ERR_UNKNOWN = -1,  /**< Unmapped/unknown error */
    FOSSIL_NET_ERR_WOULDBLOCK,    /**< Operation would block */
    FOSSIL_NET_ERR_CONNRESET,     /**< Connection reset by peer */
    FOSSIL_NET_ERR_TIMEDOUT,      /**< Operation timed out */
    FOSSIL_NET_ERR_REFUSED,       /**< Connection refused */
    FOSSIL_NET_ERR_ADDRINUSE,     /**< Address already in use */
    FOSSIL_NET_ERR_NETDOWN,       /**< Network is down */
    FOSSIL_NET_ERR_NETUNREACH,    /**< Network unreachable */
    FOSSIL_NET_ERR_HOSTUNREACH    /**< Host unreachable */
} fossil_network_error_t;

/**
 * @brief Translate the last platform-specific socket error
 *        into a fossil_network_error_t code.
 *
 * @return Normalized fossil_network_error_t error code.
 */
fossil_network_error_t fossil_network_socket_translate_error(void);

#ifdef __cplusplus
}
#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include <cstring>

namespace fossil {

namespace network {

    /**
     * @brief C++ RAII wrapper for fossil_network_socket_t.
     *
     * This class provides a convenient, exception-safe C++ interface to the Fossil
     * network socket API. It manages the socket's lifetime, disables copying,
     * and supports move semantics. All socket operations are provided as member
     * functions, and static helpers are available for global socket operations.
     */
    class Socket {
    public:
        /**
         * @brief Default constructor. Initializes the internal socket structure to zero.
         */
        Socket() { std::memset(&c_sock_, 0, sizeof(c_sock_)); }

        /**
         * @brief Construct from an existing fossil_network_socket_t.
         * @param s The C socket structure to wrap.
         */
        explicit Socket(const fossil_network_socket_t& s) : c_sock_(s) {}

        /**
         * @brief Destructor. Closes the socket if open.
         */
        ~Socket() { close(); }

        // Non-copyable
        /**
         * @brief Copy constructor (deleted).
         */
        Socket(const Socket&) = delete;
        /**
         * @brief Copy assignment (deleted).
         */
        Socket& operator=(const Socket&) = delete;

        // Movable
        /**
         * @brief Move constructor. Transfers ownership of the socket.
         * @param other The socket to move from.
        #if defined(_WIN32)
                static constexpr fossil_socket_fd_t invalid_fd = INVALID_SOCKET;
        #else
                static constexpr fossil_socket_fd_t invalid_fd = -1;
        #endif
                Socket(Socket&& other) noexcept { c_sock_ = other.c_sock_; other.c_sock_.fd = invalid_fd; }
        Socket& operator=(Socket&& other) noexcept {
            if (this != &other) {
                close();
                c_sock_ = other.c_sock_;
                other.c_sock_.fd = invalid_fd;
            }
            return *this;
        }
                close();
                c_sock_ = other.c_sock_;
                other.c_sock_.fd = -1;
            }
            return *this;
        }

        /**
         * @brief Initialize the socket subsystem (Windows only).
         * @return 0 on success, non-zero on failure.
         */
        static int init() { return fossil_network_socket_init(); }

        /**
         * @brief Cleanup the socket subsystem (Windows only).
         * @return 0 on success, non-zero on failure.
         */
        static int cleanup() { return fossil_network_socket_cleanup(); }

        /**
         * @brief Convert protocol name to fossil_protocol_t.
         * @param name Protocol name string.
         * @return Protocol enumeration value.
         */
        static fossil_protocol_t proto_from_name(const std::string& name) {
            return fossil_network_socket_proto_from_name(name.c_str());
        }

        /**
         * @brief Convert fossil_protocol_t to protocol name string.
         * @param proto Protocol enumeration value.
         * @return Protocol name string.
         */
        static std::string proto_to_name(fossil_protocol_t proto) {
            return fossil_network_socket_proto_to_name(proto);
        }

        /**
         * @brief Create a new socket.
         * @param family Address family (e.g., AF_INET).
         * @param proto Protocol type.
         * @return 0 on success, non-zero on failure.
         */
        int create(int family, fossil_protocol_t proto) {
            return fossil_network_socket_create(&c_sock_, family, proto);
        }

        /**
         * @brief Bind the socket to a local address and port.
         * @param address Local address string.
         * @param port Local port number.
         * @return 0 on success, non-zero on failure.
         */
        int bind(const std::string& address, uint16_t port) {
            return fossil_network_socket_bind(&c_sock_, address.c_str(), port);
        }

        /**
         * @brief Set the socket to listen for incoming connections.
         * @param backlog Maximum number of pending connections.
         * @return 0 on success, non-zero on failure.
         */
        int listen(int backlog) {
            return fossil_network_socket_listen(&c_sock_, backlog);
        }

        /**
         * @brief Accept an incoming connection.
         * @param client Socket object to receive the new connection.
         * @return 0 on success, non-zero on failure.
         */
        int accept(Socket& client) {
            return fossil_network_socket_accept(&c_sock_, &client.c_sock_);
        }

        /**
         * @brief Connect the socket to a remote address and port.
         * @param address Remote address string.
         * @param port Remote port number.
         * @return 0 on success, non-zero on failure.
         */
        int connect(const std::string& address, uint16_t port) {
            return fossil_network_socket_connect(&c_sock_, address.c_str(), port);
        int close() {
            if (c_sock_.fd != invalid_fd) {
                int r = fossil_network_socket_close(&c_sock_);
                c_sock_.fd = invalid_fd;
                return r;
            }
            return 0;
        }
                int r = fossil_network_socket_close(&c_sock_);
                c_sock_.fd = -1;
                return r;
            }
            return 0;
        }

        /**
         * @brief Send data through the socket.
         * @param buffer Pointer to data buffer.
         * @param length Number of bytes to send.
         * @return Number of bytes sent, or -1 on error.
         */
        ssize_t send(const void* buffer, size_t length) {
            return fossil_network_socket_send(&c_sock_, buffer, length);
        }

        /**
         * @brief Receive data from the socket.
         * @param buffer Pointer to receive buffer.
         * @param length Maximum number of bytes to receive.
         * @return Number of bytes received, or -1 on error.
         */
        ssize_t recv(void* buffer, size_t length) {
            return fossil_network_socket_recv(&c_sock_, buffer, length);
        }

        /**
         * @brief Open and optionally connect or bind a socket.
         * @param proto_name Protocol name string.
         * @param address Address to bind or connect to.
         * @param port Port number.
         * @return 0 on success, non-zero on failure.
         */
        int open(const std::string& proto_name, const std::string& address, uint16_t port) {
            return fossil_network_socket_open(&c_sock_, proto_name.c_str(), address.c_str(), port);
        }

        /**
         * @brief Set an integer socket option.
         * @param level Protocol level.
         * @param option Option name.
         * @param value Integer value to set.
         * @return 0 on success, non-zero on failure.
         */
        int set_option(int level, int option, int value) {
            return fossil_network_socket_set_option(&c_sock_, level, option, value);
        }

        /**
         * @brief Get an integer socket option.
         * @param level Protocol level.
         * @param option Option name.
         * @param value Output pointer for the value.
         * @return 0 on success, non-zero on failure.
         */
        int get_option(int level, int option, int* value) {
            return fossil_network_socket_get_option(&c_sock_, level, option, value);
        }

        /**
         * @brief Set the socket to blocking or non-blocking mode.
         * @param nonblock 1 for non-blocking, 0 for blocking.
         * @return 0 on success, non-zero on failure.
         */
        int set_nonblocking(int nonblock) {
            return fossil_network_socket_set_nonblocking(&c_sock_, nonblock);
        }

        /**
         * @brief Resolve a hostname to an IP address.
         * @param hostname Hostname string.
         * @param ip_buffer Buffer for resulting IP address.
         * @param ip_buffer_len Length of ip_buffer.
         * @return 0 on success, non-zero on failure.
         */
        static int resolve_hostname(const std::string& hostname, char* ip_buffer, size_t ip_buffer_len) {
            return fossil_network_socket_resolve_hostname(hostname.c_str(), ip_buffer, ip_buffer_len);
        }

        /**
         * @brief Get the local or remote address of the socket.
         * @param buffer Buffer to store address string.
         * @param buffer_len Length of buffer.
         * @param remote 1 for remote address, 0 for local address.
         * @return 0 on success, non-zero on failure.
         */
        int get_address(char* buffer, size_t buffer_len, int remote) {
            return fossil_network_socket_get_address(&c_sock_, buffer, buffer_len, remote);
        }

        /**
         * @brief Get the last socket error code.
         * @return Platform-specific error code.
         */
        static int last_error() { return fossil_network_socket_last_error(); }

        /**
         * @brief Convert a socket error code to a human-readable string.
         * @param err Error code.
         * @return Error string.
         */
        static std::string error_string(int err) { return fossil_network_socket_error_string(err); }

        /**
         * @brief Poll multiple sockets for readiness.
         * @param fds Array of pollfd structures.
         * @param nfds Number of sockets in fds.
         * @param timeout Timeout in milliseconds.
         * @return Number of ready sockets, 0 on timeout, or -1 on error.
         */
        static int poll(fossil_network_pollfd_t* fds, size_t nfds, int timeout) {
            return fossil_network_socket_poll(fds, nfds, timeout);
        }

        /**
         * @brief Shut down part or all of the socket connection.
         * @param how Shutdown mode (0=recv, 1=send, 2=both).
         * @return 0 on success, non-zero on failure.
         */
        int shutdown(int how) {
            return fossil_network_socket_shutdown(&c_sock_, how);
        }

        /**
         * @brief Send a datagram to a specific address and port.
         * @param buf Data buffer to send.
         * @param len Number of bytes to send.
         * @param address Destination address string.
         * @param port Destination port number.
         * @return Number of bytes sent, or -1 on error.
         */
        ssize_t sendto(const void* buf, size_t len, const std::string& address, uint16_t port) {
            return fossil_network_socket_sendto(&c_sock_, buf, len, address.c_str(), port);
        }

        /**
         * @brief Receive a datagram from the socket.
         * @param buf Buffer to store received data.
         * @param len Maximum number of bytes to receive.
         * @param address Output buffer for source address.
         * @param addr_len Length of address buffer.
         * @param port Output pointer for source port.
         * @return Number of bytes received, or -1 on error.
         */
        ssize_t recvfrom(void* buf, size_t len, char* address, size_t addr_len, uint16_t* port) {
            return fossil_network_socket_recvfrom(&c_sock_, buf, len, address, addr_len, port);
        }

        /**
         * @brief Set send and receive timeouts for the socket.
         * @param send_ms Send timeout in milliseconds.
         * @param recv_ms Receive timeout in milliseconds.
         * @return 0 on success, non-zero on failure.
         */
        int set_timeout(int send_ms, int recv_ms) {
            return fossil_network_socket_set_timeout(&c_sock_, send_ms, recv_ms);
        }

        /**
         * @brief Wait for the socket to become readable or writable.
         * @param events Bitmask: 1=readable, 2=writeable.
         * @param timeout Timeout in milliseconds.
         * @return Number of events ready, 0 on timeout, or -1 on error.
         */
        int wait(int events, int timeout) {
            return fossil_network_socket_wait(&c_sock_, events, timeout);
        }

        /**
         * @brief Check if the socket uses IPv6.
         * @return 1 if IPv6, 0 if IPv4, -1 on error.
         */
        int is_ipv6() {
            return fossil_network_socket_is_ipv6(&c_sock_);
        }

        /**
         * @brief Translate the last platform-specific socket error.
         * @return Normalized fossil_network_error_t error code.
         */
        static fossil_network_error_t translate_error() {
            return fossil_network_socket_translate_error();
        }

        /**
         * @brief Get a pointer to the underlying fossil_network_socket_t.
         * @return Pointer to the internal socket structure.
         */
        fossil_network_socket_t* native_handle() { return &c_sock_; }

        /**
         * @brief Get a const pointer to the underlying fossil_network_socket_t.
         * @return Const pointer to the internal socket structure.
         */
        const fossil_network_socket_t* native_handle() const { return &c_sock_; }

    private:
        /**
         * @brief Internal C socket structure.
         */
        fossil_network_socket_t c_sock_;
    };

} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_SOCKET_H */
