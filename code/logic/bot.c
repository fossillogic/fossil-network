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
#include "fossil/network/bot.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ------------------------------
// Bot structure
// ------------------------------
struct fossil_network_bot_t {
    fossil_network_client_t *client;
    fossil_bot_action_t action;
    void *userdata;
};

// ------------------------------
// Create bot
// ------------------------------
fossil_network_bot_t *fossil_network_bot_create(const char *host,
                                                uint16_t port,
                                                fossil_protocol_t proto,
                                                fossil_bot_action_t action,
                                                void *userdata)
{
    fossil_network_bot_t *bot = calloc(1, sizeof(*bot));
    if (!bot) return NULL;

    bot->client = fossil_network_client_create(host, port, proto);
    if (!bot->client) {
        free(bot);
        return NULL;
    }

    bot->action = action;
    bot->userdata = userdata;
    return bot;
}

// ------------------------------
// Run bot (simple loop)
// ------------------------------
int fossil_network_bot_run(fossil_network_bot_t *bot)
{
    if (!bot || !bot->client || !bot->action)
        return -1;

    // Try connecting
    if (fossil_network_client_connect(bot->client) != 0)
        return -1;

    // Execute the bot’s action once for now
    bot->action(bot->client, bot->userdata);

    // For real bots: loop, schedule, etc.
    return 0;
}

// ------------------------------
// Destroy bot
// ------------------------------
int fossil_network_bot_destroy(fossil_network_bot_t *bot)
{
    if (!bot) return -1;

    if (bot->client) {
        fossil_network_client_destroy(bot->client);
    }
    free(bot);
    return 0;
}
