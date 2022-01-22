/**
 * @file ui_main.c
 * @brief 
 * @version 0.1
 * @date 2021-08-11
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
#include <math.h>
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "soc/soc_memory_layout.h"
#include "time.h"
#include "ui_main.h"
#include "app_audio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "app_ir.h"

#ifndef PI
#define PI  (3.14159f)
#endif

// LVGL image declare
LV_IMG_DECLARE(esp_logo)
LV_IMG_DECLARE(esp_text)

LV_FONT_DECLARE(font_cmd_ir)

static lv_obj_t *btn = NULL;
static lv_obj_t *btn_down = NULL;
static lv_obj_t *btn_up = NULL;
static lv_obj_t *btn_left = NULL;
static lv_obj_t *btn_right = NULL;
static lv_obj_t *btn_ok = NULL;
static lv_obj_t *btn_shutdown = NULL;
static lv_obj_t *btn_vol_up = NULL;
static lv_obj_t *btn_vol_down = NULL;
static lv_obj_t *btn_back = NULL;

static bool is_pressing = false;

LV_IMG_DECLARE(circle)
LV_IMG_DECLARE(circle_down)
LV_IMG_DECLARE(circle_left)
LV_IMG_DECLARE(circle_right)
LV_IMG_DECLARE(circle_up)

typedef esp_err_t (*btn_back_call_back_t)(bool);

/* Command word handler */
// extern void sr_task_handler(lv_timer_t *timer);
static uint32_t MAN_CLICK_EVENT = 0;

/* Private variable(s) */

static void click_event_cb(lv_event_t *event)
{
    // ESP_LOGI("ui_main", "CLICKED");
    app_audio_send_play_signal(ANNOUNCEMENT);

    if(LV_EVENT_CLICKED == event->code || event->code == MAN_CLICK_EVENT) {
        ESP_ERROR_CHECK(rmt_send_cmd_with_id((rmt_btn_t)event->user_data));
    }
}

static void btn_back_rmt_cb(lv_event_t *event)
{
    ui_ir_remote_show(false);

    ui_dev_ctrl(true);
}

static bool in_area(lv_area_t area, lv_point_t point)
{
    if(point.x < area.x1 || point.x > area.x2) return false;
    if(point.y < area.y1 || point.y > area.y2) return false;

    return true;
}

