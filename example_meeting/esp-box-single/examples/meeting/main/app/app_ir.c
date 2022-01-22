/**
 * @file app_ir.c
 * @brief 
 * @version 0.1
 * @date 2021-09-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "driver/gpio.h"
#include "driver/rmt.h"
#include "esp_err.h"
#include "esp_log.h"
#include "ir_tools.h"
#include "bsp_board.h"
#include "app_ir.h"

#define IR_CHANNEL RMT_CHANNEL_1

typedef struct {
    uint16_t addr;
    uint16_t cmd;
} rmt_ir_t;

static const rmt_ir_t rmt_btn_val[] = {
    { 0x5583, 0x6f90 }, /* ON */
    { 0x5583, 0x4fb0 }, /* UP */
    { 0x5583, 0x4db2 }, /* DOWN */
    { 0x5583, 0x4cb3 }, /* LEFT */
    { 0x5583, 0x4eb1 }, /* RIGHT */
    { 0x5583, 0x7a85 }, /* OK */
    { 0x5583, 0x6798 }, /* VOL UP */
    { 0x5583, 0x6699 }, /* VOL DOWN */
};

static ir_builder_t *ir_builder_handle = NULL;

esp_err_t rmt_ir_init(void)
{
    esp_err_t ret_val = ESP_OK;

    rmt_config_t rmt_tx_config = RMT_DEFAULT_CONFIG_TX(GPIO_RMT_IR, IR_CHANNEL);
    rmt_tx_config.tx_config.carrier_en = true;
    ret_val |= rmt_config(&rmt_tx_config);
    ret_val |= rmt_driver_install(IR_CHANNEL, 0, 0);

    ir_builder_config_t ir_builder_config = IR_BUILDER_DEFAULT_CONFIG((ir_dev_t) IR_CHANNEL);

    ir_builder_config.flags |= IR_TOOLS_FLAGS_PROTO_EXT;
    ir_builder_handle = ir_builder_rmt_new_nec(&ir_builder_config);

    return ret_val;
}

esp_err_t rmt_nec_send(uint16_t addr, uint16_t cmd)
{
    size_t length = 0;
    rmt_item32_t *items = NULL;
    esp_err_t ret_val = ESP_OK;

    ESP_LOGI("ir", "ready to write %x %x", addr, cmd);
    
    ret_val |= ir_builder_handle->build_frame(ir_builder_handle, addr, cmd);
    ret_val |= ir_builder_handle->get_result(ir_builder_handle, &items, &length);

    rmt_write_items(IR_CHANNEL, items, length, false);

    // for(int i = 0; i < 5; i++) {
    // }
    
    return ret_val;
}

esp_err_t rmt_send_cmd_with_id(rmt_btn_t button_id)
{
    ESP_LOGI("APP_IR", "SEND IR CMD : %d", button_id);
    return rmt_nec_send(rmt_btn_val[button_id].addr, rmt_btn_val[button_id].cmd);
}
