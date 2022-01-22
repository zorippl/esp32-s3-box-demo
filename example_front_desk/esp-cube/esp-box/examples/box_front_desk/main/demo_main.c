/**
 * @file demo_main.c
 * @brief 
 * @version 0.1
 * @date 2021-06-23
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

#include "app_audio.h"
#include "app_led.h"
#include "app_network.h"
#include "app_sr.h"
#include "app_sntp.h"
#include "app_camrec.h"

#include "bsp_board.h"
#include "bsp_lcd.h"
#include "bsp_storage.h"
#include "bsp_tp.h"

#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_port.h"
#include "lvgl.h"
#include "ui_main.h"

void app_main(void)
{
    ESP_ERROR_CHECK(bsp_board_init());
    ESP_ERROR_CHECK(bsp_board_power_ctrl(POWER_MODULE_AUDIO, true));
    ESP_ERROR_CHECK(bsp_spiffs_init("model", "/srmodel", 4));
    ESP_ERROR_CHECK(bsp_spiffs_init("storage", "/spiffs", 10));

    /* Initialize LCD and GUI */
    ESP_ERROR_CHECK(bsp_lcd_init());
    ESP_ERROR_CHECK(bsp_tp_init());
    ESP_ERROR_CHECK(lv_port_init());



    /* Start speech recognition task. I2S is initialized in task pinned to core 1 */
    ESP_ERROR_CHECK(app_audio_start());
    ESP_ERROR_CHECK(app_sr_start(false));

    //close: ESP_ERROR_CHECK(app_pwm_led_init());
    network_task();
    //you can configure ssid and pw in app_network.h
    //ESP_ERROR_CHECK(app_network_start("ESP-Box"));

    //start_sntp();
    ESP_ERROR_CHECK(app_camrec_start());
    ESP_ERROR_CHECK(ui_main_start());
    lv_task_handler();

    /* Run LVGL task handler */
    while (vTaskDelay(2), true) {
        lv_task_handler();
    }
}
