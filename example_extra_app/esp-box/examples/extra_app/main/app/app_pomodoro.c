/**
 * @file app_pomodoro.c
 * @brief 
 * @version 0.1
 * @date 2021-12-14
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
#include "app_pomodoro.h"
#include "app_audio.h"

extern lv_anim_t lv_anim_transfer;
extern lv_obj_t *lv_label_transfer;
extern lv_obj_t *lv_roll_transfer;
extern lv_obj_t *lv_btn_start_transfer;
extern lv_obj_t *label_countdown;
extern uint8_t pomodoro_time_set;       /* minutes */
extern uint8_t pomodoro_time_backup;    /* minutes */ 
extern uint16_t pomodoro_time_past;     /* seconds */
extern uint8_t is_pressed;

static uint8_t min;
static uint8_t sec = 59;
static lv_timer_t *timer = NULL;

static void app_pomodoro_blink()
{
    static char buff[8];
    sprintf(buff, "%02d:%02d", min, sec);
    lv_label_set_text_static(lv_label_transfer, buff);
    if (sec == 0 && min != 0) {
        sec = 59;
        min = min - 1;
    }else if (sec == 0 && min == 0) {
        sprintf(buff, "%02d:%02d", min, sec);
        lv_label_set_text_static(lv_label_transfer, buff);
        audio_play_start();
        app_pomodoro_reset();
    }
    sec -= 1;
    pomodoro_time_past += 1;
}

void app_pomodoro_reset()
{
    if (timer != NULL) {
        lv_timer_del(timer);
        timer = NULL;
    }
    min = pomodoro_time_set - 1;
    sec = 59;
    pomodoro_time_past = 1;
    is_pressed = POMODORO_FREE;
    lv_anim_set_values(&lv_anim_transfer, 0, 100); /* 动画开始结束位置 */
    lv_anim_set_time(&lv_anim_transfer, 1000 * 60 * pomodoro_time_backup);
    lv_label_set_text_static(lv_btn_start_transfer, LV_SYMBOL_PLAY);
    lv_obj_add_flag(label_countdown, LV_OBJ_FLAG_CLICKABLE);
    lv_anim_refr_now();
}

void app_pomodoro_start()
{
    min = pomodoro_time_set - 1;
    /* 构造一个lv定时任务 */
    if (NULL == timer) {
        timer = lv_timer_create(app_pomodoro_blink, 1000, NULL);
    }
}

void app_pomodoro_reverse()
{
    if (is_pressed == POMODORO_START) {
        lv_obj_add_flag(label_countdown, LV_OBJ_FLAG_CLICKABLE);
        lv_timer_pause(timer);
        
    }else if (is_pressed == POMODORO_PAUSE) {
        lv_obj_clear_flag(label_countdown, LV_OBJ_FLAG_CLICKABLE);
        lv_timer_resume(timer);
    }
}