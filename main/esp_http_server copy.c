// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <esp_log.h>
// #include <nvs_flash.h>
// #include <sys/param.h>
// #include "esp_netif.h"
// #include "protocol_examples_common.h"
// #include <esp_http_server.h>
// #include "esp_event.h"
// #include "cJSON.h"
// #include "esp_sntp.h" // 添加此行以包含SNTP头文件
// #include "esp_netif_sntp.h"
// #include "esp_wifi.h"
// #include "esp_sntp.h"

// #include "./ui_demo/ui.h"

// #define TAG "example"
// #define BUFFER_SIZE 1024

// /* HTTP POST handler */
// static esp_err_t echo_post_handler(httpd_req_t *req) {
//     char buf[BUFFER_SIZE];
//     int total_len = req->content_len;
//     int received = 0;
//     if (total_len >= BUFFER_SIZE) {
//         ESP_LOGE(TAG, "Request content too large");
//         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too large");
//         return ESP_FAIL;
//     }

//     while (received < total_len) {
//         int ret = httpd_req_recv(req, buf + received, total_len - received);
//         if (ret <= 0) {
//             ESP_LOGE(TAG, "Error receiving data");
//             return ESP_FAIL;
//         }
//         received += ret;
//     }
//     buf[received] = '\0'; // Null-terminate the received string

//     ESP_LOGI(TAG, "Received JSON: %s", buf);

//     // Parse JSON
//     cJSON *root = cJSON_Parse(buf);
//     if (!root) {
//         ESP_LOGE(TAG, "Invalid JSON received");
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
//         return ESP_FAIL;
//     }

//     // Extract fields
//     double cpu_usage = cJSON_GetObjectItem(root, "cpu_usage")->valuedouble;
//     double gpu_usage = cJSON_GetObjectItem(root, "gpu_usage")->valuedouble;
//     double memory_usage = cJSON_GetObjectItem(root, "memory_usage")->valuedouble;
//     double temperature = cJSON_GetObjectItem(root, "temperature")->valuedouble;
//     // cJSON *memory_info = cJSON_GetObjectItem(root, "memory_usage");
//     // double memory_total = cJSON_GetObjectItem(memory_info, "total")->valuedouble;
//     // double memory_used = cJSON_GetObjectItem(memory_info, "used")->valuedouble;

//     // double memory_percent = (memory_used / memory_total) * 100;

//     // lv_arc_set_value(ui_Arc5, cpu_usage);
//     // lv_event_send(ui_Arc5, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc3, gpu_usage);
//     // lv_event_send(ui_Arc3, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc4, memory_usage);
//     // lv_event_send(ui_Arc4, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc2, temperature);
//     // lv_event_send(ui_Arc2, LV_EVENT_VALUE_CHANGED, NULL); 

//     // lv_label_set_text(ui_year, "2023-08-08");
//     // lv_event_send(ui_year, LV_EVENT_VALUE_CHANGED, NULL);

//     // lv_label_set_text(ui_time, "00:00:00");
//     // lv_event_send(ui_time, LV_EVENT_VALUE_CHANGED, NULL);


//     // Construct response JSON
//     cJSON *response = cJSON_CreateObject();
//     cJSON_AddNumberToObject(response, "cpu_usage", cpu_usage);
//     cJSON_AddNumberToObject(response, "memory_used_percent", memory_usage);

//     char *response_str = cJSON_Print(response);
//     httpd_resp_set_type(req, "application/json");
//     httpd_resp_send(req, response_str, strlen(response_str));

//     // Clean up
//     cJSON_Delete(root);
//     cJSON_Delete(response);
//     free(response_str);

//     return ESP_OK;
// }

// static const httpd_uri_t echo = {
//     .uri       = "/echo",
//     .method    = HTTP_POST,
//     .handler   = echo_post_handler,
//     .user_ctx  = NULL
// };

// static httpd_handle_t start_webserver(void) {
//     httpd_handle_t server = NULL;
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();

