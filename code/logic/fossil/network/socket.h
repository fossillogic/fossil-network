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
#ifndef FOSSIL_NETWORK_SOCKET_H
#define FOSSIL_NETWORK_SOCKET_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
STRING IDS
=============================================================================*/

/*
Socket Types:
    tcp
    udp
    raw

Address Families:
    ipv4
    ipv6
*/

/*=============================================================================
CORE STRUCTURES
=============================================================================*/

typedef struct fossil_net_socket
{
    void *handle;    /* OS-specific socket */
    char id[64];     /* user-defined ID */
    char type[32];   /* socket type string ID */
    char family[32]; /* address family string ID */
    bool blocking;
} fossil_net_socket_t;

typedef struct fossil_net_address
{
    char ip[64]; /* IPv4 or IPv6 string */
    uint16_t port;
    char addr[64];
    char family[32];
} fossil_net_address_t;

typedef struct fossil_net_mac
{
    uint8_t bytes[6];
    char string[18]; /* AA:BB:CC:DD:EE:FF */
} fossil_net_mac_t;

/*=============================================================================
LIFECYCLE
=============================================================================*/

/**
 * @brief Initialize the networking subsystem.
 *
 * This function must be called before any other socket operations.
 * On Windows, it initializes Winsock; on POSIX, it is a no-op.
 *
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_init(void);

/**
 * @brief Shutdown the networking subsystem.
 *
 * Cleans up resources allocated by the networking subsystem.
 * On Windows, it calls WSACleanup; on POSIX, it is a no-op.
 *
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_shutdown(void);

/*=============================================================================
SOCKET MANAGEMENT
=============================================================================*/

/**
 * @brief Create a new socket.
 *
 * Initializes a socket structure with the specified type and address family.
 * Supported types: "tcp", "udp", "raw".
 * Supported families: "ipv4", "ipv6".
 *
 * @param sock   Pointer to socket structure to initialize.
 * @param type   Socket type string ID.
 * @param family Address family string ID.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_create(
    fossil_net_socket_t *sock,
    const char *type,
    const char *family);

/**
 * @brief Close a socket and release its resources.
 *
 * Closes the underlying OS socket handle and resets the structure.
 *
 * @param sock Pointer to socket structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_close(
    fossil_net_socket_t *sock);

/**
 * @brief Set socket blocking or non-blocking mode.
 *
 * Configures the socket to operate in blocking or non-blocking mode.
 *
 * @param sock     Pointer to socket structure.
 * @param blocking true for blocking, false for non-blocking.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_set_blocking(
    fossil_net_socket_t *sock,
    bool blocking);

/*=============================================================================
CONNECTION
=============================================================================*/

/**
 * @brief Bind a socket to a local address and port.
 *
 * Associates the socket with a specific local IP address and port.
 *
 * @param sock Pointer to socket structure.
 * @param addr Pointer to address structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_bind(
    fossil_net_socket_t *sock,
    const fossil_net_address_t *addr);

/**
 * @brief Listen for incoming connections on a socket.
 *
 * Marks the socket as a passive socket to accept incoming connection requests.
 *
 * @param sock    Pointer to socket structure.
 * @param backlog Maximum length of the pending connections queue.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_listen(
    fossil_net_socket_t *sock,
    int backlog);

/**
 * @brief Accept an incoming connection on a listening socket.
 *
 * Accepts a new connection and initializes a client socket structure.
 * Optionally fills in the address of the connecting peer.
 *
 * @param server Pointer to listening socket structure.
 * @param client Pointer to client socket structure to initialize.
 * @param addr   Pointer to address structure to receive peer info (optional).
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_accept(
    fossil_net_socket_t *server,
    fossil_net_socket_t *client,
    fossil_net_address_t *addr);

/**
 * @brief Connect a socket to a remote address.
 *
 * Initiates a connection to the specified remote IP address and port.
 *
 * @param sock Pointer to socket structure.
 * @param addr Pointer to remote address structure.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_connect(
    fossil_net_socket_t *sock,
    const fossil_net_address_t *addr);

/*=============================================================================
DATA TRANSFER
=============================================================================*/

