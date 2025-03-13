#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"

#include "lvgl.h"

extern void lvgl_init(void);
extern void esp_http_server_task(void *pvParameters);
static void set_up(void)
{
    lvgl_init();

    // Set up the system
    xTaskCreate(esp_http_server_task,       // 任务函数
                "http_server_task",      // 任务名称
                16 * 1024,          // 任务栈大小，单位字节
                NULL,           // 任务参数
                5,              // 任务优先级
                NULL);          // 任务句柄
}

void app_main(void)
{
    set_up();

    while (1)
    {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(10));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
    }
    
}
