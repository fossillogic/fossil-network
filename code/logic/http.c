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
#include "fossil/network/http.h"


// ------------------------------
// Send HTTP GET request
// ------------------------------
int fossil_network_http_get(fossil_network_socket_t *sock,
                            const char *path) {
    if (!sock || !path) return -1;

    char req[512];
    int n = snprintf(req, sizeof(req),
                     "GET %s HTTP/1.1\r\n"
                     "Host: localhost\r\n"
                     "Connection: close\r\n"
                     "\r\n",
                     path);

    if (n <= 0 || n >= (int)sizeof(req)) {
        return -1; // formatting error or truncated
    }

    ssize_t sent = send(sock->fd, req, (size_t)n, 0);
    return (sent == n) ? 0 : -1;
}

// ------------------------------
// Internal: read a line until CRLF
// ------------------------------
static int fossil_network_http_readline(int fd, char *buf, size_t len) {
    size_t i = 0;
    char c;
    while (i + 1 < len) {
        ssize_t r = recv(fd, &c, 1, 0);
        if (r <= 0) return -1; // connection closed or error
        if (c == '\r') continue; // skip CR
        if (c == '\n') {
            buf[i] = '\0';
            return 0;
        }
        buf[i++] = c;
    }
    buf[len - 1] = '\0';
    return 0;
}

// ------------------------------
// Read HTTP Response status line
// ------------------------------
int fossil_network_http_read_response(fossil_network_socket_t *sock,
                                      fossil_network_http_response_t *resp) {
    if (!sock || !resp) return -1;

    char line[512];
    if (fossil_network_http_readline(sock->fd, line, sizeof(line)) != 0)
        return -1;

    // Expect format: HTTP/1.1 200 OK
    char version[16];
    int status = 0;
    char reason[64] = {0};

    int n = sscanf(line, "%15s %d %63[^\r\n]",
                   version, &status, reason);
    if (n < 2) return -1;

    resp->status_code = status;
    strncpy(resp->reason, reason, sizeof(resp->reason) - 1);
    resp->reason[sizeof(resp->reason) - 1] = '\0';

    return 0;
}
