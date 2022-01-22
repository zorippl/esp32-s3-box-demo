/**
 * @file app_sr.h
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

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SR_EVENT_WAKE_UP = 1 << 0,
    SR_EVENT_WORD_DETECT = 1 << 1,
    SR_EVENT_TIMEOUT = 1 << 2,
    SR_EVENT_ALL =
        SR_EVENT_WAKE_UP | SR_EVENT_WORD_DETECT | SR_EVENT_TIMEOUT,
} sr_event_t;

/**
 * @brief User defined command list
 * 
 */
typedef enum {
    // SR_CMD_LIGHT_ON = 0,
    // SR_CMD_LIGHT_OFF,
    SR_CMD_UP = 0,
    SR_CMD_DOWN,
    SR_CMD_LEFT,
    SR_CMD_RIGHT,
    SR_CMD_OK,
    SR_CMD_PROJECTOR_ON,
    //SR_CMD_PROJECTOR_OFF,
    SR_CMD_VOL_UP,
    SR_CMD_VOL_DOWN,
    SR_CMD_LIGHT_ON,
    SR_CMD_LIGHT_OFF,
    SR_CMD_BOTH_ON,
    SR_CMD_PROJECTOR_OFF,

} sr_cmd_t;

#define STR_TIMEOUT             "超时"
#define STR_WAKEWORD            "Hi 乐鑫"
#define STR_RMT_UP              "向上"
#define STR_RMT_DOWN            "向下"
#define STR_RMT_LEFT            "向左"
#define STR_RMT_RIGHT           "向右"
#define STR_RMT_OK              "确定"
#define STR_RMT_PROJECTOR_ON    "打开投影仪"
#define STR_RMT_VOL_UP          "增大音量"
#define STR_RMT_VOL_DOWN        "减小音量"
#define STR_LIGHT_ON            "打开电灯"
#define STR_LIGHT_OFF           "关闭电灯"
#define STR_RMT_BOTH_ON         "打开空气净化器和投影仪"
#define STR_RMT_PROJECTOR_OFF   "关闭投影仪"

/**
 * @brief Start speech recognition task
 * 
 * @param record_en Record audio to SD crad if set to `true`
 * @return 
 *    - ESP_OK: Success
 *    - ESP_ERR_NO_MEM: No enough memory for speech recognition
 *    - Others: Fail
 */
esp_err_t app_sr_start(bool record_en);

/**
 * @brief Get previous recognized command ID
 * 
 * @return int32_t Command index from 0
 */
int32_t app_sr_get_last_cmd_id(void);

/**
 * @brief Reset command list
 * 
 * @param command_list New command string
 * @return 
 *    - ESP_OK: Success
 *    - ESP_ERR_NO_MEM: No enough memory for err_id string
 *    - Others: Fail
 */
esp_err_t app_sr_reset_command_list(char *command_list);

#ifdef __cplusplus
}
#endif
