/**
 * @file app_album.h
 * @brief 
 * @version 0.1
 * @date 2022-01-06
 * 
 * @copyright Copyright 2022 Espressif Systems (Shanghai) Co. Ltd.
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

#include "lvgl.h"

LV_IMG_DECLARE(pic_01)
LV_IMG_DECLARE(pic_02)
LV_IMG_DECLARE(pic_03)
LV_IMG_DECLARE(pic_04)

typedef enum {
    TURN_PREVIOUS = 0,
    TURN_NEXT
}APP_DIRECTION_t;


