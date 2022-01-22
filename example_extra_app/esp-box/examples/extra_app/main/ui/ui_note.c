/**
 * @file ui_note.c
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

LV_FONT_DECLARE(font_en_16)

static void ta_event_cb(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t * ta = lv_event_get_target(event);
    lv_obj_t * kb = lv_event_get_user_data(event);
    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void ui_note(bool show)
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
    
    static lv_obj_t *kb = NULL;
    static lv_obj_t *ta = NULL;
    if (NULL == kb && NULL == ta) {
        ta = lv_textarea_create(widget);
        kb = lv_keyboard_create(widget);
        lv_obj_align(ta, LV_ALIGN_CENTER, 0, 0);
        lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
        lv_textarea_set_placeholder_text(ta, "Leave a message here");
        lv_obj_set_style_text_font(ta, &font_en_16, LV_STATE_DEFAULT);
        lv_obj_set_size(ta, 270, 160);

        lv_keyboard_set_textarea(kb, ta);
    }

    

    if (show) {
        lv_obj_clear_flag(widget, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(widget, LV_OBJ_FLAG_HIDDEN);
    }
}