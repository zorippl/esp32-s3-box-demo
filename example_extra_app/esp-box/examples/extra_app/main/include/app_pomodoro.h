/**
 * @file app_pomodoro.h
 * @brief 
 * @version 0.1
 * @date 2021-12-13
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

#define ENUM_TIME_SELLECT_ROLLER    ("1\n" "5\n" "10\n" "15\n" "20\n" "25\n" "30\n" "35\n" "40\n" "45\n" "50\n" "55\n" "60\n")
#define VALUE_ANIME_RESUME          ((pomodoro_time_past * 100) / (pomodoro_time_set * 60))
#define VALUE_ANIME_REMAIN          (pomodoro_time_set * 60 - pomodoro_time_past)
/**
 * @brief State of the pomodoro
 * 
 */
typedef enum {
    POMODORO_FREE = 0,
    POMODORO_START,
    POMODORO_PAUSE,
} app_pomodoro_state_t;

/**
 * @brief Start pomodoro timer
 * 
 */
void app_pomodoro_start();

/**
 * @brief Pause pomodoro timer
 * 
 */
void app_pomodoro_reverse();

/**
 * @brief Reset the pomodoro
 * 
 */
void app_pomodoro_reset();