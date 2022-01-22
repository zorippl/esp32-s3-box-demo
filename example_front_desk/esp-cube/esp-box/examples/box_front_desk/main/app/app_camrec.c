

#include <stdbool.h>
#include <stdio.h>
#include "esp_err.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "libjpeg.h"

#include "lvgl.h"
#include "ui_main.h"

#include "app_audio.h"

/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "192.168.1.109"
#define WEB_PORT "80"
#define WEB_PATH "/image"

#define MAX_DATA_SIZE   (16 * 1024)

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
    "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";

static uint8_t *image_data = NULL;
extern uint16_t *image_rgb565 = NULL;

extern lv_obj_t *img_guest;
extern lv_img_dsc_t img_cam_receive;

static const char *TAG = "cam_receiver";

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
        }else {
            ESP_LOGI(TAG, "Socket send success");
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
                    ESP_LOGE(TAG, "GGGGGGGGGGGGGGGG");
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
                img_cam_receive.data = (uint8_t *) image_rgb565;
                ESP_LOGE(TAG, "refresh the picture");
                lv_event_send(img_guest, LV_EVENT_REFRESH, NULL);
                audio_play_start();
                ui_network(false);
                ui_dev_ctrl(false);
                ui_led(false);
                ui_hint(false);
                ui_clock(false);
                ui_guest(true);
                ui_connected(false);

                close(sock);
                return ESP_OK;

              /*bsp_lcd_flush(0, 0, width, 48, image_rgb565 + width * 0, portMAX_DELAY);
                bsp_lcd_flush(0, 48, width, 96, image_rgb565 + width * 48, portMAX_DELAY);
                bsp_lcd_flush(0, 96, width, 144, image_rgb565 + width * 96, portMAX_DELAY);
                bsp_lcd_flush(0, 144, width, 192, image_rgb565 + width * 144, portMAX_DELAY);
                bsp_lcd_flush(0, 192, width, 240, image_rgb565 + width * 192, portMAX_DELAY);*/
            }
        }
    }

    close(sock);
    return ESP_OK;
}
static void app_camera_receive_handle(void)
{
    while(true)
    {
        http_request();
    }
}

esp_err_t app_camrec_start(void)
{
    image_data = heap_caps_malloc(MAX_DATA_SIZE, MALLOC_CAP_INTERNAL);
    image_rgb565 = heap_caps_malloc(320 * 240 * 2, MALLOC_CAP_SPIRAM);

    if (NULL == image_data || NULL == image_rgb565) {
        ESP_LOGE("main", "No memory for image data");
        return ESP_ERR_NO_MEM;
    }

    /* Create cam receive task*/
    BaseType_t ret_val = xTaskCreatePinnedToCore(
        (TaskFunction_t)        app_camera_receive_handle,
        (const char * const)    "http_rq_task",
        (const uint32_t)        4 * 1024,
        (void * const)          NULL,
        (UBaseType_t)           4,
        (TaskHandle_t * const)  NULL,
        (const BaseType_t)      0);
    if (pdPASS != ret_val) {
        ESP_LOGE(TAG, "Failed create sr initialize task");
        return ESP_FAIL;
    }

    return ESP_OK;
}