/**
 * @brief Send data through a socket.
 *
 * Sends a buffer of data over the socket. The number of bytes actually sent is returned.
 *
 * @param sock  Pointer to socket structure.
 * @param data  Pointer to data buffer to send.
 * @param size  Size of data buffer in bytes.
 * @param sent  Pointer to variable to receive number of bytes sent.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_send(
    fossil_net_socket_t *sock,
    const void *data,
    uint32_t size,
    uint32_t *sent);

/**
 * @brief Receive data from a socket.
 *
 * Receives data from the socket into the provided buffer. The number of bytes received is returned.
 *
 * @param sock     Pointer to socket structure.
 * @param buffer   Pointer to buffer to receive data.
 * @param size     Size of buffer in bytes.
 * @param received Pointer to variable to receive number of bytes received.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_receive(
    fossil_net_socket_t *sock,
    void *buffer,
    uint32_t size,
    uint32_t *received);

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
    uint16_t port);

/**
 * @brief Convert an address structure to a string.
 *
 * Formats the address as a string (e.g., "127.0.0.1:8080").
 *
 * @param addr   Pointer to address structure.
 * @param buffer Output buffer for string.
 * @param size   Size of output buffer.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_address_to_string(
    const fossil_net_address_t *addr,
    char *buffer,
    uint32_t size);

/**
 * @brief Get the local address of a socket.
 *
 * Retrieves the local IP address and port that the socket is bound to.
 *
 * @param sock Pointer to socket structure.
 * @param addr Pointer to address structure to fill with local address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_get_local_address(
    fossil_net_socket_t *sock,
    fossil_net_address_t *addr);

/*=============================================================================
DNS / HOST
=============================================================================*/

/**
 * @brief Resolve a hostname to an IP address.
 *
 * Looks up the IP address for the given hostname and fills the address structure.
 *
 * @param hostname Hostname to resolve.
 * @param out_addr Pointer to address structure to fill.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_resolve(
    const char *hostname,
    fossil_net_address_t *out_addr);

/**
 * @brief Get the local machine's hostname.
 *
 * Retrieves the system's hostname into the provided buffer.
 *
 * @param buffer Output buffer for hostname.
 * @param size   Size of output buffer.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_hostname(
    char *buffer,
    uint32_t size);

/**
 * @brief Enable or disable the SO_REUSEADDR option on a socket.
 *
 * Allows the socket to bind to an address that is already in use.
 *
 * @param sock    Pointer to socket structure.
 * @param enabled true to enable, false to disable.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_set_reuseaddr(
    fossil_net_socket_t *sock,
    bool enabled);

/**
 * @brief Get the peer (remote) address of a connected socket.
 *
 * Retrieves the remote IP address and port that the socket is connected to.
 *
 * @param sock Pointer to socket structure.
 * @param addr Pointer to address structure to fill with peer address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_get_peer_address(
    fossil_net_socket_t *sock,
    fossil_net_address_t *addr);

/*=============================================================================
MAC ADDRESS
=============================================================================*/

/**
 * @brief Get the primary MAC address of the system.
 *
 * Retrieves the hardware MAC address of the primary network interface.
 *
 * @param mac Pointer to MAC address structure to fill.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_mac_get(
    fossil_net_mac_t *mac);

/**
 * @brief Convert a MAC address structure to a string.
 *
 * Formats the MAC address as a string (e.g., "AA:BB:CC:DD:EE:FF").
 *
 * @param mac    Pointer to MAC address structure.
 * @param buffer Output buffer for string.
 * @param size   Size of output buffer.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_mac_to_string(
    const fossil_net_mac_t *mac,
    char *buffer,
    uint32_t size);

/**
 * @brief Parse a MAC address string into a MAC address structure.
 *
 * Converts a string representation of a MAC address (e.g., "AA:BB:CC:DD:EE:FF")
 * into a fossil_net_mac_t structure.
 *
 * @param mac    Pointer to MAC address structure to fill.
 * @param string String representation of MAC address.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_mac_parse(
    fossil_net_mac_t *mac,
    const char *string);

/**
 * @brief Compare two MAC address structures for equality.
 *
 * Returns true if the two MAC addresses are identical.
 *
 * @param mac1 Pointer to first MAC address structure.
 * @param mac2 Pointer to second MAC address structure.
 * @return true if equal, false otherwise.
 */
