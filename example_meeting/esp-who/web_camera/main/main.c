/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "esp_camera.h"
#include "app_camera.h"



static const char *TAG = "example:take_picture";

esp_err_t init_camera(uint32_t xclk_freq_hz, pixformat_t pixel_format, framesize_t framesize, uint8_t fb_count)
{
    camera_config_t camera_config = {
        .pin_pwdn = PWDN_GPIO_NUM,
        .pin_reset = RESET_GPIO_NUM,
        .pin_xclk = XCLK_GPIO_NUM,
        .pin_sscb_sda = SIOD_GPIO_NUM,
        .pin_sscb_scl = SIOC_GPIO_NUM,

        .pin_d0 = Y2_GPIO_NUM,
        .pin_d1 = Y3_GPIO_NUM,
        .pin_d2 = Y4_GPIO_NUM,
        .pin_d3 = Y5_GPIO_NUM,
        .pin_d4 = Y6_GPIO_NUM,
        .pin_d5 = Y7_GPIO_NUM,
        .pin_d6 = Y8_GPIO_NUM,
        .pin_d7 = Y9_GPIO_NUM,
        .pin_vsync = VSYNC_GPIO_NUM,
        .pin_href = HREF_GPIO_NUM,
        .pin_pclk = PCLK_GPIO_NUM,

        //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
        .xclk_freq_hz = xclk_freq_hz,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = pixel_format, //YUV422,GRAYSCALE,RGB565,JPEG
        .frame_size = framesize,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

        .jpeg_quality = 12, //0-63 lower number means higher quality
        .fb_count = fb_count,       //if more than one, i2s runs in continuous mode. Use only with JPEG
        .fb_location = CAMERA_FB_IN_PSRAM,
    };

    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    sensor_t * s = esp_camera_sensor_get();
    s->set_vflip(s, 1);//flip it back
    //initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID) {
        s->set_brightness(s, 1);//up the blightness just a bit
        s->set_saturation(s, -2);//lower the saturation
    }

    return ESP_OK;
}




float _camera_test_fps(uint16_t times)
{
    ESP_LOGW("TAG", "satrt to test fps");
    uint64_t total_time = esp_timer_get_time();
    for (size_t i = 0; i < times; i++) {
        uint64_t s = esp_timer_get_time();
        camera_fb_t *pic = esp_camera_fb_get();
        if (NULL == pic) {
            ESP_LOGW("TAG", "fb get failed");
            continue;
        }
        printf("fb_get: (%d x %d) %lluUS\n", pic->width, pic->height, esp_timer_get_time() - s); s = esp_timer_get_time();
        // char str[64]={0};
        // sprintf(str, "hd_%d.jpg", i);
        // image_save(pic->buf, pic->len, str);
        esp_camera_fb_return(pic);//printf("fb_ret: %lluUS\n", esp_timer_get_time()-s);s=esp_timer_get_time();
    }
    total_time = esp_timer_get_time() - total_time;
    float fps = times / (total_time / 1000000.0f);
    return fps;
}

void web_camera_start();

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    if (init_camera(20*1000000, PIXFORMAT_JPEG, FRAMESIZE_VGA, 2) != ESP_OK) {
        return;
    }

    printf("Free heap: %d\n", esp_get_free_heap_size());

    printf("fps=%f\n", _camera_test_fps(16));
    // sensor_t * s = esp_camera_sensor_get();
    // s->set_vflip(s, 0);

    web_camera_start();
    
}
