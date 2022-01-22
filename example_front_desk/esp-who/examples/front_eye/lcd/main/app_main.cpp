#include "who_camera.h"
#include "who_human_face_detection.hpp"
#include "who_lcd.h"

#include "app_wifi.h"

#include "esp_http_server.h"

#define MAX_DATA_SIZE   (16 * 1024)

extern bool system_state = false;
extern camera_fb_t *fb_screenshot = (camera_fb_t*)malloc(sizeof(camera_fb_t));
extern camera_fb_t *out_frame = (camera_fb_t*)malloc(sizeof(camera_fb_t));

static QueueHandle_t xQueueAIFrame = NULL;
static QueueHandle_t xQueueLCDFrame = NULL;

static const char *TAG = "main";

extern "C" esp_err_t server_start(void);
extern "C" esp_err_t image_request_handler(httpd_req_t *req);
extern "C" esp_err_t common_request_handler(httpd_req_t *req);

httpd_handle_t server = NULL;

extern "C" void app_main()
{
    xQueueAIFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    xQueueLCDFrame = xQueueCreate(2, sizeof(camera_fb_t *));
    app_wifi_main();
    register_camera(PIXFORMAT_RGB565, FRAMESIZE_240X240, 2, xQueueAIFrame);
    register_human_face_detection(xQueueAIFrame, NULL, NULL, xQueueLCDFrame, false);
    register_lcd(xQueueLCDFrame, NULL, true);
}
extern "C" esp_err_t server_start(void)
{
    esp_err_t ret = ESP_OK;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    ESP_LOGI(TAG, "Starting HTTP Server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) != ESP_OK) {

        ESP_LOGE(TAG, "Failed to start file server!");
        return ESP_FAIL;
    }
    httpd_uri_t image_req = {
        .uri       = "/image",
        .method    = HTTP_GET,
        .handler   = image_request_handler,
    .   user_ctx  = NULL,
    };
    ret |= httpd_register_uri_handler(server, &image_req);

    /* URI handler for getting uploaded files */
    httpd_uri_t common_req = {
        .uri       = "/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = common_request_handler,
        .user_ctx  = NULL,
    };
    ret |= httpd_register_uri_handler(server, &common_req);//
 

    printf("Ret :%d\n", ret);

    return ESP_OK;
}

extern "C" void stop_server(httpd_handle_t server)
{
    if (server) {
        if (httpd_stop(server) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to stop file server!");
        };
    }
}

static uint8_t image_buf[MAX_DATA_SIZE];

extern "C" esp_err_t image_request_handler(httpd_req_t *req)
{
    size_t image_size;
    esp_err_t ret_val = ESP_OK;
    fb_screenshot = out_frame;
    image_size = fb_screenshot->len;
    memcpy(image_buf, fb_screenshot->buf, image_size < MAX_DATA_SIZE ? image_size : MAX_DATA_SIZE);
    esp_camera_fb_return(fb_screenshot);

    /* Check for JPEG head and tail */
    if (image_buf[0] != 0xff || image_buf[1] != 0xd8 || image_buf[image_size - 2] != 0xff || image_buf[image_size - 1] != 0xd9) {
            ESP_LOGE(TAG, "Invalid JPEG data : %zu", image_size);
        image_buf[0] = 0xff;
        image_buf[1] = 0xd8;
        image_buf[image_size - 2] = 0xff;
        image_buf[image_size - 1] = 0xd9;
    } else {
        ESP_LOGI(TAG, "Sending image : %zu", image_size);
        //httpd_unregister_uri_handler(ptServer, "/image", HTTP_GET);//关闭服务
    }

    ret_val = httpd_resp_send(req, (const char *) image_buf, image_size);
    // ret_val = httpd_resp_send_chunk(req, );
    if (system_state) {
        stop_server(server);
    }
    if (ESP_OK != ret_val) {
        /* Abort sending file */
        httpd_resp_sendstr_chunk(req, NULL);

        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");

        return ret_val;
    }

    return ret_val;
}

esp_err_t common_request_handler(httpd_req_t *req)
{
    ESP_LOGW(TAG, "Unsupported request : %s", req->uri);

    return ESP_OK;
}

