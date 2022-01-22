#include "who_human_face_detection.hpp"

#include "esp_log.h"
#include "esp_camera.h"
#include "esp_err.h"

#include "dl_image.hpp"
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"

#include "who_ai_utils.hpp"
#include "app_main.hpp"

extern bool system_state;
extern camera_fb_t *fb_screenshot;
extern camera_fb_t *out_frame;

#define TWO_STAGE_ON 1

static const char *TAG = "human_face_detection";

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueEvent = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResult = NULL;


static bool gEvent = true;
static bool gReturnFB = true;


static void task_process_handler(void *arg)
{
    static uint16_t detected_counts = 0;//zori
    camera_fb_t *frame = NULL;
    HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
#if TWO_STAGE_ON
    HumanFaceDetectMNP01 detector2(0.4F, 0.3F, 10);
#endif

    while (true)
    {
        if (system_state) {
            vTaskSuspend(NULL);//不要suspend,用其他方式如信号量，互斥量，setbit,receive等
        }
        if (gEvent)
        {
            bool is_detected = false;
            if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
            {
#if TWO_STAGE_ON
                std::list<dl::detect::result_t> &detect_candidates = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
                std::list<dl::detect::result_t> &detect_results = detector2.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3}, detect_candidates);
#else
                std::list<dl::detect::result_t> &detect_results = detector.infer((uint16_t *)frame->buf, {(int)frame->height, (int)frame->width, 3});
#endif

                if (detect_results.size() > 0)
                {
                    if(detected_counts++ <= 30) {
                        draw_detection_result((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                    }else {
                        draw_detection_result_my((uint16_t *)frame->buf, frame->height, frame->width, detect_results);
                        system_state = true;
                        frame2jpg(frame, 40, &(out_frame->buf) , &(out_frame->len));
                        out_frame->format = PIXFORMAT_JPEG;
                        out_frame->timestamp = frame->timestamp;
                        //fb_screenshot = frame;//进行转码
                        //jpg2rgb565(frame->buf, frame->len, fb_screenshot->buf, JPG_SCALE_2X);
                        server_start();
                        /* screenshots */
                    }
                    print_detection_result(detect_results);
                    is_detected = true;
                }
            }

            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            if (xQueueResult)
            {
                xQueueSend(xQueueResult, &is_detected, portMAX_DELAY);
            }
        }
    }
}

static void task_event_handler(void *arg)
{
    while (true)
    {
        /*if(system_state) {
            vTaskSuspend(NULL);
        }*/
        xQueueReceive(xQueueEvent, &(gEvent), portMAX_DELAY);
    }
}

void register_human_face_detection(QueueHandle_t frame_i,
                                   QueueHandle_t event,
                                   QueueHandle_t result,
                                   QueueHandle_t frame_o,
                                   const bool camera_fb_return)
{
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    xQueueEvent = event;
    xQueueResult = result;
    gReturnFB = camera_fb_return;


    xTaskCreatePinnedToCore(task_process_handler, TAG, 4 * 1024, NULL, 5, NULL, 0);
    if (xQueueEvent)
        xTaskCreatePinnedToCore(task_event_handler, TAG, 4 * 1024, NULL, 5, NULL, 1);
}