//     ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
//     if (httpd_start(&server, &config) == ESP_OK) {
//         ESP_LOGI(TAG, "Registering URI handlers");
//         httpd_register_uri_handler(server, &echo);
//         return server;
//     }

//     ESP_LOGI(TAG, "Error starting server!");
//     return NULL;
// }

// // 改进后的时区设置
// void set_timezone() {
//     static bool tz_set = false;
//     if (!tz_set) {
//         setenv("TZ", "CST-8", 1);
//         tzset();
//         tz_set = true;
//     }
// }

// // 优化后的时间更新
// void update_ui_time() {
//     time_t now;
//     struct tm timeinfo;
//     time(&now);
//     localtime_r(&now, &timeinfo); // 自动应用时区

//     char buf[64];
//     strftime(buf, sizeof(buf), "%Y-%m-%d", &timeinfo);
//     lv_label_set_text(ui_year, buf);
    
//     strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
//     lv_label_set_text(ui_time, buf);
// }

// void my_sntp_sync_time(void) {
//     // 使用新API初始化并配置SNTP客户端
//     esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");
//     // config.sync_mode = SNTP_SYNC_MODE_IMMED; // 设置同步模式为立即同步
//     esp_netif_sntp_init(&config);

//     // 等待直到时间同步完成
//     time_t now = 0;
//     int retry = 0;
//     const int retry_count = 20;
//     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
//         vTaskDelay(pdMS_TO_TICKS(2000));
//     }
//     time(&now);
//     ESP_LOGI(TAG, "Time is synchronized with NTP server.");
// }



// // 默认 AP 配置
// #define AP_SSID "ESP32-Config"
// #define AP_PASSWORD "12345678" // AP 密码（至少 8 位）
// #define AP_CHANNEL 1

// // 配置页面 HTML（简化示例）
// static const char *HTML_FORM =
//     "<form method='post' action='/save'>"
//     "SSID: <input type='text' name='ssid'><br>"
//     "密码: <input type='password' name='password'><br>"
//     "<input type='submit' value='连接'>"
//     "</form>";

// // 处理 GET 请求（返回配置页面）
// static esp_err_t uri_get_handler(httpd_req_t *req)
// {
//     httpd_resp_send(req, HTML_FORM, HTTPD_RESP_USE_STRLEN);
//     return ESP_OK;
// }

// // 处理 POST 请求（接收 SSID 和密码）
// static esp_err_t uri_post_handler(httpd_req_t *req)
// {
//     char buf[100];
//     int ret = httpd_req_recv(req, buf, sizeof(buf));
//     if (ret <= 0)
//         return ESP_FAIL;

//     // 解析表单数据
//     char ssid[32] = {0};
//     char password[64] = {0};
//     if (httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid)) != ESP_OK ||
//         httpd_query_key_value(buf, "password", password, sizeof(password)) != ESP_OK)
//     {
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid request");
//         return ESP_FAIL;
//     }

//     // 检查SSID和密码长度
//     if (strlen(ssid) == 0 || strlen(password) < 8)
//     {
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid SSID or password");
//         return ESP_FAIL;
//     }

//     // 保存到NVS
//     nvs_handle_t nvs;
//     if (nvs_open("wifi_cfg", NVS_READWRITE, &nvs) == ESP_OK)
//     {
//         nvs_set_str(nvs, "ssid", ssid);
//         nvs_set_str(nvs, "password", password);
//         nvs_close(nvs);
//     }
//     else
//     {
//         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open NVS");
//         return ESP_FAIL;
//     }

//     // 返回成功响应
//     httpd_resp_sendstr(req, "Configuration saved successfully");

//     // 重启设备尝试连接
//     esp_restart();
//     return ESP_OK;
// }

// static esp_err_t favicon_get_handler(httpd_req_t *req)
// {
//     // Return empty response for favicon.ico
//     httpd_resp_send(req, NULL, 0);
//     return ESP_OK;
// }

