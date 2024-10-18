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
#include "fossil/network/internal.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

/**
 * @brief Allocate memory.
 * 
 * @param size Size of the memory to allocate.
 * @return Pointer to the allocated memory, or NULL if allocation fails or size is 0.
 */
network_memory_t fossil_network_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    network_memory_t ptr = malloc(size);
    if (!ptr) {
        return NULL;
    }
    return ptr;
} // end of fun

/**
 * @brief Reallocate memory.
 * 
 * @param ptr Pointer to the memory to reallocate.
 * @param size Size of the memory to reallocate.
 * @return Pointer to the reallocated memory, or NULL if reallocation fails.
 */
network_memory_t fossil_network_realloc(network_memory_t ptr, size_t size) {
    if (size == 0) {
        fossil_network_free(ptr);
        return NULL;
    }

    network_memory_t new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        return NULL; // Return NULL if reallocation fails
    }

    return new_ptr;
} // end of fun

/**
 * @brief Free memory.
 * 
 * @param ptr Pointer to the memory to free.
 */
void fossil_network_free(network_memory_t ptr) {
    if (ptr) {
        free(ptr);
    } // No need to exit if ptr is NULL
} // end of fun

/**
 * @brief Duplicate a string.
 * 
 * @param str String to duplicate.
 * @return Pointer to the duplicated string, or NULL if allocation fails.
 */
char* fossil_network_strdup(const char* str) {
    if (!str) return NULL; // Handle NULL pointer gracefully

    size_t len = 0;
    while (str[len] != '\0') len++; // Calculate the length of the string

    char* dup = fossil_network_alloc((len + 1) * sizeof(char)); // Allocate memory for the duplicate string
    if (!dup) return NULL; // Return NULL if allocation fails

    for (size_t i = 0; i < len; i++) {
        dup[i] = str[i]; // Copy each character from the original string to the duplicate
    }
    dup[len] = '\0'; // Add null terminator to the end of the duplicate string

    return dup;
} // end of fun
