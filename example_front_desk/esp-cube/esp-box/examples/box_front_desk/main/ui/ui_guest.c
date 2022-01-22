/**
 * @file ui_guest.c
 * @brief 
 * @version 0.1
 * @date 2021-01-04
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

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
//change: #include "app_pomodoro.h"
#include "lvgl.h"
#include "ui_main.h"

LV_FONT_DECLARE(font_en_24)
LV_FONT_DECLARE(font_en_16)

LV_IMG_DECLARE(guest)

extern lv_obj_t *img_guest = NULL;
extern uint16_t *image_rgb565;
extern lv_img_dsc_t img_cam_receive = {
    .header.always_zero = 0,
    .header.w = 240,
    .header.h = 240,
    .data_size = 240 * 240 * 2,
    .header.cf = LV_IMG_CF_TRUE_COLOR,          /*Set the color format*/
    .data = NULL,
};

static void btn_back_guest_cb(lv_event_t *event)
{
    ui_guest(false);
    ui_clock(true);
}

static void btn_register_cb(lv_event_t *event)
{
    ui_guest(false);
    ui_network(true);
}

static void btn_unlock_cb(lv_event_t *event)
{
    ;
}

void ui_guest(bool show)
{
    static lv_obj_t *widget = NULL;
    if (NULL == widget) {
        widget = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(widget, 20, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(widget, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(widget, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_size(widget, 290, 180);
        lv_obj_align(widget, LV_ALIGN_BOTTOM_MID, 0, -15);
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_shadow_width(widget, 20, LV_STATE_DEFAULT);
    }

    static lv_obj_t *pic_widget = NULL;
    if (NULL == pic_widget) {
        pic_widget = lv_obj_create(widget);
        //lv_obj_set_style_radius(pic_widget, 20, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(pic_widget, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(pic_widget, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_size(pic_widget, 110, 110);
        lv_obj_align(pic_widget, LV_ALIGN_BOTTOM_MID, 0, -35);
        lv_obj_clear_flag(pic_widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_shadow_width(pic_widget, 20, LV_STATE_DEFAULT);
    }

    static lv_obj_t *btn_back = NULL;
    if (NULL == btn_back) {
        btn_back = lv_label_create(widget);
        lv_obj_add_flag(btn_back, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(btn_back, 15);
        lv_label_set_text_static(btn_back, LV_SYMBOL_LEFT);
        lv_obj_set_style_text_font(btn_back, &lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_back, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_back, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_align(btn_back, LV_ALIGN_CENTER, -120, -48 - 20);
        lv_obj_add_event_cb(btn_back, btn_back_guest_cb, LV_EVENT_CLICKED, NULL);
    }

    static lv_obj_t *btn_unlock = NULL;
    if (NULL == btn_unlock) {
        btn_unlock = lv_btn_create(widget);
        lv_obj_set_size(btn_unlock, 100, 30);
        lv_obj_align(btn_unlock, LV_ALIGN_BOTTOM_MID, -70, 5);
        lv_obj_set_style_text_color(btn_unlock, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_unlock, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_add_event_cb(btn_unlock, btn_unlock_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t * label_unlock = lv_label_create(btn_unlock);
        lv_label_set_text(label_unlock, "Unlock door");
        lv_obj_center(label_unlock);
    }

    static lv_obj_t *btn_register = NULL;
    if (NULL == btn_register) {
        btn_register = lv_btn_create(widget);
        lv_obj_set_size(btn_register, 100, 30);
        lv_obj_align(btn_register, LV_ALIGN_BOTTOM_MID, 70, 5);
        lv_obj_set_style_text_color(btn_register, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_register, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_add_event_cb(btn_register, btn_register_cb, LV_EVENT_CLICKED, NULL);

        lv_obj_t * label_register = lv_label_create(btn_register);
        lv_label_set_text(label_register, "Register info");
        lv_obj_center(label_register);
    }

    if (NULL == img_guest) {
        img_guest = lv_img_create(pic_widget);
        lv_img_set_src(img_guest, &img_cam_receive);
        lv_obj_align(img_guest, LV_ALIGN_CENTER, 0, 0);
    }

    if (show) {
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);
    }
}