/**
 * @file esp_now.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-10-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "esp_log.h"
#include "esp_system.h"
#include "esp_event.h"


#pragma once

#ifdef __cplusplus
extern "C" {
#endif


esp_err_t example_espnow_init(void);

void example_wifi_init(void);

void turn_on_light(bool on);


#ifdef __cplusplus
}
#endif
