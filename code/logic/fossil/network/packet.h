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
#ifndef FOSSIL_NETWORK_PACKET_H
#define FOSSIL_NETWORK_PACKET_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    uint8_t *data;
    size_t length;
    size_t capacity;
    uint16_t proto;   // e.g., ETH_P_IP, custom
    uint32_t src;
    uint32_t dst;
} fossil_network_packet_t;

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Initialize a network packet with the specified capacity.
 * 
 * @param pkt Pointer to the packet structure to initialize.
 * @param capacity Number of bytes to allocate for the packet data.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_packet_init(fossil_network_packet_t *pkt, size_t capacity);

/**
 * @brief Append data to the packet.
 * 
 * @param pkt Pointer to the packet structure.
 * @param data Pointer to the data to append.
 * @param len Length of the data to append.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_packet_append(fossil_network_packet_t *pkt, const void *data, size_t len);

/**
 * @brief Parse the IP header from the packet data.
 * 
 * @param pkt Pointer to the packet structure.
 * @param header_out Pointer to a buffer to receive the parsed IP header.
 * @return 0 on success, non-zero on failure.
 */
int fossil_network_packet_parse_ip(fossil_network_packet_t *pkt, void *header_out);

/**
 * @brief Free resources associated with the packet.
 * 
 * @param pkt Pointer to the packet structure to free.
 */
void fossil_network_packet_free(fossil_network_packet_t *pkt);

#ifdef __cplusplus
}

namespace fossil {

    namespace network {

        /**
         * @class Packet
         * @brief C++ wrapper for fossil_network_packet_t and related functions.
         *
         * This class provides static methods to manage network packets, including
         * initialization, data appending, IP header parsing, and resource cleanup.
         * All methods operate on the underlying C struct fossil_network_packet_t.
         */
        class Packet {
        public:
            /**
             * @brief Initialize a network packet with the specified capacity.
             *
             * Allocates memory for the packet data buffer and sets the initial
             * capacity. The packet structure must be provided and will be set up
             * for further operations.
             *
             * @param pkt Pointer to the packet structure to initialize.
             * @param capacity Number of bytes to allocate for the packet data.
             * @return 0 on success, non-zero on failure.
             */
            static int init(fossil_network_packet_t *pkt, size_t capacity) {
                return fossil_network_packet_init(pkt, capacity);
            }

            /**
             * @brief Append data to the packet.
             *
             * Adds the specified data to the end of the packet's data buffer.
             * The buffer will be resized if necessary to accommodate the new data.
             *
             * @param pkt Pointer to the packet structure.
             * @param data Pointer to the data to append.
             * @param len Length of the data to append.
             * @return 0 on success, non-zero on failure.
             */
            static int append(fossil_network_packet_t *pkt, const void *data, size_t len) {
                return fossil_network_packet_append(pkt, data, len);
            }

            /**
             * @brief Parse the IP header from the packet data.
             *
             * Extracts the IP header from the packet's data buffer and copies it
             * to the provided output buffer. The output buffer must be large enough
             * to hold the IP header structure.
             *
             * @param pkt Pointer to the packet structure.
             * @param header_out Pointer to a buffer to receive the parsed IP header.
             * @return 0 on success, non-zero on failure.
             */
            static int parse_ip(fossil_network_packet_t *pkt, void *header_out) {
                return fossil_network_packet_parse_ip(pkt, header_out);
            }

            /**
             * @brief Free resources associated with the packet.
             *
             * Releases any memory allocated for the packet's data buffer and resets
             * the structure's fields. The packet structure itself is not freed.
             *
             * @param pkt Pointer to the packet structure to free.
             */
            static void free(fossil_network_packet_t *pkt) {
                fossil_network_packet_free(pkt);
            }
        };

    } // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_PROTOCOL_H */