// // 注册 favicon.ico 处理器
// static const httpd_uri_t favicon_get = {
//     .uri = "/favicon.ico",
//     .method = HTTP_GET,
//     .handler = favicon_get_handler};

// // 注册 URI 处理函数
// static const httpd_uri_t uri_get = {
//     .uri = "/",
//     .method = HTTP_GET,
//     .handler = uri_get_handler};
// static const httpd_uri_t uri_post = {
//     .uri = "/save",
//     .method = HTTP_POST,
//     .handler = uri_post_handler};

// // 初始化 AP 模式
// void init_softap()
// {
//     esp_netif_create_default_wifi_ap();
//     wifi_config_t wifi_config = {
//         .ap = {
//             .ssid = AP_SSID,
//             .password = AP_PASSWORD,
//             .ssid_len = strlen(AP_SSID),
//             .channel = AP_CHANNEL,
//             .max_connection = 4,
//             .authmode = WIFI_AUTH_WPA2_PSK}};
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
//     ESP_ERROR_CHECK(esp_wifi_start());
//     printf("AP 已启动，SSID: %s\n", AP_SSID);
// }

// // 启动 Web 服务器
// static httpd_handle_t wifi_config_start_webserver()
// {
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();
//     httpd_handle_t server = NULL;
//     if (httpd_start(&server, &config) == ESP_OK)
//     {
//         // 注册网页处理函数
//         httpd_register_uri_handler(server, &uri_get);
//         httpd_register_uri_handler(server, &uri_post);
//         httpd_register_uri_handler(server, &favicon_get);
//     }
//     return server;
// }

// void wifi_event_handler(void *arg, esp_event_base_t event_base,
//                         int32_t event_id, void *event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
//     {
//         esp_wifi_connect();
//     }
//     else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
//     {
//         ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
//         ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
//         // WiFi连接成功，可以在这里执行其他操作
//     }
//     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
//     {
//         // WiFi连接失败，可以在这里重试连接或采取其他行动
//         ESP_LOGI(TAG, "Disconnected from WiFi");
//         esp_wifi_connect(); // 重新连接
//     }
//     else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
//     {
//         wifi_event_sta_disconnected_t *disconn = (wifi_event_sta_disconnected_t *) event_data;
//         ESP_LOGE(TAG, "Disconnected from WiFi, reason: %d", disconn->reason);
//         esp_wifi_connect();
//     }
// }

// void esp_http_server_task(void *pvParameters) {
//     nvs_flash_init();
//     // esp_netif_init();
//     // esp_event_loop_create_default();
//     // example_connect();

//     // 尝试读取已保存的 Wi-Fi 配置
//     char saved_ssid[32] = {0};
//     char saved_password[64] = {0};
//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     nvs_handle_t nvs;
//     if (nvs_open("wifi_cfg", NVS_READONLY, &nvs) == ESP_OK)
//     {
//         size_t len = sizeof(saved_ssid);
//         nvs_get_str(nvs, "ssid", saved_ssid, &len);
//         len = sizeof(saved_password);
//         nvs_get_str(nvs, "password", saved_password, &len);
//         nvs_close(nvs);
//     }

//     esp_netif_init();
//     esp_event_loop_create_default();
//     esp_wifi_init(&cfg);

//     // 如果已保存配置，直接连接
//     if (strlen(saved_ssid) > 0)
//     {
//         wifi_config_t wifi_config = {0};
//         strcpy((char *)wifi_config.sta.ssid, saved_ssid);
//         strcpy((char *)wifi_config.sta.password, saved_password);
//         esp_wifi_set_mode(WIFI_MODE_STA);
//         esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
//         // 注册WiFi事件处理程序
//         ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
//         ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));
//         esp_wifi_connect();
//         ESP_LOGI(TAG, "尝试连接已保存的 Wi-Fi: %s\n", saved_ssid);
//         ESP_LOGI(TAG, "尝试连接已保存的 密码: %s\n", saved_password);
//     }
//     else
//     {
//         // 未保存配置，启动 AP 和 Web 服务器
//         init_softap();
//         wifi_config_start_webserver();
//     }

