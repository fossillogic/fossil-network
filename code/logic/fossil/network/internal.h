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
#ifndef FOSSIL_NETWORK_INTERNAL_H
#define FOSSIL_NETWORK_INTERNAL_H

// common standard headers
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// network related headers
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    FOSSIL_NET_SUCCESS = 0,
    FOSSIL_NET_ERROR = -1
};

// TCP Client Types
typedef struct {
    int socket_fd;  // Socket file descriptor
} fossil_net_client_socket_t;

// TCP Socket Types
typedef struct {
    int socket_fd;  // Socket file descriptor
} fossil_net_server_socket_t;

/**
 * @brief Allocate memory.
 * 
 * @param size Size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
typedef void * network_memory_t;

/**
 * @brief Allocate memory.
 * 
 * @param size Size of the memory to allocate.
 * @return Pointer to the allocated memory.
 */
network_memory_t fossil_network_alloc(size_t size);

/**
 * @brief Reallocate memory.
 * 
 * @param ptr Pointer to the memory to reallocate.
 * @param size Size of the memory to reallocate.
 * @return Pointer to the reallocated memory.
 */
network_memory_t fossil_network_realloc(network_memory_t ptr, size_t size);

/**
 * @brief Free memory.
 * 
 * @param ptr Pointer to the memory to free.
 */
void fossil_network_free(network_memory_t ptr);

/**
 * @brief Duplicate a string.
 * 
 * @param str String to duplicate.
 * @return Pointer to the duplicated string.
 */
char* fossil_network_strdup(const char* str);

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_NETWORK_FRAMEWORK_H */
