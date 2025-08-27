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
#include "fossil/network/async.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    typedef WSAPOLLFD fossil_pollfd_t;
#else
    #include <unistd.h>
    #include <poll.h>
    #define fossil_poll poll
#endif

// *****************************************************************************
// Internal Structures
// *****************************************************************************
typedef struct {
    fossil_network_socket_t *sock;
    int events;
    void *userdata;
} fossil_async_entry_t;

struct fossil_network_async {
    fossil_async_entry_t *entries;
    int count;
    int capacity;
};

// *****************************************************************************
// Public API
// *****************************************************************************
fossil_network_async_t* fossil_network_async_create(void) {
    fossil_network_async_t *loop =
        (fossil_network_async_t*)calloc(1, sizeof(fossil_network_async_t));
    if (!loop) return NULL;

    loop->capacity = 8;
    loop->entries = (fossil_async_entry_t*)calloc(loop->capacity,
                                                  sizeof(fossil_async_entry_t));
    return loop;
}

void fossil_network_async_free(fossil_network_async_t *loop) {
    if (!loop) return;
    free(loop->entries);
    free(loop);
}

int fossil_network_async_add(fossil_network_async_t *loop,
                             fossil_network_socket_t *sock,
                             int events,
                             void *userdata) {
    if (!loop || !sock) return -1;

    if (loop->count >= loop->capacity) {
        loop->capacity *= 2;
        loop->entries = (fossil_async_entry_t*)realloc(
            loop->entries, loop->capacity * sizeof(fossil_async_entry_t));
    }

    fossil_async_entry_t *e = &loop->entries[loop->count++];
    e->sock = sock;
    e->events = events;
    e->userdata = userdata;

    return 0;
}

int fossil_network_async_run(fossil_network_async_t *loop,
                             int timeout_ms) {
    if (!loop || loop->count == 0) return -1;

    fossil_pollfd_t *pfds = (fossil_pollfd_t*)calloc(loop->count,
                                                     sizeof(fossil_pollfd_t));
    if (!pfds) return -1;

    for (int i = 0; i < loop->count; i++) {
        pfds[i].fd = loop->entries[i].sock->fd;
        pfds[i].events = 0;
        if (loop->entries[i].events & 1) pfds[i].events |= POLLIN;
        if (loop->entries[i].events & 2) pfds[i].events |= POLLOUT;
    }

    int ret = fossil_poll(pfds, loop->count, timeout_ms);
    if (ret > 0) {
        for (int i = 0; i < loop->count; i++) {
            if (pfds[i].revents & POLLIN) {
                printf("[async] Read event on socket %d (userdata=%p)\n",
                       (int)pfds[i].fd, loop->entries[i].userdata);
            }
            if (pfds[i].revents & POLLOUT) {
                printf("[async] Write event on socket %d (userdata=%p)\n",
                       (int)pfds[i].fd, loop->entries[i].userdata);
            }
        }
    }

    free(pfds);
    return ret;
}
