/**
 * @file app_network.c
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
#include "app_server.h"
#include "app_network.h"
#include "app_sntp.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "lwip/apps/netbiosns.h"
#include "mdns.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "ui_main.h"

static const char *TAG = "app_network";
/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group = NULL;
const int WIFI_CONNECTED_BIT = BIT0;

extern SemaphoreHandle_t app_network_xSemaphore = NULL; 

static void initialise_mdns(const char *file_name)
{
    mdns_init();
    mdns_hostname_set("esp-box");
    mdns_instance_name_set("esp-box");

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "ESP32-S3-Box"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(
        mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
        sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}

static void soft_ap_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (WIFI_EVENT_AP_STACONNECTED == event_id) {
        ui_network_set_state(true);
    }

    if (WIFI_EVENT_AP_STADISCONNECTED == event_id) {
        ui_network_set_state(false);
    }
}

static void start_soft_ap(void)
{
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &soft_ap_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pairwise_cipher = WIFI_CIPHER_TYPE_CCMP,
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "Connected with IP Address:" IPSTR, IP2STR(&event->ip_info.ip));
        /* Signal main application to continue execution */
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Disconnected. Connecting to the AP again...");
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "STA Connecting to the AP again...");
        ui_network_set_state(true);
    }
}

esp_netif_t *app_wifi_init(wifi_mode_t mode)
{
    if (s_wifi_event_group) {
        return NULL;
    }

    esp_netif_t *wifi_netif = NULL;

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    //ESP_ERROR_CHECK(esp_event_loop_create_default());

    if (mode & WIFI_MODE_STA) {
        wifi_netif = esp_netif_create_default_wifi_sta();
    }

    if (mode & WIFI_MODE_AP) {
        wifi_netif = esp_netif_create_default_wifi_ap();
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* Register our event handler for Wi-Fi, IP and Provisioning related events */
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, event_handler, NULL, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(mode));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_start());

    return wifi_netif;
}

esp_err_t app_wifi_set(wifi_mode_t mode, const char *ssid, const char *password)
{
    wifi_config_t wifi_cfg = {0};

    if (mode & WIFI_MODE_STA) {
        strlcpy((char *)wifi_cfg.sta.ssid, ssid, sizeof(wifi_cfg.sta.ssid));
        strlcpy((char *)wifi_cfg.sta.password, password, sizeof(wifi_cfg.sta.password));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_cfg));

        ESP_LOGI(TAG, "sta ssid: %s password: %s", ssid, password);
    }

    if (mode & WIFI_MODE_AP) {
        wifi_cfg.ap.max_connection = 10;
        wifi_cfg.ap.authmode = strlen(password) < 8 ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
        strlcpy((char *)wifi_cfg.ap.ssid, ssid, sizeof(wifi_cfg.ap.ssid));
        strlcpy((char *)wifi_cfg.ap.password, password, sizeof(wifi_cfg.ap.password));

        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_cfg));

        ESP_LOGI(TAG, "softap ssid: %s password: %s", ssid, password);
    }

    return ESP_OK;
}

esp_err_t app_wifi_sta_connected(uint32_t wait_ms)
{
    esp_wifi_connect();
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        true, true, pdMS_TO_TICKS(wait_ms));
    return ESP_OK;
}

static void network_task(void *pvParam)
{
    char *host_name = (char *) pvParam;

    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    initialise_mdns(host_name);
    netbiosns_init();
    netbiosns_set_name(host_name);
    /* Create STA netif */
    esp_netif_t *sta_wifi_netif = app_wifi_init(WIFI_MODE_STA);
    app_wifi_set(WIFI_MODE_STA, CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
    app_wifi_sta_connected(portMAX_DELAY);
    start_sntp();

    
    /* Start soft-AP */
    start_soft_ap();

    /* Start example connection if you want to make ESP32-S3 as station */
    // ESP_ERROR_CHECK(example_connect());

    start_rest_server("/spiffs/web");

    vTaskDelete(NULL);
}

esp_err_t app_network_start(const char *host_name)
{
    //xSemaphoreGive( app_network_xSemaphore );
    BaseType_t ret_val = xTaskCreatePinnedToCore(
        (TaskFunction_t)        network_task,
        (const char * const)    "Network Task",
        (const uint32_t)        4 * 1024,
        (void * const)          host_name,
        (UBaseType_t)           5,
        (TaskHandle_t * const)  NULL,
        (const BaseType_t)      1);
    if (pdPASS != ret_val) {
        ESP_LOGE(TAG, "Failed create Network task");
        return ESP_FAIL;
    }

    return ESP_OK;
}
