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

/**
 * @brief Opaque type representing a network bot.
 */
typedef struct fossil_network_bot_t fossil_network_bot_t;

/**
 * @brief Callback type for bot actions.
 *
 * This function is called by the bot when an action is required.
 * @param cli Pointer to the network client.
 * @param userdata User-defined data passed to the callback.
 */
typedef void (*fossil_bot_action_t)(fossil_network_client_t *cli, void *userdata);

/**
 * @brief Create a new network bot instance.
 *
 * @param host      The server hostname or IP address.
 * @param port      The server port number.
 * @param proto     The network protocol to use.
 * @param action    The user-defined action callback.
 * @param userdata  Pointer to user data for the callback.
 * @return Pointer to the created bot instance, or NULL on failure.
 */
fossil_network_bot_t *fossil_network_bot_create(const char *host,
                                                uint16_t port,
                                                fossil_protocol_t proto,
                                                fossil_bot_action_t action,
                                                void *userdata);

/**
 * @brief Run the network bot event loop.
 *
 * This function starts the bot and processes events until completion or error.
 * @param bot Pointer to the bot instance.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_network_bot_run(fossil_network_bot_t *bot);

/**
 * @brief Destroy a network bot instance and release resources.
 *
 * @param bot Pointer to the bot instance to destroy.
 * @return 0 on success, or a negative error code on failure.
 */
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
