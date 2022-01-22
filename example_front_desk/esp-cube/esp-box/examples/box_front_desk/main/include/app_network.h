/**
 * @file app_network.h
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

#pragma once

#include "esp_err.h"

#define EXAMPLE_ESP_WIFI_SSID       "TP-LINK-2.4-zori"
#define EXAMPLE_ESP_WIFI_PASS       "a18281132"
#define EXAMPLE_ESP_WIFI_CHANNEL    1
#define EXAMPLE_MAX_STA_CONN        2
#define CONFIG_MANUAL_DNS 1
#define CONFIG_MANUAL_DNS_ADDR "114.114.114.114"
#define CONFIG_AP_SSID "esp-guest-304"
#define CONFIG_AP_PASSWORD "a18281132"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start network task
 * 
 * @param host_name Server host name
 * @return 
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t app_network_start(const char *host_name);
void network_task(void);
#ifdef __cplusplus
}
#endif
