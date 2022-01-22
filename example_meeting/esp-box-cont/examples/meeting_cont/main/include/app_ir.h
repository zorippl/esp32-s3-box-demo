/**
 * @file app_ir.h
 * @brief 
 * @version 0.1
 * @date 2021-09-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "esp_err.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RMT_BTN_ON = 0,
    RMT_BTN_UP,
    RMT_BTN_DOWN,
    RMT_BTN_LEFT,
    RMT_BTN_RIGHT,
    RMT_BTN_OK,
    RMT_BTN_VOL_UP,
    RMT_BTN_VOL_DOWN,
} rmt_btn_t;

/**
 * @brief 
 * 
 * @return esp_err_t 
 */
esp_err_t rmt_ir_init(void);

/**
 * @brief 
 * 
 * @param addr 
 * @param cmd 
 * @return esp_err_t 
 */
esp_err_t rmt_nec_send(uint16_t addr, uint16_t cmd);

/**
 * @brief 
 * 
 * @param button_id 
 * @return esp_err_t 
 */
esp_err_t rmt_send_cmd_with_id(rmt_btn_t button_id);

#ifdef __cplusplus
}
#endif