//     // 初始化并同步时间
//     my_sntp_sync_time();
//     set_timezone();

//     httpd_handle_t server = start_webserver();
//     while (1) {
//         update_ui_time();
//         sleep(1);
//     }
// }











































// #include <string.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <esp_log.h>
// #include <nvs_flash.h>
// #include <sys/param.h>
// #include "esp_netif.h"
// #include "protocol_examples_common.h"
// #include <esp_http_server.h>
// #include "esp_event.h"
// #include "cJSON.h"
// #include "esp_sntp.h" // 添加此行以包含SNTP头文件
// #include "esp_netif_sntp.h"

// #include "./ui_demo/ui.h"

// #define TAG "example"
// #define BUFFER_SIZE 1024

// /* HTTP POST handler */
// static esp_err_t echo_post_handler(httpd_req_t *req) {
//     char buf[BUFFER_SIZE];
//     int total_len = req->content_len;
//     int received = 0;
//     if (total_len >= BUFFER_SIZE) {
//         ESP_LOGE(TAG, "Request content too large");
//         httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too large");
//         return ESP_FAIL;
//     }

//     while (received < total_len) {
//         int ret = httpd_req_recv(req, buf + received, total_len - received);
//         if (ret <= 0) {
//             ESP_LOGE(TAG, "Error receiving data");
//             return ESP_FAIL;
//         }
//         received += ret;
//     }
//     buf[received] = '\0'; // Null-terminate the received string

//     ESP_LOGI(TAG, "Received JSON: %s", buf);

//     // Parse JSON
//     cJSON *root = cJSON_Parse(buf);
//     if (!root) {
//         ESP_LOGE(TAG, "Invalid JSON received");
//         httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
//         return ESP_FAIL;
//     }

//     // Extract fields
//     double cpu_usage = cJSON_GetObjectItem(root, "cpu_usage")->valuedouble;
//     double gpu_usage = cJSON_GetObjectItem(root, "gpu_usage")->valuedouble;
//     double memory_usage = cJSON_GetObjectItem(root, "memory_usage")->valuedouble;
//     double temperature = cJSON_GetObjectItem(root, "temperature")->valuedouble;
//     // cJSON *memory_info = cJSON_GetObjectItem(root, "memory_usage");
//     // double memory_total = cJSON_GetObjectItem(memory_info, "total")->valuedouble;
//     // double memory_used = cJSON_GetObjectItem(memory_info, "used")->valuedouble;

//     // double memory_percent = (memory_used / memory_total) * 100;

//     // lv_arc_set_value(ui_Arc5, cpu_usage);
//     // lv_event_send(ui_Arc5, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc3, gpu_usage);
//     // lv_event_send(ui_Arc3, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc4, memory_usage);
//     // lv_event_send(ui_Arc4, LV_EVENT_VALUE_CHANGED, NULL); 
//     // lv_arc_set_value(ui_Arc2, temperature);
//     // lv_event_send(ui_Arc2, LV_EVENT_VALUE_CHANGED, NULL); 

//     // lv_label_set_text(ui_year, "2023-08-08");
//     // lv_event_send(ui_year, LV_EVENT_VALUE_CHANGED, NULL);

//     // lv_label_set_text(ui_time, "00:00:00");
//     // lv_event_send(ui_time, LV_EVENT_VALUE_CHANGED, NULL);


//     // Construct response JSON
//     cJSON *response = cJSON_CreateObject();
//     cJSON_AddNumberToObject(response, "cpu_usage", cpu_usage);
//     cJSON_AddNumberToObject(response, "memory_used_percent", memory_usage);

//     char *response_str = cJSON_Print(response);
//     httpd_resp_set_type(req, "application/json");
//     httpd_resp_send(req, response_str, strlen(response_str));

//     // Clean up
//     cJSON_Delete(root);
//     cJSON_Delete(response);
//     free(response_str);

//     return ESP_OK;
// }