static void circle_click_cb(lv_event_t *e)
{
    if(e->code == LV_EVENT_RELEASED) {
        lv_obj_set_style_img_recolor_opa(btn_down, LV_OPA_0, LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(btn_up, LV_OPA_0, LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(btn_left, LV_OPA_0, LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor_opa(btn_right, LV_OPA_0, LV_STATE_DEFAULT);
        is_pressing = false;
        return ;
    }
    if(e->code == LV_EVENT_PRESSING && !is_pressing) {
        lv_indev_t *indev = lv_indev_get_act();
        lv_point_t click_point;
        lv_indev_get_point(indev, &click_point);

        lv_area_t area_down, area_up, area_left, area_right;
        lv_obj_get_coords(btn_down, &area_down);
        lv_obj_get_coords(btn_up, &area_up);
        lv_obj_get_coords(btn_left, &area_left);
        lv_obj_get_coords(btn_right, &area_right);

        lv_color_t pixel_color;
        if(in_area(area_down, click_point)) {
            pixel_color = lv_img_buf_get_px_color(&circle_down, click_point.x - area_down.x1, click_point.y - area_down.y1, pixel_color);
            // printf("%d : %d ... %d %d ... %d %d ... %x\n", click_point.x, click_point.y, area_down.x1, area_down.y1, click_point.x - area_down.x1, click_point.y - area_down.y1, pixel_color.full);
            if(pixel_color.full == 0xffff) {
                is_pressing = true;
                lv_obj_set_style_img_recolor_opa(btn_down, LV_OPA_20, LV_STATE_DEFAULT);
                lv_event_send(btn_down, MAN_CLICK_EVENT, (void *)RMT_BTN_DOWN);
                return;
            }
        }
        
        if(in_area(area_up, click_point)) {
            pixel_color = lv_img_buf_get_px_color(&circle_up, click_point.x - area_up.x1, click_point.y - area_up.y1, pixel_color);
            // printf("%d : %d ... %d %d ... %d %d ... %x\n", click_point.x, click_point.y, area_down.x1, area_down.y1, click_point.x - area_down.x1, click_point.y - area_down.y1, pixel_color.full);
            if(pixel_color.full == 0xffff) {
                is_pressing = true;
                lv_obj_set_style_img_recolor_opa(btn_up, LV_OPA_20, LV_STATE_DEFAULT);
                lv_event_send(btn_up, MAN_CLICK_EVENT, (void *)RMT_BTN_UP);
                return;
            }
        }
        
        if(in_area(area_left, click_point)) {
            pixel_color = lv_img_buf_get_px_color(&circle_left, click_point.x - area_left.x1, click_point.y - area_left.y1, pixel_color);
            // printf("%d : %d ... %d %d ... %d %d ... %x\n", click_point.x, click_point.y, area_down.x1, area_down.y1, click_point.x - area_down.x1, click_point.y - area_down.y1, pixel_color.full);
            if(pixel_color.full == 0xffff) {
                is_pressing = true;
                lv_obj_set_style_img_recolor_opa(btn_left, LV_OPA_20, LV_STATE_DEFAULT);
                lv_event_send(btn_left, MAN_CLICK_EVENT, (void *)RMT_BTN_LEFT);
                return;
            }
        }
        
        if(in_area(area_right, click_point)) {
            pixel_color = lv_img_buf_get_px_color(&circle_right, click_point.x - area_right.x1, click_point.y - area_right.y1, pixel_color);
            // printf("%d : %d ... %d %d ... %d %d ... %x\n", click_point.x, click_point.y, area_down.x1, area_down.y1, click_point.x - area_down.x1, click_point.y - area_down.y1, pixel_color.full);
            if(pixel_color.full == 0xffff) {
                is_pressing = true;
                lv_obj_set_style_img_recolor_opa(btn_right, LV_OPA_20, LV_STATE_DEFAULT);
                lv_event_send(btn_right, MAN_CLICK_EVENT, (void *)RMT_BTN_RIGHT);
                return;
            }
        }
    }
}

static void ui_remote_start(void)
{
    MAN_CLICK_EVENT = lv_event_register_id();

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(0xe6, 0xe6, 0xe6), LV_STATE_DEFAULT);

    btn = lv_img_create(lv_scr_act());
    lv_img_set_src(btn, &circle);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 10);
    lv_obj_align_to(btn, lv_scr_act(), LV_ALIGN_CENTER, 0, 10);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(btn, circle_click_cb, LV_EVENT_PRESSING, NULL);
    lv_obj_add_event_cb(btn, circle_click_cb, LV_EVENT_RELEASED, NULL);

    btn_down = lv_img_create(lv_scr_act());
    lv_img_set_src(btn_down, &circle_down);
    lv_obj_align_to(btn_down, btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(btn_down, click_event_cb, MAN_CLICK_EVENT, (void *)RMT_BTN_DOWN);

    lv_obj_t *down_label = lv_label_create(btn_down);
    lv_label_set_text(down_label, LV_SYMBOL_DOWN);
    lv_obj_set_style_text_font(down_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(down_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_align(down_label, LV_ALIGN_CENTER, 0, 15);

    btn_up = lv_img_create(lv_scr_act());
    lv_img_set_src(btn_up, &circle_up);
    lv_obj_align_to(btn_up, btn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_event_cb(btn_up, click_event_cb, MAN_CLICK_EVENT, (void *)RMT_BTN_UP);

    lv_obj_t *up_label = lv_label_create(btn_up);
    lv_label_set_text(up_label, LV_SYMBOL_UP);
    lv_obj_set_style_text_font(up_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(up_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_align(up_label, LV_ALIGN_CENTER, 0, -15);

    btn_left = lv_img_create(lv_scr_act());
    lv_img_set_src(btn_left, &circle_left);
    lv_obj_align_to(btn_left, btn, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_add_event_cb(btn_left, click_event_cb, MAN_CLICK_EVENT, (void *)RMT_BTN_LEFT);
    
    lv_obj_t *left_label = lv_label_create(btn_left);
    lv_label_set_text(left_label, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(left_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(left_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_align(left_label, LV_ALIGN_CENTER, -15, 0);

    btn_right = lv_img_create(lv_scr_act());
    lv_img_set_src(btn_right, &circle_right);
    lv_obj_align_to(btn_right, btn, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_add_event_cb(btn_right, click_event_cb, MAN_CLICK_EVENT, (void *)RMT_BTN_RIGHT);
    
    lv_obj_t *right_label = lv_label_create(btn_right);
    lv_label_set_text(right_label, LV_SYMBOL_RIGHT);
    lv_obj_set_style_text_font(right_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(right_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_align(right_label, LV_ALIGN_CENTER, 15, 0);

    btn_ok = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_ok, 55, 55);
    lv_obj_set_style_bg_color(btn_ok, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align_to(btn_ok, btn, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_width(btn_ok, 5, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(btn_ok, lv_color_darken(lv_color_white(), 5), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(btn_ok, 100, LV_STATE_DEFAULT);
    lv_obj_add_event_cb(btn_ok, click_event_cb, LV_EVENT_CLICKED, (void *)RMT_BTN_OK);

    lv_obj_t *ok_label = lv_label_create(btn_ok);
    lv_label_set_text(ok_label, LV_SYMBOL_OK);
    lv_obj_set_style_text_font(ok_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ok_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_center(ok_label);

    lv_obj_move_foreground(btn_ok);

    btn_shutdown = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_shutdown, 50, 50);
    lv_obj_set_style_bg_color(btn_shutdown, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align_to(btn_shutdown, btn, LV_ALIGN_CENTER, -120, 50);
    lv_obj_set_style_radius(btn_shutdown, 100, LV_STATE_DEFAULT);    
    lv_obj_add_event_cb(btn_shutdown, click_event_cb, LV_EVENT_CLICKED, (void *)RMT_BTN_ON);

    lv_obj_t *shutdown_label = lv_label_create(btn_shutdown);
    lv_label_set_text(shutdown_label, LV_SYMBOL_POWER);
    lv_obj_set_style_text_font(shutdown_label, &lv_font_montserrat_18, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(shutdown_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_center(shutdown_label);

    btn_vol_up = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_vol_up, 40, 40);
    lv_obj_set_style_bg_color(btn_vol_up, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align_to(btn_vol_up, btn, LV_ALIGN_CENTER, 130, -30); 
    lv_obj_add_event_cb(btn_vol_up, click_event_cb, LV_EVENT_CLICKED, (void *)RMT_BTN_VOL_UP);

    lv_obj_t *vol_up_label = lv_label_create(btn_vol_up);
    lv_label_set_text(vol_up_label, LV_SYMBOL_VOLUME_MAX);
    lv_obj_set_style_text_font(vol_up_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(vol_up_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_center(vol_up_label);

    btn_vol_down = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn_vol_down, 40, 40);
    lv_obj_set_style_bg_color(btn_vol_down, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_align_to(btn_vol_down, btn, LV_ALIGN_CENTER, 130, 30); 
    lv_obj_add_event_cb(btn_vol_down, click_event_cb, LV_EVENT_CLICKED, (void *)RMT_BTN_VOL_DOWN); 

    lv_obj_t *vol_down_label = lv_label_create(btn_vol_down);
    lv_label_set_text(vol_down_label, LV_SYMBOL_VOLUME_MID);
    lv_obj_set_style_text_font(vol_down_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(vol_down_label, lv_color_darken(lv_color_white(), LV_OPA_50), 0);
    lv_obj_center(vol_down_label);

    if (NULL == btn_back) {
        btn_back = lv_label_create(lv_scr_act());
        lv_obj_add_flag(btn_back, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(btn_back, 20);
        lv_label_set_text_static(btn_back, LV_SYMBOL_LEFT);
        lv_obj_set_style_text_font(btn_back, &lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_back, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_back, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_align(btn_back, LV_ALIGN_CENTER, -120, -48 - 20);
        lv_obj_add_event_cb(btn_back, btn_back_rmt_cb, LV_EVENT_CLICKED, NULL);
    }

}

void ui_ir_remote_show(bool show)
{
    if (NULL == btn) {
        ui_remote_start();
    }

    if (show) {
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_down, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_up, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_left, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_right, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_ok, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_shutdown, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_vol_up, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_vol_down, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_back, LV_OBJ_FLAG_HIDDEN);

    } else {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_down, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_up, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_left, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_right, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_ok, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_shutdown, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_vol_up, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_vol_down, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_back, LV_OBJ_FLAG_HIDDEN);
    }
}

// static const char *cmd_list[] = {
//     "打开投影仪",
//     "关闭投影仪",
//     "向上",
//     "向下",
//     "向左",
//     "向右",
//     "确定",
//     "增大音量",
//     "减小音量",
// };

// static esp_err_t ui_show_cmd_string(int8_t cmd_id)
// {
//     if (cmd_id < -1) {
//         return ESP_ERR_INVALID_ARG;
//     }

//     if(lv_obj_has_flag(label_cmd, LV_OBJ_FLAG_HIDDEN)) {
//         lv_obj_clear_flag(label_cmd, LV_OBJ_FLAG_HIDDEN);
//     }

//     lv_label_set_text_static(label_cmd, cmd_list[cmd_id]);

//     return ESP_OK;
// }

// static void ui_hide_cmd_string(void)
// {
//     lv_obj_add_flag(label_cmd, LV_OBJ_FLAG_HIDDEN);
// }
