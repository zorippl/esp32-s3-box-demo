/**
 * @file app_audio.c
 * @brief 
 * @version 0.1
 * @date 2021-09-14
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "app_audio.h"
#include "es8311.h"


static void *audio_buffer[music_num];
static size_t file_size[music_num], bytes_written = 0;
static const char *TAG = "app_audio";
static QueueHandle_t Audio_Play_Handle = NULL;
static bool b_audio_playing = false;

static int audio_announcement = 0;
static int audio_coming_in = 1;
static int audio_warning = 2;
static int audio_wake = 3;

esp_err_t app_audio_load(s_audio_t audio)
{
    /* Read PCM data from SPIFFS */
    FILE *fp;

    switch(audio) {
    case ANNOUNCEMENT: {
        fp = fopen("/spiffs/audio/test_button_single.wav", "rb");
        break;
    }
    case COMINGIN: {
        fp = fopen("/spiffs/audio/ComingIn.wav", "rb");
        break;
    }
    case WARNING: {
        fp = fopen("/spiffs/audio/fine.wav", "rb");
        break;
    }
    case WAKE: {
        fp = fopen("/spiffs/audio/wake.wav", "rb");
        break;
    }
    default:
        fp = fopen("/spiffs/audio/test_button_single.wav", "rb");
        break;
    }

    if (NULL == fp) {
        ESP_LOGE(TAG, "File does not exist");
        return ESP_ERR_NOT_FOUND;
    }

    /* Get file size and reset curse */
    fseek(fp, 0, SEEK_END);
    file_size[audio] = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // printf("%d %d\n", file_size[audio]);

    /* Allocate audio buffer */
    // printf("%zu\n", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    audio_buffer[audio] = heap_caps_malloc(file_size[audio], MALLOC_CAP_SPIRAM);
    if (NULL == audio_buffer[audio]) {
        fclose(fp);
        ESP_LOGE(TAG, "No mem for audio buffer");
        return ESP_ERR_NO_MEM;
    }

    // ESP_LOGE(TAG, "malloc music %d size %d", audio, file_size[audio]);

    fread(audio_buffer[audio], 1, file_size[audio], fp);
    fclose(fp);

    return ESP_OK;
}


static BaseType_t Audio_Wait_Signal(int *audio, TickType_t TicksToWait)
{
    return xQueueReceive(Audio_Play_Handle, audio, TicksToWait);
}
void app_audio_task(void *arg)
{
    Audio_Play_Handle = xQueueCreate(1, sizeof(int));
    int audio_num = 0;
    // int volu = 0;
    while(1) {
        Audio_Wait_Signal(&audio_num, portMAX_DELAY);
        // app_audio_load(audio_num);
        // ESP_LOGI(TAG, "Playing audio %d", audio_num);
        switch(audio_num) {
        case ANNOUNCEMENT: {
            es8311_codec_set_voice_volume(60); //60
            break;
        }
        case COMINGIN: {
            es8311_codec_set_voice_volume(60); // 80
            break;
        }
        case WARNING: {
            es8311_codec_set_voice_volume(60); // 80
            break;
        }
        case WAKE: {
            es8311_codec_set_voice_volume(60); // 70
            break;
        }
        default:
            break;
        }
        // es8311_codec_get_voice_volume(&volu);
        // ESP_LOGE(TAG, "volume now : %d", volu);
        i2s_zero_dma_buffer(I2S_NUM_0);
        i2s_write(I2S_NUM_0, audio_buffer[audio_num], file_size[audio_num], &bytes_written, portMAX_DELAY);
        // ESP_LOGE(TAG, "HEAP LARGEST FREE SIZE : %d BLOCK %d", heap_caps_get_free_size(MALLOC_CAP_INTERNAL), heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));
        Audio_Wait_Signal(&audio_num, 0);
    }
}

BaseType_t app_audio_send_play_signal(s_audio_t audio)
{
    switch(audio) {
    case ANNOUNCEMENT:
        return xQueueSend(Audio_Play_Handle, &audio_announcement, 0); 
    case COMINGIN:
        return xQueueSend(Audio_Play_Handle, &audio_coming_in, 0);
    case WARNING:
        return xQueueSend(Audio_Play_Handle, &audio_warning, 0);
    case WAKE:
        return xQueueSend(Audio_Play_Handle, &audio_wake, 0);
    default:
        break;
    }
    return pdFAIL;
}

bool audio_is_playing(void)
{
    return b_audio_playing;
}