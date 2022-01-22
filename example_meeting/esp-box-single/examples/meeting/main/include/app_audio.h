/**
 * @file app_audio.h
 * @brief 
 * @version 0.1
 * @date 2021-08-05
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

#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// typedef enum {
//     ANNOUNCEMENT = 0,
//     COMINGIN,
// } app_audio_t;

#define music_num 4 //attention

typedef enum {
    ANNOUNCEMENT = 0,
    COMINGIN,
    WARNING,
    WAKE,
} s_audio_t;

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t app_audio_load(s_audio_t audio);

/**
 * @brief 
 * 
 * @param arg 
 */
void app_audio_task(void *arg);

/**
 * @brief 
 * 
 * @param TicksToWait 
 * @return BaseType_t 
 */
// BaseType_t Audio_Wait_Signal(int *audio, TickType_t TicksToWait);

/**
 * @brief 
 * 
 * @return BaseType_t 
 */
BaseType_t app_audio_send_play_signal(s_audio_t audio);

bool audio_is_playing(void);

/**
 * @brief 
 * 
 */
void audio_play_start(void);



#ifdef __cplusplus
}
#endif
