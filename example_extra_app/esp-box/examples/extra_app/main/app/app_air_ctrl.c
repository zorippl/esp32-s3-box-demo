/**
 * @file app_air_ctrl.c
 * @brief 
 * @version 0.1
 * @date 2021-01-21
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

#include <string.h>
#include <stdlib.h>
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "app_air_ctrl.h"
#include "ui_main.h"

static char *fake_num[2][5]= {{"17.1","17.2","17.1","17.0","17.1"},
                              {"48.3","48.1","48.2","48.2","48.3"}};

static void app_air_ctrl_blink(void)
{
    static int i = 0;
    if (i <= 4) {
        tem_blink_set_text(fake_num[0][i]);
        hum_blink_set_text(fake_num[1][i]);
        i +=1;
    }else {
        i = 0;
    }
}

void app_blink_start(void)
{
    static lv_timer_t *timer = NULL;
    /* 构造一个lv定时任务 */
    if (NULL == timer) {
        timer = lv_timer_create(app_air_ctrl_blink, 1000, NULL);
    }
}