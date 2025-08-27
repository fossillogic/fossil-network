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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define fossil_poll WSAPoll
#else
    #define fossil_poll poll
#endif

// *****************************************************************************
// Internal Structures
// *****************************************************************************
typedef struct {
    fossil_network_pollfd_t pfd;
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
    e->pfd.sock = sock;
    e->pfd.events = events;
    e->pfd.revents = 0;
    e->userdata = userdata;

    return 0;
}

int fossil_network_async_run(fossil_network_async_t *loop,
                             int timeout_ms) {
    if (!loop || loop->count == 0) return -1;

    // Build temporary pollfds
#ifdef _WIN32
    WSAPOLLFD *pfds = (WSAPOLLFD*)calloc(loop->count, sizeof(WSAPOLLFD));
#else
    struct pollfd *pfds = (struct pollfd*)calloc(loop->count, sizeof(struct pollfd));
#endif
    if (!pfds) return -1;

    for (int i = 0; i < loop->count; i++) {
        pfds[i].fd = loop->entries[i].pfd.sock->fd;
        pfds[i].events = 0;
        if (loop->entries[i].pfd.events & 1) pfds[i].events |= POLLIN;
        if (loop->entries[i].pfd.events & 2) pfds[i].events |= POLLOUT;
        if (loop->entries[i].pfd.events & 4) pfds[i].events |= POLLERR;
    }

    int ret = fossil_poll(pfds, loop->count, timeout_ms);
    if (ret > 0) {
        for (int i = 0; i < loop->count; i++) {
            loop->entries[i].pfd.revents = 0;
            if (pfds[i].revents & POLLIN)  loop->entries[i].pfd.revents |= 1;
            if (pfds[i].revents & POLLOUT) loop->entries[i].pfd.revents |= 2;
            if (pfds[i].revents & POLLERR) loop->entries[i].pfd.revents |= 4;

            if (loop->entries[i].pfd.revents) {
                printf("[async] Event mask=%d on socket %d (userdata=%p)\n",
                       loop->entries[i].pfd.revents,
                       (int)pfds[i].fd,
                       loop->entries[i].userdata);
            }
        }
    }

    free(pfds);
    return ret;
}