// static const httpd_uri_t echo = {
//     .uri       = "/echo",
//     .method    = HTTP_POST,
//     .handler   = echo_post_handler,
//     .user_ctx  = NULL
// };

// static httpd_handle_t start_webserver(void) {
//     httpd_handle_t server = NULL;
//     httpd_config_t config = HTTPD_DEFAULT_CONFIG();

//     ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
//     if (httpd_start(&server, &config) == ESP_OK) {
//         ESP_LOGI(TAG, "Registering URI handlers");
//         httpd_register_uri_handler(server, &echo);
//         return server;
//     }

//     ESP_LOGI(TAG, "Error starting server!");
//     return NULL;
// }

// // 改进后的时区设置
// void set_timezone() {
//     static bool tz_set = false;
//     if (!tz_set) {
//         setenv("TZ", "CST-8", 1);
//         tzset();
//         tz_set = true;
//     }
// }

// // 优化后的时间更新
// void update_ui_time() {
//     time_t now;
//     struct tm timeinfo;
//     time(&now);
//     localtime_r(&now, &timeinfo); // 自动应用时区

//     char buf[64];
//     strftime(buf, sizeof(buf), "%Y-%m-%d", &timeinfo);
//     lv_label_set_text(ui_year, buf);
    
//     strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
//     lv_label_set_text(ui_time, buf);
// }

// void my_sntp_sync_time(void) {
//     // 使用新API初始化并配置SNTP客户端
//     esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("ntp.aliyun.com");
//     // config.sync_mode = SNTP_SYNC_MODE_IMMED; // 设置同步模式为立即同步
//     esp_netif_sntp_init(&config);

//     // 等待直到时间同步完成
//     time_t now = 0;
//     int retry = 0;
//     const int retry_count = 20;
//     while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
//         vTaskDelay(pdMS_TO_TICKS(2000));
//     }
//     time(&now);
//     ESP_LOGI(TAG, "Time is synchronized with NTP server.");
// }

// void esp_http_server_task(void *pvParameters) {
//     ESP_ERROR_CHECK(nvs_flash_init());
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     ESP_ERROR_CHECK(example_connect());

//     // 初始化并同步时间
//     my_sntp_sync_time();
//     set_timezone();

//     httpd_handle_t server = start_webserver();
//     while (1) {
//         update_ui_time();
//         sleep(1);
//     }
// }

















// #include <string.h>
// #include "esp_wifi.h"
// #include "esp_event.h"
// #include "nvs_flash.h"
// #include "esp_log.h"
// #include "esp_sntp.h"

// static const char *TAG = "wifi station";

// /* Wi-Fi event handler */
// static void event_handler(void* arg, esp_event_base_t event_base,
//                           int32_t event_id, void* event_data)
// {
//     if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
//         esp_wifi_connect();
//     } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
//         esp_wifi_connect();
//         ESP_LOGI(TAG, "Retry to connect to the AP");
//     } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
//         ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
//         ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
//     }
// }

// /* Initialize Wi-Fi as sta and set scan method */
// void wifi_init_sta(void)
// {
//     // 初始化非易失性存储(NVS)，因为Wi-Fi和事件库都会用到它。
//     ESP_ERROR_CHECK(nvs_flash_init());

//     // 创建默认的Wi-Fi事件循环。
//     ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // 创建默认的Wi-Fi station接口。
//     esp_netif_create_default_wifi_sta();

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // 注册Wi-Fi事件处理器。
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

//     // 配置Wi-Fi参数。
//     wifi_config_t wifi_config = {
//         .sta = {
//             .ssid = "HUAWEI",
//             .password = "1015jjL.",
//         },
//     };
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
//     ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

//     // 启动Wi-Fi。
//     ESP_ERROR_CHECK(esp_wifi_start());

//     ESP_LOGI(TAG, "wifi_init_sta finished.");
// }

// void esp_http_server_task(void *pvParameters) {
//     ESP_LOGI(TAG, "ESP_WIFI_STA");
//     wifi_init_sta();
//     while(1)
//     {
//         sleep(1);
//     }
// }

