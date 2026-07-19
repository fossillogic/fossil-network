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
#include "fossil/network/framework.h"
#include <fossil/maip/framework.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_SUITE(c_request_fixture);

FOSSIL_SETUP(c_request_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_request_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_request_test_request_init) {
    fossil_net_request_t req;
    int rc = fossil_net_request_init(&req, "get", "http://example.com");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(strcmp(req.method, "get") == 0);
    ASSUME_ITS_TRUE(strcmp(req.url, "http://example.com") == 0);
    ASSUME_ITS_TRUE(strcmp(req.content_type, "text/plain") == 0);
    ASSUME_ITS_TRUE(req.body == NULL);
    ASSUME_ITS_TRUE(req.body_size == 0);
    ASSUME_ITS_TRUE(req.header_count == 0);
}

FOSSIL_TEST(c_request_test_request_init_invalid_method) {
    fossil_net_request_t req;
    int rc = fossil_net_request_init(&req, "invalid", "http://example.com");
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST(c_request_test_request_init_null_params) {
    fossil_net_request_t req;
    ASSUME_ITS_TRUE(fossil_net_request_init(NULL, "get", "http://example.com") != 0);
    ASSUME_ITS_TRUE(fossil_net_request_init(&req, NULL, "http://example.com") != 0);
    ASSUME_ITS_TRUE(fossil_net_request_init(&req, "get", NULL) != 0);
}

FOSSIL_TEST(c_request_test_request_set_header) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    int rc = fossil_net_request_set_header(&req, "Authorization", "Bearer token123");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(req.header_count == 1);
    ASSUME_ITS_TRUE(strcmp(req.headers[0].key, "Authorization") == 0);
    ASSUME_ITS_TRUE(strcmp(req.headers[0].value, "Bearer token123") == 0);
}

FOSSIL_TEST(c_request_test_request_set_header_update) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    fossil_net_request_set_header(&req, "User-Agent", "OldAgent");
    int rc = fossil_net_request_set_header(&req, "User-Agent", "NewAgent");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(req.header_count == 1);
    ASSUME_ITS_TRUE(strcmp(req.headers[0].value, "NewAgent") == 0);
}

FOSSIL_TEST(c_request_test_request_set_header_multiple) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    fossil_net_request_set_header(&req, "Accept", "application/json");
    fossil_net_request_set_header(&req, "User-Agent", "TestClient");
    ASSUME_ITS_TRUE(req.header_count == 2);
}

FOSSIL_TEST(c_request_test_request_set_body_json) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    const char *json_data = "{\"key\": \"value\"}";
    int rc = fossil_net_request_set_body(&req, json_data, strlen(json_data), "application/json");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(req.body == json_data);
    ASSUME_ITS_TRUE(req.body_size == strlen(json_data));
    ASSUME_ITS_TRUE(strcmp(req.content_type, "application/json") == 0);
}

FOSSIL_TEST(c_request_test_request_set_body_xml) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    const char *xml_data = "<root><item>test</item></root>";
    int rc = fossil_net_request_set_body(&req, xml_data, strlen(xml_data), "application/xml");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(strcmp(req.content_type, "application/xml") == 0);
}

FOSSIL_TEST(c_request_test_request_set_body_invalid_type) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    const char *data = "test";
    int rc = fossil_net_request_set_body(&req, data, strlen(data), "invalid/type");
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST(c_request_test_request_set_body_octet_stream) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "post", "http://example.com");
    unsigned char binary_data[] = {0x00, 0x01, 0x02, 0x03};
    int rc = fossil_net_request_set_body(&req, binary_data, sizeof(binary_data), "application/octet-stream");
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(strcmp(req.content_type, "application/octet-stream") == 0);
}

FOSSIL_TEST(c_request_test_response_get_header) {
    fossil_net_response_t res;
    memset(&res, 0, sizeof(res));
    fossil_net_header_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    strcpy(hdr.key, "Content-Type");
    strcpy(hdr.value, "application/json");
    res.headers[0] = hdr;
    res.header_count = 1;
    
    char buffer[256];
    int rc = fossil_net_response_get_header(&res, "Content-Type", buffer, sizeof(buffer));
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(strcmp(buffer, "application/json") == 0);
}

FOSSIL_TEST(c_request_test_response_get_header_not_found) {
    fossil_net_response_t res;
    memset(&res, 0, sizeof(res));
    char buffer[256];
    int rc = fossil_net_response_get_header(&res, "Missing-Header", buffer, sizeof(buffer));
    ASSUME_ITS_TRUE(rc != 0);
}

FOSSIL_TEST(c_request_test_request_free) {
    fossil_net_request_t req;
    fossil_net_request_init(&req, "get", "http://example.com");
    req.header_count = 2;
    fossil_net_request_free(&req);
    ASSUME_ITS_TRUE(req.body == NULL);
    ASSUME_ITS_TRUE(req.body_size == 0);
    ASSUME_ITS_TRUE(req.header_count == 0);
}

FOSSIL_TEST(c_request_test_response_free) {
    fossil_net_response_t res;
    memset(&res, 0, sizeof(res));
    res.body = malloc(100);
    res.body_size = 100;
    res.header_count = 1;
    fossil_net_response_free(&res);
    ASSUME_ITS_TRUE(res.body == NULL);
    ASSUME_ITS_TRUE(res.body_size == 0);
    ASSUME_ITS_TRUE(res.header_count == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_request_tests) {
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_init);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_init_invalid_method);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_init_null_params);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_header);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_header_update);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_header_multiple);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_body_json);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_body_xml);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_body_invalid_type);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_set_body_octet_stream);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_response_get_header);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_response_get_header_not_found);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_request_free);
    FOSSIL_ADD_TEST(c_request_fixture, c_request_test_response_free);

    FOSSIL_ADD_SUITE(c_request_fixture);
} // end of tests