bool fossil_net_socket_mac_equal(
    const fossil_net_mac_t *mac1,
    const fossil_net_mac_t *mac2);

/*=============================================================================
POLLING / MULTIPLEXING
=============================================================================*/

/**
 * @brief Poll multiple sockets for readiness.
 *
 * Waits for one or more sockets to become ready for I/O within a timeout period.
 *
 * @param sockets    Array of pointers to socket structures.
 * @param count      Number of sockets in the array.
 * @param timeout_ms Timeout in milliseconds.
 * @return Number of sockets ready, 0 on timeout, or negative on error.
 */
int fossil_net_socket_poll(
    fossil_net_socket_t **sockets,
    uint32_t count,
    uint32_t timeout_ms);

/**
 * @brief Add a socket to a poll set.
 *
 * Registers a socket to be monitored for readiness in a poll set.
 *
 * @param poll_set Array of pointers to socket structures.
 * @param count    Pointer to the number of sockets in the poll set (updated).
 * @param max      Maximum number of sockets allowed in the poll set.
 * @param sock     Pointer to socket structure to add.
 * @return 0 on success, non-zero on failure (e.g., if poll set is full).
 */
int fossil_net_socket_poll_add(
    fossil_net_socket_t **poll_set,
    uint32_t *count,
    uint32_t max,
    fossil_net_socket_t *sock);

/**
 * @brief Remove a socket from a poll set.
 *
 * Unregisters a socket from being monitored in a poll set.
 *
 * @param poll_set Array of pointers to socket structures.
 * @param count    Pointer to the number of sockets in the poll set (updated).
 * @param sock     Pointer to socket structure to remove.
 * @return 0 on success, non-zero on failure (e.g., if socket not found).
 */
int fossil_net_socket_poll_remove(
    fossil_net_socket_t **poll_set,
    uint32_t *count,
    fossil_net_socket_t *sock);

/**
 * @brief Clear all sockets from a poll set.
 *
 * Empties the poll set so that no sockets are monitored.
 *
 * @param poll_set Array of pointers to socket structures.
 * @param count    Pointer to the number of sockets in the poll set (set to 0).
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_poll_clear(
    fossil_net_socket_t **poll_set,
    uint32_t *count);

/*=============================================================================
ERROR HANDLING
=============================================================================*/

/**
 * @brief Get the last socket error code.
 *
 * Returns the last error code produced by a socket operation.
 *
 * @return Error code (platform-specific).
 */
int fossil_net_socket_error_last(void);

/**
 * @brief Convert a socket error code to a human-readable string.
 *
 * Returns a string describing the specified error code.
 *
 * @param err Error code.
 * @return Pointer to static string describing the error.
 */
const char *fossil_net_socket_error_string(int err);

/*=============================================================================
UTILITY
=============================================================================*/

