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
#include "fossil/network/ws.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <windows.h>
  #include <bcrypt.h>
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <errno.h>
#endif

// ------------------------------
// Base64 encoding (for Sec-WebSocket-Key)
// ------------------------------
static const char fossil_b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char *fossil_base64_encode(const uint8_t *input, size_t len) {
    size_t out_len = 4 * ((len + 2) / 3);
    char *out = (char *)malloc(out_len + 1);
    if (!out) return NULL;

    size_t i, j;
    for (i = 0, j = 0; i < len;) {
        uint32_t octet_a = i < len ? input[i++] : 0;
        uint32_t octet_b = i < len ? input[i++] : 0;
        uint32_t octet_c = i < len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        out[j++] = fossil_b64_table[(triple >> 18) & 0x3F];
        out[j++] = fossil_b64_table[(triple >> 12) & 0x3F];
        out[j++] = (i > len + 1) ? '=' : fossil_b64_table[(triple >> 6) & 0x3F];
        out[j++] = (i > len)     ? '=' : fossil_b64_table[triple & 0x3F];
    }
    out[j] = '\0';
    return out;
}

// ------------------------------
// WebSocket Handshake
// ------------------------------
int fossil_network_ws_handshake(fossil_network_socket_t *sock,
                                const char *host, const char *path) {
    if (!sock || !host || !path) return -1;

    // Generate random 16-byte key
    uint8_t key_raw[16];
#ifdef _WIN32
    if (BCryptGenRandom(NULL, key_raw, sizeof(key_raw), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0)
        return -1;
#else
    FILE *urand = fopen("/dev/urandom", "rb");
    if (!urand || fread(key_raw, 1, sizeof(key_raw), urand) != sizeof(key_raw)) {
        if (urand) fclose(urand);
        return -1;
    }
    fclose(urand);
#endif

    char *key_b64 = fossil_base64_encode(key_raw, sizeof(key_raw));
    if (!key_b64) return -1;

    // Build HTTP upgrade request
    char req[1024];
    snprintf(req, sizeof(req),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Upgrade: websocket\r\n"
             "Connection: Upgrade\r\n"
             "Sec-WebSocket-Key: %s\r\n"
             "Sec-WebSocket-Version: 13\r\n"
             "\r\n",
             path, host, key_b64);

    free(key_b64);

    // Send request
    if (send(sock->fd, req, (int)strlen(req), 0) < 0)
        return -1;

    // Receive response
    char resp[2048];
    int n = recv(sock->fd, resp, sizeof(resp) - 1, 0);
    if (n <= 0) return -1;
    resp[n] = '\0';

    // Must contain "101 Switching Protocols"
    if (!strstr(resp, "101")) return -1;
    if (!strstr(resp, "Sec-WebSocket-Accept")) return -1;

    return 0;
}

// ------------------------------
// Frame Helpers
// ------------------------------
static int fossil_ws_send_frame(fossil_network_socket_t *sock,
                                int opcode, const char *msg, size_t len) {
    if (!sock || sock->fd == (fossil_socket_fd_t)-1) return -1;

    uint8_t header[14];
    size_t header_len = 0;

    header[0] = 0x80 | (opcode & 0x0F); // FIN=1, opcode
    if (len <= 125) {
        header[1] = (uint8_t)len;
        header_len = 2;
    } else if (len <= 0xFFFF) {
        header[1] = 126;
        header[2] = (len >> 8) & 0xFF;
        header[3] = len & 0xFF;
        header_len = 4;
    } else {
        header[1] = 127;
        for (int i = 0; i < 8; i++)
            header[2 + i] = (uint8_t)((len >> (56 - i * 8)) & 0xFF);
        header_len = 10;
    }

    if (send(sock->fd, (const char *)header, (int)header_len, 0) < 0) return -1;
    if (len > 0 && send(sock->fd, msg, (int)len, 0) < 0) return -1;

    return 0;
}

static int fossil_ws_recv_frame(fossil_network_socket_t *sock,
                                char *buf, size_t len) {
    uint8_t header[2];
    if (recv(sock->fd, (char *)header, 2, 0) != 2) return -1;

    int opcode = header[0] & 0x0F;
    size_t payload_len = header[1] & 0x7F;

    if (payload_len == 126) {
        uint8_t ext[2];
        if (recv(sock->fd, (char *)ext, 2, 0) != 2) return -1;
        payload_len = (ext[0] << 8) | ext[1];
    } else if (payload_len == 127) {
        uint8_t ext[8];
        if (recv(sock->fd, (char *)ext, 8, 0) != 8) return -1;
        payload_len = 0;
        for (int i = 0; i < 8; i++)
            payload_len = (payload_len << 8) | ext[i];
    }

    if (payload_len >= len) return -1; // buffer too small

    if (recv(sock->fd, buf, (int)payload_len, 0) != (int)payload_len)
        return -1;

    buf[payload_len] = '\0';
    return opcode;
}

// ------------------------------
// Public WebSocket API
// ------------------------------
int fossil_network_ws_send_text(fossil_network_socket_t *sock,
                                const char *msg) {
    return fossil_ws_send_frame(sock, 0x1, msg, strlen(msg));
}

int fossil_network_ws_recv_text(fossil_network_socket_t *sock,
                                char *buf, size_t len) {
    int opcode = fossil_ws_recv_frame(sock, buf, len);
    if (opcode != 0x1) return -1; // only accept text frames
    return 0;
}
