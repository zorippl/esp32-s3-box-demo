/**
 * @file ui_air_ctrl.c
 * @brief 
 * @version 0.1
 * @date 2022-01-04
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
//#include "app_pomodoro.h"
#include "lvgl.h"
#include "ui_main.h"

LV_FONT_DECLARE(font_en_bold_36)
LV_FONT_DECLARE(font_en_12)
LV_FONT_DECLARE(font_en_14)

LV_IMG_DECLARE(air_warn)

static lv_obj_t *label_tem = NULL;
static lv_obj_t *label_hum = NULL;

static void btn_back_air_ctrl_cb(lv_event_t *event)
{
    ui_air_ctrl(false);
    ui_dev_ctrl(true);
}

void ui_air_ctrl(bool show)
{
    static lv_obj_t *widget = NULL;
    if (NULL == widget) {
        widget = lv_obj_create(lv_scr_act());
        lv_obj_set_style_radius(widget, 20, LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(widget, 0, LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(widget, lv_color_turquoise(), LV_STATE_DEFAULT);
        lv_obj_set_size(widget, 290, 180);
        lv_obj_align(widget, LV_ALIGN_BOTTOM_MID, 0, -15);
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_shadow_width(widget, 20, LV_STATE_DEFAULT);
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
        lv_obj_add_event_cb(btn_back, btn_back_air_ctrl_cb, LV_EVENT_CLICKED, NULL);
    }

    static lv_point_t line_points[] = { {0, -10}, {0, 50} };
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_white());
    lv_style_set_line_rounded(&style_line, true);

    static lv_obj_t * split_line = NULL;
    if (NULL == split_line) {
        split_line = lv_line_create(widget);
        lv_line_set_points(split_line, line_points, 2);
        lv_obj_add_style(split_line, &style_line, 0);
        lv_obj_center(split_line);
    }

    static lv_obj_t *text_tem = NULL;
    if (NULL == text_tem) {
        text_tem = lv_label_create(widget);
        lv_label_set_text(text_tem, "Temperature:");
        lv_obj_set_style_text_color(text_tem, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text_tem, &font_en_12, LV_STATE_DEFAULT);
        lv_obj_align(text_tem, LV_ALIGN_CENTER, -85, -30);
    }

    static lv_obj_t *text_hum = NULL;
    if (NULL == text_hum) {
        text_hum = lv_label_create(widget);
        lv_label_set_text(text_hum, "Humidty:");
        lv_obj_set_style_text_color(text_hum, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(text_hum, &font_en_12, LV_STATE_DEFAULT);
        lv_obj_align(text_hum, LV_ALIGN_CENTER, 40, -30);
    }

    if (NULL == label_tem) {
        label_tem = lv_label_create(widget);
        lv_label_set_text(label_tem, "18.7");
        lv_obj_set_style_text_color(label_tem, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label_tem, &font_en_bold_36, LV_STATE_DEFAULT);
        lv_obj_align(label_tem, LV_ALIGN_CENTER, -60, 0);
    }

    if (NULL == label_hum) {
        label_hum = lv_label_create(widget);
        lv_label_set_text(label_hum, "48.3");
        lv_obj_set_style_text_color(label_hum, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label_hum, &font_en_bold_36, LV_STATE_DEFAULT);
        lv_obj_align(label_hum, LV_ALIGN_CENTER, 80, 0);
    }

    static lv_obj_t *img_warn = NULL;
    if (NULL == img_warn) {
        img_warn = lv_img_create(widget);
        lv_img_set_src(img_warn, &air_warn);
        lv_obj_align(img_warn, LV_ALIGN_CENTER, -100, 50);
    }

    static lv_obj_t *label_info = NULL;
    if (NULL == label_info) {
        label_info = lv_label_create(widget);
        lv_label_set_text(label_info, "Tips: Temperature is too low!");
        lv_obj_set_style_text_color(label_info, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label_info, &font_en_14, LV_STATE_DEFAULT);
        lv_obj_align(label_info, LV_ALIGN_CENTER, 6, 52);
    }

    if (show) {
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);
    }
}

void tem_blink_set_text(char *text)
{
    if (NULL != label_tem) {
        lv_label_set_text_static(label_tem, text);
    }
}

void hum_blink_set_text(char *text)
{
    if (NULL != label_hum) {
        lv_label_set_text_static(label_hum, text);
    }
}