/**
 * @brief Sleep for a specified number of milliseconds.
 *
 * Suspends the calling thread for the given duration.
 *
 * @param ms Number of milliseconds to sleep.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_socket_sleep(uint32_t ms);

#ifdef __cplusplus
}
#include <string>
#include <cstring>

namespace fossil::net
{

    class Socket
    {
    public:
        /**
         * @brief Default constructor.
         *
         * Initializes the internal socket structure to zero.
         */
        Socket()
        {
            std::memset(&sock_, 0, sizeof(sock_));
        }

        /**
         * @brief Destructor.
         *
         * Closes the socket and releases any associated resources.
         */
        ~Socket()
        {
            socket_close();
        }

        /**
         * @brief Initialize the networking subsystem.
         *
         * This function must be called before any other socket operations.
         * @return 0 on success, non-zero on failure.
         */
        static int socket_init()
        {
            return fossil_net_socket_init();
        }

        /**
         * @brief Shutdown the networking subsystem.
         *
         * Cleans up resources allocated by the networking subsystem.
         * @return 0 on success, non-zero on failure.
         */
        static int socket_shutdown()
        {
            return fossil_net_socket_shutdown();
        }

        /**
         * @brief Create a new socket.
         *
         * Initializes the socket with the specified type and address family.
         *
         * @param type   Socket type string ID.
         * @param family Address family string ID.
         * @return 0 on success, non-zero on failure.
         */
        int socket_create(const std::string &type, const std::string &family)
        {
            return fossil_net_socket_create(&sock_, type.c_str(), family.c_str());
        }

        /**
         * @brief Close the socket.
         *
         * Closes the underlying OS socket handle and resets the structure.
         *
         * @return 0 on success, non-zero on failure.
         */
        int socket_close()
        {
            return fossil_net_socket_close(&sock_);
        }

        /**
         * @brief Set blocking or non-blocking mode.
         *
         * Configures the socket to operate in blocking or non-blocking mode.
         *
         * @param blocking true for blocking, false for non-blocking.
         * @return 0 on success, non-zero on failure.
         */
        int socket_set_blocking(bool blocking)
        {
            return fossil_net_socket_set_blocking(&sock_, blocking);
        }

        /**
         * @brief Set SO_REUSEADDR option on the socket.
         *
         * Allows the socket to bind to an address that is already in use.
         *
         * @param enabled true to enable, false to disable.
         * @return 0 on success, non-zero on failure.
         */
        int socket_set_reuseaddr(bool enabled)
        {
            return fossil_net_socket_set_reuseaddr(&sock_, enabled);
        }

        /**
         * @brief Get the local address of the socket.
         *
         * Retrieves the local IP address and port that the socket is bound to.
         *
         * @param addr Pointer to address structure to fill with local address.
         * @return 0 on success, non-zero on failure.
         */
        int socket_get_local_address(fossil_net_address_t *addr)
        {
            return fossil_net_socket_get_local_address(&sock_, addr);
        }

        /**
         * @brief Get the peer (remote) address of the connected socket.
         *
         * Retrieves the remote IP address and port that the socket is connected to.
         *
         * @param addr Pointer to address structure to fill with peer address.
         * @return 0 on success, non-zero on failure.
         */
        int socket_get_peer_address(fossil_net_address_t *addr)
        {
            return fossil_net_socket_get_peer_address(&sock_, addr);
        }

        /**
         * @brief Bind the socket to a local address and port.
         *
         * Associates the socket with a specific local IP address and port.
         *
         * @param addr Pointer to address structure.
         * @return 0 on success, non-zero on failure.
         */
        int socket_bind(const fossil_net_address_t *addr)
        {
            return fossil_net_socket_bind(&sock_, addr);
        }

        /**
         * @brief Listen for incoming connections.
         *
         * Marks the socket as a passive socket to accept incoming connection requests.
         *
         * @param backlog Maximum length of the pending connections queue.
         * @return 0 on success, non-zero on failure.
         */
        int socket_listen(int backlog)
        {
            return fossil_net_socket_listen(&sock_, backlog);
        }

        /**
         * @brief Accept an incoming connection.
         *
         * Accepts a new connection and initializes a client socket structure.
         * Optionally fills in the address of the connecting peer.
         *
         * @param client Reference to client socket to initialize.
         * @param addr   Pointer to address structure to receive peer info (optional).
         * @return 0 on success, non-zero on failure.
         */
        int socket_accept(Socket &client, fossil_net_address_t *addr)
        {
            return fossil_net_socket_accept(&sock_, &client.sock_, addr);
        }

        /**
         * @brief Connect the socket to a remote address.
         *
         * Initiates a connection to the specified remote IP address and port.
         *
         * @param addr Pointer to remote address structure.
         * @return 0 on success, non-zero on failure.
         */
        int socket_connect(const fossil_net_address_t *addr)
        {
            return fossil_net_socket_connect(&sock_, addr);
        }

        /**
         * @brief Send data through the socket.
         *
         * Sends a buffer of data over the socket.
         *
         * @param data Pointer to data buffer to send.
         * @param size Size of data buffer in bytes.
         * @param sent Pointer to variable to receive number of bytes sent.
         * @return 0 on success, non-zero on failure.
         */
        int socket_send(const void *data, uint32_t size, uint32_t *sent)
        {
            return fossil_net_socket_send(&sock_, data, size, sent);
        }

        /**
         * @brief Receive data from the socket.
         *
         * Receives data from the socket into the provided buffer.
         *
         * @param buffer   Pointer to buffer to receive data.
         * @param size     Size of buffer in bytes.
         * @param received Pointer to variable to receive number of bytes received.
         * @return 0 on success, non-zero on failure.
         */
        int socket_receive(void *buffer, uint32_t size, uint32_t *received)
        {
            return fossil_net_socket_receive(&sock_, buffer, size, received);
        }

        /**
         * @brief Get a pointer to the native socket structure.
         *
         * @return Pointer to the internal fossil_net_socket_t structure.
         */
        fossil_net_socket_t *native_handle() { return &sock_; }

        /**
         * @brief Get a const pointer to the native socket structure.
         *
         * @return Const pointer to the internal fossil_net_socket_t structure.
         */
        const fossil_net_socket_t *native_handle() const { return &sock_; }

        // ----------- Static utility methods for address and DNS/host ------------

        /**
         * @brief Parse an IP address and port into an address structure.
         *
         * @param addr Pointer to address structure to fill.
         * @param ip   String representation of IP address.
         * @param port Port number.
         * @return 0 on success, non-zero on failure.
         */
        static int address_parse(fossil_net_address_t *addr, const std::string &ip, uint16_t port)
        {
            return fossil_net_socket_address_parse(addr, ip.c_str(), port);
        }

        /**
         * @brief Convert an address structure to a string.
         *
         * @param addr   Pointer to address structure.
         * @param buffer Output buffer for string.
         * @param size   Size of output buffer.
         * @return 0 on success, non-zero on failure.
         */
        static int address_to_string(const fossil_net_address_t *addr, char *buffer, uint32_t size)
        {
            return fossil_net_socket_address_to_string(addr, buffer, size);
        }

        /**
         * @brief Resolve a hostname to an IP address.
         *
         * @param hostname Hostname to resolve.
         * @param out_addr Pointer to address structure to fill.
         * @return 0 on success, non-zero on failure.
         */
        static int resolve(const std::string &hostname, fossil_net_address_t *out_addr)
        {
            return fossil_net_socket_resolve(hostname.c_str(), out_addr);
        }

        /**
         * @brief Get the local machine's hostname.
         *
         * @param buffer Output buffer for hostname.
         * @param size   Size of output buffer.
         * @return 0 on success, non-zero on failure.
         */
        static int hostname(char *buffer, uint32_t size)
        {
            return fossil_net_socket_hostname(buffer, size);
        }

        // ----------- Static utility methods for MAC address ------------

        /**
         * @brief Get the primary MAC address of the system.
         *
         * @param mac Pointer to MAC address structure to fill.
         * @return 0 on success, non-zero on failure.
         */
        static int mac_get(fossil_net_mac_t *mac)
        {
            return fossil_net_socket_mac_get(mac);
        }

        /**
         * @brief Convert a MAC address structure to a string.
         *
         * @param mac    Pointer to MAC address structure.
         * @param buffer Output buffer for string.
         * @param size   Size of output buffer.
         * @return 0 on success, non-zero on failure.
         */
        static int mac_to_string(const fossil_net_mac_t *mac, char *buffer, uint32_t size)
        {
            return fossil_net_socket_mac_to_string(mac, buffer, size);
        }

        /**
         * @brief Parse a MAC address string into a MAC address structure.
         *
         * @param mac    Pointer to MAC address structure to fill.
         * @param string String representation of MAC address.
         * @return 0 on success, non-zero on failure.
         */
        static int mac_parse(fossil_net_mac_t *mac, const std::string &string)
        {
            return fossil_net_socket_mac_parse(mac, string.c_str());
        }

        /**
         * @brief Compare two MAC address structures for equality.
         *
         * @param mac1 Pointer to first MAC address structure.
         * @param mac2 Pointer to second MAC address structure.
         * @return true if equal, false otherwise.
         */
        static bool mac_equal(const fossil_net_mac_t *mac1, const fossil_net_mac_t *mac2)
        {
            return fossil_net_socket_mac_equal(mac1, mac2);
        }

        // ----------- Static utility methods for polling ------------

        /**
         * @brief Poll multiple sockets for readiness.
         *
         * @param sockets    Array of pointers to socket structures.
         * @param count      Number of sockets in the array.
         * @param timeout_ms Timeout in milliseconds.
         * @return Number of sockets ready, 0 on timeout, or negative on error.
         */
        static int poll(fossil_net_socket_t **sockets, uint32_t count, uint32_t timeout_ms)
        {
            return fossil_net_socket_poll(sockets, count, timeout_ms);
        }

        /**
         * @brief Add a socket to a poll set.
         *
         * @param poll_set Array of pointers to socket structures.
         * @param count    Pointer to the number of sockets in the poll set (updated).
         * @param max      Maximum number of sockets allowed in the poll set.
         * @param sock     Pointer to socket structure to add.
         * @return 0 on success, non-zero on failure.
         */
        static int poll_add(fossil_net_socket_t **poll_set, uint32_t *count, uint32_t max, fossil_net_socket_t *sock)
        {
            return fossil_net_socket_poll_add(poll_set, count, max, sock);
        }

        /**
         * @brief Remove a socket from a poll set.
         *
         * @param poll_set Array of pointers to socket structures.
         * @param count    Pointer to the number of sockets in the poll set (updated).
         * @param sock     Pointer to socket structure to remove.
         * @return 0 on success, non-zero on failure.
         */
        static int poll_remove(fossil_net_socket_t **poll_set, uint32_t *count, fossil_net_socket_t *sock)
        {
            return fossil_net_socket_poll_remove(poll_set, count, sock);
        }

        /**
         * @brief Clear all sockets from a poll set.
         *
         * @param poll_set Array of pointers to socket structures.
         * @param count    Pointer to the number of sockets in the poll set (set to 0).
         * @return 0 on success, non-zero on failure.
         */
        static int poll_clear(fossil_net_socket_t **poll_set, uint32_t *count)
        {
            return fossil_net_socket_poll_clear(poll_set, count);
        }

        // ----------- Static utility methods for error handling ------------

        /**
         * @brief Get the last socket error code.
         *
         * @return Error code (platform-specific).
         */
        static int error_last()
        {
            return fossil_net_socket_error_last();
        }

        /**
         * @brief Convert a socket error code to a human-readable string.
         *
         * @param err Error code.
         * @return Pointer to static string describing the error.
         */
        static const char *error_string(int err)
        {
            return fossil_net_socket_error_string(err);
        }

        // ----------- Static utility methods for sleep ------------

        /**
         * @brief Sleep for a specified number of milliseconds.
         *
         * @param ms Number of milliseconds to sleep.
         * @return 0 on success, non-zero on failure.
         */
        static int sleep(uint32_t ms)
        {
            return fossil_net_socket_sleep(ms);
        }

    private:
        fossil_net_socket_t sock_;
    };

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_SOCKET_H */
