#include "esp_err.h"
#pragma once

#define CONFIG_ESPNOW_CHANNEL (1)
#define CONFIG_ESPNOW_SEND_COUNT 100
#define CONFIG_ESPNOW_SEND_DELAY 1000
#define CONFIG_ESPNOW_SEND_LEN 15

typedef enum {
    PROJECTOR_LIGHT,
    DOWNLIGHT,
    MAIN_LIGHT,
} light_name_t;

typedef enum {
    COMMAND_NOBODY_MOVE,
    COMMAND_SOMEONE_MOVE,
    COMMAND_SOMEONE_ENTERS,
} command_t;


void app_myespnow_start(void);
void app_myespnow_init(void);
esp_err_t set_light_status(light_name_t light_name, bool status);