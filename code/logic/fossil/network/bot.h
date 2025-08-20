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
#ifndef FOSSIL_NETWORK_BOT_H
#define FOSSIL_NETWORK_BOT_H

#include "client.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// ------------------------------
// Bot type
// ------------------------------
typedef struct fossil_network_bot_t fossil_network_bot_t;

// Action callback (user-defined)
typedef void (*fossil_bot_action_t)(fossil_network_client_t *cli, void *userdata);

// ------------------------------
// Lifecycle
// ------------------------------
fossil_network_bot_t *fossil_network_bot_create(const char *host,
                                                uint16_t port,
                                                fossil_protocol_t proto,
                                                fossil_bot_action_t action,
                                                void *userdata);

int fossil_network_bot_run(fossil_network_bot_t *bot);

// Cleanup
int fossil_network_bot_destroy(fossil_network_bot_t *bot);

#ifdef __cplusplus
}
#include <stdexcept>
#include <vector>
#include <string>

namespace fossil {

namespace network {



} // namespace network

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_BOT_H */
