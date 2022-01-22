/**
 * @file ui_album.c
 * @brief 
 * @version 0.1
 * @date 2022-01-06
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
#include "lvgl.h"
#include "ui_main.h"
#include "app_album.h"

#define NUM_PICTURES 4

static lv_obj_t* img_album[NUM_PICTURES] = {&pic_01, &pic_02, &pic_03, &pic_04};
static lv_obj_t *img_album_def = NULL;

static void album_switch(lv_event_t *event)
{
    static uint16_t i = 0;
    int8_t status = (int8_t) event->user_data;
    
    if (status == TURN_NEXT && i < NUM_PICTURES - 1) {
        lv_img_set_src(img_album_def, img_album[++i]);
    }else if (status == TURN_PREVIOUS && i > 0) {
        lv_img_set_src(img_album_def, img_album[--i]);
    }
    
}

void ui_album(bool show)
{
    static lv_obj_t *widget = NULL;
    if (NULL == widget) {
        widget = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(widget, 20, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(widget, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(widget, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_size(widget, 180, 180);
        lv_obj_align(widget, LV_ALIGN_BOTTOM_MID, 0, -15);
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_shadow_width(widget, 20, LV_STATE_DEFAULT);
    }

    
    if (NULL == img_album_def) {
        img_album_def = lv_img_create(widget);
        lv_img_set_src(img_album_def, &pic_01);
        lv_obj_align(img_album_def, LV_ALIGN_CENTER, 0, 0);
    }

    static lv_obj_t *btn_pre = NULL;
    if (NULL == btn_pre) {
        int8_t t_pre = TURN_PREVIOUS;
        btn_pre = lv_label_create(lv_scr_act());
        lv_obj_add_flag(btn_pre, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(btn_pre, 15);
        lv_label_set_text_static(btn_pre, LV_SYMBOL_LEFT);
        lv_obj_set_style_text_font(btn_pre, &lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_pre, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_pre, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_align(btn_pre, LV_ALIGN_CENTER, -120, 15);
        lv_obj_add_event_cb(btn_pre, album_switch, LV_EVENT_CLICKED, (void*)t_pre);
    }

    static lv_obj_t *btn_next = NULL;
    if (NULL == btn_next) {
        int8_t t_next = TURN_NEXT;
        btn_next = lv_label_create(lv_scr_act());
        lv_obj_add_flag(btn_next, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(btn_next, 15);
        lv_label_set_text_static(btn_next, LV_SYMBOL_RIGHT);
        lv_obj_set_style_text_font(btn_next, &lv_font_montserrat_16, LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_next, lv_color_black(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(btn_next, lv_color_white(), LV_STATE_PRESSED);
        lv_obj_align(btn_next, LV_ALIGN_CENTER, 120, 15);
        lv_obj_add_event_cb(btn_next, album_switch, LV_EVENT_CLICKED, (void*)t_next);
    }

    if (show) {
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_pre, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_next, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_pre, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(btn_next, LV_OBJ_FLAG_HIDDEN);
    }
}

