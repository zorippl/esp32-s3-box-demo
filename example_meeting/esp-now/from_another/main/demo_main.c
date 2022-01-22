/**
 * @file demo_main.c
 * @brief 
 * @version 0.1
 * @date 2021-06-23
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

// #include "bsp_board.h"
// #include "bsp_codec.h"
// #include "bsp_i2s.h"
// #include "bsp_lcd.h"
// #include "bsp_storage.h"
// #included "bsp_tp.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "lv_demo.h"
// #include "lv_port.h"
// #include "lvgl.h"

#include "nvs_flash.h"
#include "espnow_cube.h"

#include "driver/gpio.h"

#define TOGGLE (3)
#define RELEASE (2)
// GPIO
#define ESP_INTR_FLAG_DEFAULT   0
#define GPIO_INPUT_IO           19
#define GPIO_INPUT_PIN_SEL      (1ULL<<GPIO_INPUT_IO)


static xSemaphoreHandle gpio_evt = NULL;

// void send_cb(lv_event_t *event)
// {
//     lv_obj_t *button = lv_event_get_target(event);
//     if(event->code == LV_EVENT_VALUE_CHANGED){
//         if(button->state == TOGGLE) {
//             printf("turn on light\n");
//             turn_on_light();
//         } else {
//             printf("turn off light\n");
//             turn_off_light();
//         }
//     }
// }

// static void ui_main()
// {
//     lv_obj_t *button = lv_btn_create(lv_scr_act());
//     lv_obj_set_size(button, 80, 80);
//     lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
//     lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
//     lv_obj_add_event_cb(button, send_cb, LV_EVENT_ALL, NULL);
// }

static void IRAM_ATTR gpio_intr_handler(void* arg)
{
    // (void *) arg;
    xSemaphoreGiveFromISR(gpio_evt, NULL);
}


static void gpio_send_espnow_init(void)
{
    gpio_config_t io_config = {};
    io_config.intr_type = GPIO_INTR_NEGEDGE;
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_config.pull_up_en = 1;
    gpio_config(&io_config);

    gpio_evt = xSemaphoreCreateBinary();

    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_INPUT_IO, gpio_intr_handler, NULL);
}

static void light_task(void *arg)
{
    static int flag = 1;
    while(1){
        xSemaphoreTake(gpio_evt, portMAX_DELAY);
        if(flag) {
            printf("turn on light\n");
            turn_on_light(true);
            flag ^= 1;
        } else {
            printf("turn off light\n");
            turn_on_light(false);
            flag ^= 1;
        }
    }
}

void app_main(void)
{
    // ESP_ERROR_CHECK(bsp_board_init());
    // ESP_ERROR_CHECK(bsp_board_power_ctrl(POWER_MODULE_AUDIO, true));
    // ESP_ERROR_CHECK(bsp_storage_init(BSP_STORAGE_SPIFFS));
    // ESP_ERROR_CHECK(bsp_storage_init(BSP_STORAGE_SD_CARD));

    // ESP_ERROR_CHECK(bsp_lcd_init());
    // ESP_ERROR_CHECK(bsp_tp_init());
    // ESP_ERROR_CHECK(lv_port_init());
    // lv_task_handler();

    // ESP_ERROR_CHECK(bsp_i2s_init(I2S_NUM_0, 16000));
    // ESP_ERROR_CHECK(bsp_codec_init(AUDIO_HAL_16K_SAMPLES));
    // vTaskDelay(pdMS_TO_TICKS(500));
    // app_audio_load();
    // if(xTaskCreate(app_audio_task, "app_audio", 4096, NULL, 5, NULL) != pdTRUE) {
    //     ESP_LOGE("app_main", "create audio task failed");
    // }

    // rmt_ir_init();
    // ESP_ERROR_CHECK(ui_main_start());
    // ESP_ERROR_CHECK(app_sr_start(false));
    // audio_record_to_file(5000, "/sdcard/Record_TDM.pcm");

    // ESP_ERROR_CHECK(app_led_init(GPIO_RMT_LED));
    // ESP_ERROR_CHECK(app_network_start("esp-cube"));

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    example_wifi_init();
    example_espnow_init();
    // ui_main();
    gpio_send_espnow_init();
    
    xTaskCreate(light_task, "light_task", 2048, NULL, 10, NULL);

    // while (vTaskDelay(2), true) {
    //     lv_task_handler();
    // }
}