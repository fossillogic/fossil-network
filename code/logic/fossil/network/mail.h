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
#ifndef FOSSIL_NETWORK_MAIL_H
#define FOSSIL_NETWORK_MAIL_H

#include "client.h"

#ifdef __cplusplus
extern "C"
{
#endif

// *****************************************************************************
// Function prototypes
// *****************************************************************************

// ------------------------------
// SMTP
// ------------------------------
int fossil_network_mail_smtp_send(const char *smtp_host,
                                  uint16_t port,
                                  const char *from,
                                  const char *to,
                                  const char *subject,
                                  const char *body);

// ------------------------------
// POP3 (basic inbox fetch)
// ------------------------------
int fossil_network_mail_pop3_list(const char *pop3_host,
                                  uint16_t port,
                                  const char *user,
                                  const char *pass);

int fossil_network_mail_pop3_retrieve(const char *pop3_host,
                                      uint16_t port,
                                      const char *user,
                                      const char *pass,
                                      int msg_num,
                                      char *buffer,
                                      size_t buf_size);

// ------------------------------
// IMAP (basic folder/mail fetch)
// ------------------------------
int fossil_network_mail_imap_list(const char *imap_host,
                                  uint16_t port,
                                  const char *user,
                                  const char *pass);

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

#endif /* FOSSIL_NETWORK_MAIL_H */
