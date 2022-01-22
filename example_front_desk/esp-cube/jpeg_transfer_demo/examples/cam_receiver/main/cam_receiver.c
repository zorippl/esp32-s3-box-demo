/**
 * @file image_display.c
 * @brief Display png image with LVGL
 * @version 0.1
 * @date 2021-10-19
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

#include <string.h>

#include "bsp_board.h"
#include "bsp_lcd.h"
#include "bsp_tp.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mbedtls/base64.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "libjpeg.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.200.18"
#define WEB_PORT "80"
#define WEB_PATH "/image"

#define MAX_DATA_SIZE   (16 * 1024)

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static uint8_t *image_data = NULL;
static uint16_t *image_rgb565 = NULL;

static const char *TAG = "main";

static esp_err_t http_request(void);

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());
    ESP_ERROR_CHECK(bsp_lcd_init());


    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    image_data = heap_caps_malloc(MAX_DATA_SIZE, MALLOC_CAP_INTERNAL);
    image_rgb565 = heap_caps_malloc(320 * 240 * 2, MALLOC_CAP_SPIRAM);

    if (NULL == image_data || NULL == image_rgb565) {
        ESP_LOGE("main", "No memory for image data");
    }

    while (true) {
        http_request();
    }

}

void base64_enc(void *data, size_t size);

static esp_err_t http_request(void)
{
    int ret_val = 0;
    struct addrinfo *res = NULL;
    struct in_addr *addr = NULL;

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };

    const struct timeval timeout = {
        .tv_sec = 3,
        .tv_usec = 0,
    };

    /* **************** LOOKUP FOR IP ADDRESS **************** */
    while (true) {
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);
        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        } else {
            addr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
            ESP_LOGI(TAG, "DNS lookup succeeded. IP = %s", inet_ntoa(*addr));
            break;
        }
    }    

    /* **************** CREATE SOCKET **************** */
    int sock = socket(res->ai_family, res->ai_socktype, 0);
    if(sock < 0) {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Socket allocated");

        ret_val |= setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
        ret_val |= setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        if (0 != ret_val) {
            ESP_LOGE(TAG, "Failed set timeout");
            close(sock);
            return ESP_FAIL;
        }
    }

    /* ******** ESTABLISH CONNECTION  ******** */
    if(connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "Socket connect failed");
        close(sock);
        freeaddrinfo(res);
        return ESP_FAIL;
    } else {
        ESP_LOGI(TAG, "Socket connected");
        freeaddrinfo(res);
    }

    /* **************** */
    while(vTaskDelay(1), true) {
        /* ******** SEND REQUEST ******** */
        if (write(sock, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "Socket send failed");
            close(sock);
            return ESP_FAIL;
        }

        /* ******** READING HTTP RESPONSE ******** */
        int bytes_read = 0, total_bytes = 0;
        uint8_t *read_ptr = image_data;
        const uint8_t jpeg_tail[] = { 0xff, 0xd9 };
        do {
            bytes_read = read(sock, read_ptr, MAX_DATA_SIZE);
            if (bytes_read > 0) {
                read_ptr += bytes_read;
                total_bytes += bytes_read;
                if (total_bytes > sizeof(jpeg_tail)) {
                    if (0 == memcmp(image_data + total_bytes - sizeof(jpeg_tail), jpeg_tail, sizeof(jpeg_tail))) {
                        break;
                    }
                }
            } else {
                if (bytes_read < 0) {
                    ESP_LOG_BUFFER_HEXDUMP(TAG, image_data, total_bytes, ESP_LOG_INFO);
                }
            }
        } while (bytes_read > 0);

        /* **************** FIND JPEG HEADER **************** */
        uint8_t *image_data_start = NULL;
        for (size_t i = 0; i < 256; i++) {
            if (image_data[i] == 0xff) {
                if (image_data[i + 1] == 0xd8) {
                    image_data_start = (void *) (image_data + i);
                    break;
                }
            }
        }

        if (NULL == image_data_start) {
            ESP_LOGE(TAG, "Failed find JPEG header");
            continue;
        }

        int jpeg_data_len = total_bytes - (int32_t) ((uint32_t) image_data_start - (uint32_t) image_data);
        printf("Total : %d, JPEG : %d\n", total_bytes, jpeg_data_len);

        if ((NULL != image_data_start) && (jpeg_data_len > 0)) {
            uint32_t width = 0, height = 0;
            bool suc = libjpeg_decode(image_data_start, jpeg_data_len, COLOR_TYPE_RGB565, (uint8_t *) image_rgb565, &width, &height);
            printf("Image decoded : [%u * %u]\n", width, height);
            if (width > 0 && height > 0 && suc) {
                bsp_lcd_flush(0, 0, width, 48, image_rgb565 + width * 0, portMAX_DELAY);
                bsp_lcd_flush(0, 48, width, 96, image_rgb565 + width * 48, portMAX_DELAY);
                bsp_lcd_flush(0, 96, width, 144, image_rgb565 + width * 96, portMAX_DELAY);
                bsp_lcd_flush(0, 144, width, 192, image_rgb565 + width * 144, portMAX_DELAY);
                bsp_lcd_flush(0, 192, width, 240, image_rgb565 + width * 192, portMAX_DELAY);
            }
        }
    }

    close(sock);
    return ESP_OK;
}

void base64_enc(void *data, size_t size)
{
    size_t base64_len = 0;
    uint8_t *jpeg_base64 = malloc(size * 2);
    if (NULL == jpeg_base64) {
        ESP_LOGE(TAG, "No mem for Base-64");
        return;
    }

    ESP_LOGI(TAG, "******** Base-64 Encode ********");
    mbedtls_base64_encode(jpeg_base64, size * 2, &base64_len, data, size);
    printf("Base-64 string size : %zu\n", base64_len);

    ESP_LOGI(TAG, "******** Print Image ********");
    jpeg_base64[base64_len] = '\0';
    printf("%s\n", jpeg_base64);
}

