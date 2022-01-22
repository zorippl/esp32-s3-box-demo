/**
 * @file app_server.c
 * @brief 
 * @version 0.1
 * @date 2021-09-26
 * 
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include "app_led.h"
#include "app_data_parse.h"
#include "cJSON.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "ui_lang.h"
#include "jsmn.h"
#include "json_parser.h"

#include "esp_netif.h"

#include "esp_console.h"
#include "dnserver.h"

static const char *TAG = "esp-server";
#define DNS_SERVER_PORT 80

static const ip_addr_t ipaddr = IPADDR4_INIT_BYTES(192, 168, 4, 1);


#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX   (128)
#define SCRATCH_BUFSIZE (4096)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    ESP_LOGW(TAG, "%s", __func__);

    ESP_LOGI(TAG, "%s", req->uri);
    char filepath[128];
    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;

    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, INDEX_NAME, sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    int read_bytes;
    
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
    
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}


/* handle any DNS requests from dns-server */
static bool dns_query_proc(const char *name, ip_addr_t *addr)
{
    /**
     * captive: enerate_204, cp.a, hotspot-detect.html
     */
    ESP_LOGW(TAG, "name: %s", name);

    *addr = ipaddr;
    return true;
}

static esp_err_t guest_post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
            ESP_LOGW(TAG, "%d", __LINE__);


        received = httpd_req_recv(req, buf + cur_len, total_len);//here is problem GG
        
        ESP_LOGW(TAG, "%d", __LINE__);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    char* q1 = cJSON_GetObjectItem(root, "q1")->valuestring;
    char* q2 = cJSON_GetObjectItem(root, "q2")->valuestring;
    char* q3 = cJSON_GetObjectItem(root, "q3")->valuestring;
    char* q4 = cJSON_GetObjectItem(root, "q4")->valuestring;
    int q5 = cJSON_GetObjectItem(root, "q5")->valueint;
    char* q5_value = cJSON_GetObjectItem(root, "q6")->valuestring;
    int q6 = cJSON_GetObjectItem(root, "q6_value")->valueint;
    char* q7 = cJSON_GetObjectItem(root, "q7")->valuestring;
    char* q7_2 = cJSON_GetObjectItem(root, "q7_2")->valuestring;
    int q8 = cJSON_GetObjectItem(root, "q8")->valueint;

    ESP_LOGI(TAG, "Light control: red = %s, green = %s, blue = %s", q1, q2, q3);
    cJSON_Delete(root);


    httpd_resp_sendstr(req, "Post control value successfully");
    return ESP_OK;
}

esp_err_t start_rest_server(const char *base_path)
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = heap_caps_calloc(1, sizeof(rest_server_context_t), MALLOC_CAP_INTERNAL);
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.recv_wait_timeout  = 30;
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    httpd_uri_t guest_post_url = {
        .uri        = "/guest",
        .method     = HTTP_PUT,
        .handler    = guest_post_handler,
        .user_ctx   = rest_context
    };

    httpd_register_uri_handler(server, &guest_post_url);

    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);

    dnserv_init(&ipaddr, 80, dns_query_proc);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}
