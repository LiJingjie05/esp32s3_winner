#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_http_client.h"

#include "./ui_demo/ui.h"

// Wi-Fi 配置
#define WIFI_SSID "HUAWEI"
#define WIFI_PASS "1015jjl."

// Flask 服务地址
#define SERVER_URL "http://192.168.31.21:5000/system_info"

static const char *TAG = "HTTP_CLIENT";

// Wi-Fi 连接函数
esp_err_t wifi_init_sta(void) {
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_connect();
    
    ESP_LOGI(TAG, "Connecting to WiFi...");
    return ESP_OK;
}

// HTTP 请求回调函数
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        // case HTTP_EVENT_HEADER_RECEIVED:
        //     ESP_LOGI(TAG, "HTTP_EVENT_HEADER_RECEIVED, key=%s, value=%s", evt->header_key, evt->header_value);
        //     break;
        // case HTTP_EVENT_BODY:
        //     if (!evt->body) {
        //         ESP_LOGI(TAG, "HTTP_EVENT_BODY is empty");
        //     } else {
        //         ESP_LOGI(TAG, "HTTP_EVENT_BODY, len=%d", evt->body_len);
        //         printf("%.*s\n", evt->body_len, (char*)evt->body);
        //     }
        //     break;
        // case HTTP_EVENT_FINISHED:
        //     ESP_LOGI(TAG, "HTTP_EVENT_FINISHED, status_code=%d", evt->status_code);
        //     break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI(TAG, "HTTP_EVENT_REDIRECT");
            break;
        case HTTP_EVENT_ON_HEADER:  // 处理 HTTP header 事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:  // 处理 HTTP 数据事件
            if (!evt->data) {
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, no data received");
            } else {
                ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, data len=%d", evt->data_len);
                // 如果你想处理接收到的 body 数据，使用 evt->data
                printf("%.*s\n", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:  // 处理 HTTP 请求完成事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:  // 处理 HTTP 连接断开事件
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            ESP_LOGI(TAG, "Unhandled HTTP event: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}

void http_get_task(void *pvParameters)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_sta();  // 连接 Wi-Fi
    
    // 等待 Wi-Fi 连接
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    esp_http_client_config_t config = {
        .url = SERVER_URL,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    ESP_LOGI(TAG, "Sending GET request to %s", SERVER_URL);

    while(1)
    {
        esp_err_t err = esp_http_client_perform(client);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "HTTP GET request sent successfully");
        } else {
            ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}
