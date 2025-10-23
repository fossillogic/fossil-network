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
#include "fossil/network/packet.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>   // For IP parsing (POSIX)
#include <stdint.h>

// *****************************************************************************
// Internal Helper Macros
// *****************************************************************************
#define FOSSIL_PACKET_MIN_CAPACITY 64

// *****************************************************************************
// Initialize a packet buffer
// *****************************************************************************
int fossil_network_packet_init(fossil_network_packet_t *pkt, size_t capacity) {
    if (!pkt) return -1;

    if (capacity < FOSSIL_PACKET_MIN_CAPACITY)
        capacity = FOSSIL_PACKET_MIN_CAPACITY;

    pkt->data = (uint8_t *)malloc(capacity);
    if (!pkt->data)
        return -1;

    pkt->length = 0;
    pkt->capacity = capacity;
    pkt->proto = 0;
    pkt->src = 0;
    pkt->dst = 0;

    return 0;
}

// *****************************************************************************
// Append raw data to a packet
// *****************************************************************************
int fossil_network_packet_append(fossil_network_packet_t *pkt, const void *data, size_t len) {
    if (!pkt || !data || len == 0)
        return -1;

    // Expand buffer if needed
    if (pkt->length + len > pkt->capacity) {
        size_t new_cap = pkt->capacity * 2;
        while (new_cap < pkt->length + len)
            new_cap *= 2;

        uint8_t *new_buf = (uint8_t *)realloc(pkt->data, new_cap);
        if (!new_buf)
            return -1;

        pkt->data = new_buf;
        pkt->capacity = new_cap;
    }

    memcpy(pkt->data + pkt->length, data, len);
    pkt->length += len;

    return 0;
}

// *****************************************************************************
// Parse IPv4 header from packet (basic version)
// *****************************************************************************
typedef struct fossil_ipv4_header_t {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t total_length;
    uint16_t id;
    uint16_t frag_off;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src;
    uint32_t dst;
} fossil_ipv4_header_t;

int fossil_network_packet_parse_ip(fossil_network_packet_t *pkt, void *header_out) {
    if (!pkt || !pkt->data || pkt->length < sizeof(fossil_ipv4_header_t) || !header_out)
        return -1;

    fossil_ipv4_header_t *hdr = (fossil_ipv4_header_t *)header_out;
    memcpy(hdr, pkt->data, sizeof(fossil_ipv4_header_t));

    // Convert network to host byte order
    hdr->total_length = ntohs(hdr->total_length);
    hdr->id           = ntohs(hdr->id);
    hdr->frag_off     = ntohs(hdr->frag_off);
    hdr->checksum     = ntohs(hdr->checksum);
    pkt->src          = ntohl(hdr->src);
    pkt->dst          = ntohl(hdr->dst);
    pkt->proto        = hdr->protocol;

    return 0;
}

// *****************************************************************************
// Free packet buffer and reset
// *****************************************************************************
void fossil_network_packet_free(fossil_network_packet_t *pkt) {
    if (!pkt) return;

    if (pkt->data) {
        free(pkt->data);
        pkt->data = NULL;
    }

    pkt->length = 0;
    pkt->capacity = 0;
    pkt->proto = 0;
    pkt->src = 0;
    pkt->dst = 0;
}
