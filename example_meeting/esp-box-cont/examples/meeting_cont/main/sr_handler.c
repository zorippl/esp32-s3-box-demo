/**
 * @file sr_handler.c
 * @brief 
 * @version 0.1
 * @date 2021-09-19
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
// #include "app_led.h"
#include "app_sr.h"
#include "app_ir.h"
// #include "app_data_parse.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "ui_main.h"
#include "app_myespnow.h"

static const char *TAG = "sr_handler";

void sr_handler_task(void *pvParam)
{
    EventGroupHandle_t event_group_handle = (EventGroupHandle_t) pvParam;

    while (true) {
        /* Wait for event(s) sent from audio_detect_task */
        EventBits_t event_val = xEventGroupWaitBits(event_group_handle, SR_EVENT_ALL, true, false, portMAX_DELAY);

        if (SR_EVENT_TIMEOUT & event_val) {
            app_audio_send_play_signal(WARNING);
            sr_anim_set_text(STR_TIMEOUT);
            sr_anim_stop();
            ESP_LOGW(TAG, LOG_BOLD(LOG_COLOR_BROWN) "Timeout");
            continue;
        }

        if (SR_EVENT_WAKE_UP & event_val) {
            sr_anim_start();
            sr_anim_set_text(STR_WAKEWORD);
            app_audio_send_play_signal(WAKE);
            ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "Say command");
            continue;
        }

        if (SR_EVENT_WORD_DETECT & event_val) {
            int32_t cmd_id = app_sr_get_last_cmd_id();
            //sr_anim_stop();

            /* **************** REGISTER COMMAND CALLBACK HERE **************** */

            /* Register your callback here. You can get command id from cmd_id  */
            switch (cmd_id) {
            case SR_CMD_UP:
                sr_anim_set_text(STR_RMT_UP);
                rmt_send_cmd_with_id(RMT_BTN_UP);

                break;
            case SR_CMD_DOWN:
                sr_anim_set_text(STR_RMT_DOWN);
                rmt_send_cmd_with_id(RMT_BTN_DOWN);

                break;
            case SR_CMD_LEFT:
                sr_anim_set_text(STR_RMT_LEFT);
                rmt_send_cmd_with_id(RMT_BTN_LEFT);

                break;
            case SR_CMD_RIGHT:
                sr_anim_set_text(STR_RMT_RIGHT);
                rmt_send_cmd_with_id(RMT_BTN_RIGHT);

                break;
            case SR_CMD_OK:
                sr_anim_set_text(STR_RMT_OK);
                rmt_send_cmd_with_id(RMT_BTN_OK);

                break;
            case SR_CMD_PROJECTOR_ON:
                sr_anim_set_text(STR_RMT_PROJECTOR_ON);
                rmt_send_cmd_with_id(RMT_BTN_ON);
                set_light_status(PROJECTOR_LIGHT, false);
                set_light_status(DOWNLIGHT, false);
                set_light_status(MAIN_LIGHT, false);

                break;
            case SR_CMD_VOL_UP:
                sr_anim_set_text(STR_RMT_VOL_UP);
                rmt_send_cmd_with_id(RMT_BTN_VOL_UP);

                break;
            case SR_CMD_VOL_DOWN:
                sr_anim_set_text(STR_RMT_VOL_DOWN);
                rmt_send_cmd_with_id(RMT_BTN_VOL_DOWN);

                break;
            case SR_CMD_LIGHT_ON:
                sr_anim_set_text(STR_LIGHT_ON);
                set_light_status(PROJECTOR_LIGHT, true);
                set_light_status(DOWNLIGHT, true);
                set_light_status(MAIN_LIGHT, true);

                break;
            case SR_CMD_LIGHT_OFF:
                sr_anim_set_text(STR_LIGHT_OFF);
                set_light_status(PROJECTOR_LIGHT, false);
                set_light_status(DOWNLIGHT, false);
                set_light_status(MAIN_LIGHT, false);

                break;
            case SR_CMD_BOTH_ON:
                sr_anim_set_text(STR_RMT_BOTH_ON);
                rmt_send_cmd_with_id(RMT_BTN_ON);
                set_light_status(PROJECTOR_LIGHT, false);
                set_light_status(DOWNLIGHT, false);
                set_light_status(MAIN_LIGHT, false);

                break;
            case SR_CMD_PROJECTOR_OFF:
                sr_anim_set_text(STR_RMT_PROJECTOR_OFF);
                set_light_status(PROJECTOR_LIGHT, true);
                set_light_status(DOWNLIGHT, true);
                set_light_status(MAIN_LIGHT, true);
                rmt_send_cmd_with_id(RMT_BTN_ON);
                vTaskDelay(1000 /  portTICK_PERIOD_MS);
                rmt_send_cmd_with_id(RMT_BTN_ON);

            default:
                /* Defalut handler */
                ESP_LOGI(TAG, LOG_BOLD(LOG_COLOR_GREEN) "Detected : %d", cmd_id);
                break;
            }
            
            /* **************** ENDS REGISTER COMMAND CALLBACK **************** */
        }
    }

    vTaskDelete(NULL);
}
