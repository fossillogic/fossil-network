/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2013
 *
 * Copyright (C) 2013-Current Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_NETWORK_REQUEST_H
#define FOSSIL_NETWORK_REQUEST_H

#include "socket.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
STRING IDS
=============================================================================*/

/*
Methods:
    get
    post
    put
    delete
    patch
    head

Content Types:
    text/plain
    application/json
    application/xml
    application/octet-stream
*/

/*=============================================================================
CORE STRUCTURES
=============================================================================*/

typedef struct fossil_net_header
{
    char key[64];
    char value[256];
} fossil_net_header_t;

typedef struct fossil_net_request
{
    char method[16];        /* get, post, etc */
    char url[512];          /* full URL or path */
    char content_type[64];  /* string ID */
    void *body;
    uint32_t body_size;

    fossil_net_header_t headers[32];
    uint32_t header_count;
} fossil_net_request_t;

typedef struct fossil_net_response
{
    int status;             /* HTTP status code */
    char content_type[64];

    void *body;
    uint32_t body_size;

    fossil_net_header_t headers[32];
    uint32_t header_count;
} fossil_net_response_t;

/*=============================================================================
REQUEST BUILDING
=============================================================================*/

/**
 * @brief Initialize a network request structure.
 *
 * Sets up the fossil_net_request_t structure with the specified HTTP method and URL.
 * Initializes all fields to default values and prepares the request for further configuration.
 *
 * @param req Pointer to the request structure to initialize.
 * @param method HTTP method as a string (e.g., "get", "post").
 * @param url Target URL or path for the request.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_request_init(
    fossil_net_request_t *req,
    const char *method,
    const char *url);

/**
 * @brief Set or update a header in the request.
 *
 * Adds a new header or updates the value of an existing header in the request's header list.
 * If the header already exists, its value is replaced.
 *
 * @param req Pointer to the request structure.
 * @param key Header name as a string.
 * @param value Header value as a string.
 * @return 0 on success, non-zero on failure (e.g., header list full).
 */
int fossil_net_request_set_header(
    fossil_net_request_t *req,
    const char *key,
    const char *value);

/**
 * @brief Set the body and content type of the request.
 *
 * Assigns the request body data and sets the content type header accordingly.
 * The body pointer is stored in the request structure, and its size is recorded.
 *
 * @param req Pointer to the request structure.
 * @param data Pointer to the body data.
 * @param size Size of the body data in bytes.
 * @param content_type MIME type of the body (e.g., "application/json").
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_request_set_body(
    fossil_net_request_t *req,
    const void *data,
    uint32_t size,
    const char *content_type);

/*=============================================================================
RESPONSE UTILITIES
=============================================================================*/

/**
 * @brief Retrieve the value of a header from a response.
 *
 * Searches for the specified header key in the response's header list and copies its value
 * into the provided buffer. The buffer is null-terminated if the header is found.
 *
 * @param res Pointer to the response structure.
 * @param key Header name to search for.
 * @param buffer Buffer to store the header value.
 * @param size Size of the buffer in bytes.
 * @return 0 on success, non-zero if the header is not found.
 */
int fossil_net_response_get_header(
    const fossil_net_response_t *res,
    const char *key,
    char *buffer,
    uint32_t size);

/*=============================================================================
EXECUTION
=============================================================================*/

/**
 * @brief Send a request over an existing socket and receive a response.
 *
 * Serializes the request, sends it through the provided socket, and waits for a response.
 * Populates the response structure with the received data.
 *
 * @param sock Pointer to an initialized and connected socket.
 * @param req Pointer to the request structure to send.
 * @param res Pointer to the response structure to populate.
 * @return 0 on success, non-zero on failure.
 */
int fossil_net_request_send(
    fossil_net_socket_t *sock,
    const fossil_net_request_t *req,
    fossil_net_response_t *res);

/*=============================================================================
LIFECYCLE
=============================================================================*/

/**
 * @brief Free resources associated with a request structure.
 *
 * Releases any dynamically allocated memory or resources held by the request.
 * After calling this function, the request structure should not be used unless re-initialized.
 *
 * @param req Pointer to the request structure to free.
 */
void fossil_net_request_free(
    fossil_net_request_t *req);

/**
 * @brief Free resources associated with a response structure.
 *
 * Releases any dynamically allocated memory or resources held by the response.
 * After calling this function, the response structure should not be used unless re-initialized.
 *
 * @param res Pointer to the response structure to free.
 */
void fossil_net_response_free(
    fossil_net_response_t *res);

#ifdef __cplusplus
}
#include <string>
#include <cstring>

namespace fossil::net
{

    class Request
    {
    public:
        /**
         * @brief Construct a new Request object with the specified HTTP method and URL.
         *
         * Initializes the underlying fossil_net_request_t structure.
         *
         * @param method HTTP method (e.g., "get", "post").
         * @param url Target URL or path for the request.
         */
        Request(const std::string &method, const std::string &url)
        {
            fossil_net_request_init(&req_, method.c_str(), url.c_str());
        }

        /**
         * @brief Destructor for the Request object.
         *
         * Frees any resources associated with the underlying request structure.
         */
        ~Request()
        {
            fossil_net_request_free(&req_);
        }

        /**
         * @brief Set a header key-value pair for the request.
         *
         * Adds or updates a header in the request.
         *
         * @param key Header name.
         * @param value Header value.
         * @return 0 on success, non-zero on failure.
         */
        int set_header(const std::string &key, const std::string &value)
        {
            return fossil_net_request_set_header(&req_, key.c_str(), value.c_str());
        }

        /**
         * @brief Set the body of the request.
         *
         * Assigns the request body data and content type.
         *
         * @param data Pointer to the body data.
         * @param size Size of the body data in bytes.
         * @param content_type MIME type of the body (e.g., "application/json").
         * @return 0 on success, non-zero on failure.
         */
        int set_body(const void *data, uint32_t size, const std::string &content_type)
        {
            return fossil_net_request_set_body(&req_, data, size, content_type.c_str());
        }

        /**
         * @brief Get a pointer to the underlying native request structure.
         *
         * Allows direct access to the fossil_net_request_t for advanced use cases.
         *
         * @return Pointer to the internal fossil_net_request_t.
         */
        fossil_net_request_t *native_handle()
        {
            return &req_;
        }

        /**
         * @brief Create a GET request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for GET.
         */
        static Request get(const std::string &url)
        {
            return Request("get", url);
        }

        /**
         * @brief Create a POST request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for POST.
         */
        static Request post(const std::string &url)
        {
            return Request("post", url);
        }

        /**
         * @brief Create a PUT request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for PUT.
         */
        static Request put(const std::string &url)
        {
            return Request("put", url);
        }

        /**
         * @brief Create a DELETE request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for DELETE.
         */
        static Request del(const std::string &url)
        {
            return Request("delete", url);
        }

        /**
         * @brief Create a PATCH request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for PATCH.
         */
        static Request patch(const std::string &url)
        {
            return Request("patch", url);
        }

        /**
         * @brief Create a HEAD request for the specified URL.
         *
         * @param url Target URL or path.
         * @return Request object initialized for HEAD.
         */
        static Request head(const std::string &url)
        {
            return Request("head", url);
        }

    private:
        fossil_net_request_t req_{};
    };

} // namespace fossil

#endif

#endif /* FOSSIL_NETWORK_SOCKET_H */
