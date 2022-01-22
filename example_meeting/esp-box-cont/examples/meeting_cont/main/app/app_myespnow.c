/* Get Start Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_crc.h"
#include "esp_netif.h"

#include "espnow.h"
//#include "app_espnow.h"
#include "app_audio.h"
#include "app_myespnow.h"
#include "cJSON.h"

static const char *TAG = "my_espnow";

esp_err_t set_light_status(light_name_t light_name, bool status)
{
    espnow_frame_head_t frame_head = {
        .retransmit_count = 10,
        .broadcast        = true,
        .forward_ttl      = 16,
        .forward_rssi     = -25,
    };

    /**
     *                  L2(4c11aeb8c3c8)        //door
     * L1(98cdacaacfe4) L3(246f28d72a88)
     *                  L2(4c11aeb8c3c8)
     */
    uint8_t addr_list[3][6] = {
        {0x98, 0xcd, 0xac, 0xaa, 0xcf, 0xe4},
        {0x4c, 0x11, 0xae, 0xb8, 0xc3, 0xc8},
        {0x24, 0x6f, 0x28, 0xd7, 0x2a, 0x88},
    };

    if (light_name != PROJECTOR_LIGHT) {
        status = !status;
    }

    char command_gpio[32] = {0};
    sprintf(command_gpio, "gpio --set 4 --level %d", status);

    ESP_LOGI(TAG, "command: %s", command_gpio);

    esp_err_t ret = espnow_send(ESPNOW_TYPE_DEBUG_COMMAND, addr_list[light_name],
                                command_gpio, strlen(command_gpio) + 1, &frame_head, portMAX_DELAY);
    ESP_ERROR_RETURN(ret != ESP_OK, ret, "espnow_send");

    return ESP_OK;
}

static void command_handle_task(void *arg)
{
    esp_err_t ret  = ESP_OK;
    char *data     = ESP_MALLOC(ESPNOW_DATA_LEN);
    size_t size    = ESPNOW_DATA_LEN;
    uint8_t addr[ESPNOW_ADDR_LEN] = {0};
    wifi_pkt_rx_ctrl_t rx_ctrl    = {0};


    ESP_LOGI(TAG, "command_handle_task");

    for (;;) {
        memset(data, 0, ESPNOW_DATA_LEN);
        ret = espnow_recv(ESPNOW_TYPE_DATA, addr, data, &size, &rx_ctrl, portMAX_DELAY);
        ESP_ERROR_CONTINUE(ret != ESP_OK, "<%s>", esp_err_to_name(ret));

        ESP_LOGI(TAG, "mac: " MACSTR ", size: %d, data: %s", MAC2STR(addr), size, data);

        cJSON *root = cJSON_Parse(data);
        ESP_ERROR_GOTO(root == NULL, MEM_FREE, "parse fail, size: %d, data: %s", size, data);
        cJSON *room_name = cJSON_GetObjectItem(root, "room_name");
        ESP_ERROR_GOTO(room_name == NULL, MEM_FREE, "parse fail, size: %d, data: %s", size, data);
        cJSON *room_status = cJSON_GetObjectItem(root, "room_status");
        ESP_ERROR_GOTO(room_name == NULL, MEM_FREE, "parse fail, size: %d, data: %s", size, data);

        switch (room_status->valueint) {
            case COMMAND_NOBODY_MOVE:
                ESP_LOGI(TAG, "COMMAND_NOBODY_MOVE");
            break;

            case COMMAND_SOMEONE_MOVE:
                ESP_LOGI(TAG, "COMMAND_SOMEONE_MOVE");
            break;

            case COMMAND_SOMEONE_ENTERS:
                ESP_LOGI(TAG, "Someone entered the meeting room");
                set_light_status(PROJECTOR_LIGHT, true);
                set_light_status(DOWNLIGHT, true);
                set_light_status(MAIN_LIGHT, true);
                app_audio_send_play_signal(COMINGIN);
                break;
            
            default:
                break;
        }

MEM_FREE:
        cJSON_Delete(root);
    }

    ESP_FREE(data);

    vTaskDelete(NULL);
}

esp_err_t app_myespnow_init(void)
{
    esp_event_loop_create_default();

    ESP_ERROR_CHECK(esp_netif_init());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));//diff
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    //ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_channel(13, 0);

    espnow_config_t espnow_config = ESPNOW_INIT_CONFIG_DEFAULT();
    espnow_config.qsize.data      = 8;
    espnow_init(&espnow_config);
    return ESP_OK;
}

void app_myespnow_start(void)
{
    xTaskCreate(command_handle_task, "command_handle", 4 * 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
}