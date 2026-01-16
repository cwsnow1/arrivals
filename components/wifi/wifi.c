#include <string.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi.h"
#include "http_server.h"

#define WIFI_AP_SSID    "Train_Tracker_PCB"
#define WIFI_AP_PASSWD  "abc12345"
#define WIFI_AP_CHANNEL 1
#define MAX_STA_CONN    2

static const char* TAG = "wifi";

static SemaphoreHandle_t s_connected;
static esp_netif_t* s_netif;
static esp_netif_t* s_apif;

static char factory_mac[13];

const char hex[] = "0123456789abcdef";

static void event_handler(
    void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
        case WIFI_EVENT_STA_START: {
            ESP_LOGD(TAG, "Station start");
        } break;
        case WIFI_EVENT_STA_DISCONNECTED: {
            xSemaphoreTake(s_connected, pdMS_TO_TICKS(10));
            esp_wifi_connect();
            ESP_LOGI(TAG, "retry to connect to the AP");
        } break;
        case WIFI_EVENT_SCAN_DONE: {
            ESP_LOGI(TAG, "Scan complete");
        } break;
        default: break;
        }
    } else if (event_base == IP_EVENT) {
        switch (event_id) {
        case IP_EVENT_STA_GOT_IP: {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
            ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            xSemaphoreGive(s_connected);
        } break;
        default:
            break;
        }
    }
}

void wifi_init(void)
{
    static bool wifi_initialized = false;
    if (wifi_initialized) {
        return;
    }

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    s_netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_EFUSE_FACTORY);
    uint64_t mac_int = 0;
    for (int i = 0; i < 6; ++i) {
        mac_int |= ((uint64_t) mac[i]) << (8 * i);
    }
    for (int i = 0; i < 12; ++i, mac_int >>= 4) {
        factory_mac[i] = hex[mac_int & 0xF];
    }
    factory_mac[12] = '\0';
    wifi_initialized = true;
    s_connected = xSemaphoreCreateBinary();
}

void wifi_connect(const char* ssid, const char* password)
{
    wifi_config_t wifi_cfg = { 0 };
    strncpy((char*) wifi_cfg.sta.ssid, ssid, sizeof(wifi_cfg.sta.ssid) - 1);
    strncpy((char*) wifi_cfg.sta.password, password, sizeof(wifi_cfg.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    ESP_ERROR_CHECK(esp_wifi_connect());
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    if (s_apif == NULL) {
        s_apif = esp_netif_create_default_wifi_ap();
    }

    wifi_config_t wifi_sta_config = {
        .sta = {
            .ssid = {0},
            .password = {0},
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .failure_retry_cnt = 0,
            .threshold.authmode = WIFI_AUTH_OPEN,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    esp_wifi_set_config(WIFI_IF_STA, &wifi_sta_config);

    wifi_config_t wifi_ap_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .channel = WIFI_AP_CHANNEL,
            .password = WIFI_AP_PASSWD,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .required = false,
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_ap_config));

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             WIFI_AP_SSID, WIFI_AP_PASSWD, WIFI_AP_CHANNEL);

    ESP_ERROR_CHECK(esp_wifi_start());
}

const char* wifi_get_mac(void)
{
    return factory_mac;
}

bool wifi_is_connected(void)
{
    if (s_connected && xSemaphoreTake(s_connected, 0)) {
        xSemaphoreGive(s_connected);
        return true;
    }
    return false;
}

bool wifi_test_connection(const char* ssid, const char* password)
{
    wifi_config_t cfg = { 0 };
    memcpy(cfg.sta.ssid, ssid, sizeof cfg.sta.ssid);
    if (password)
        memcpy((char*) cfg.sta.password, password, sizeof cfg.sta.password);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_set_config(WIFI_IF_STA, &cfg);
    ESP_LOGI(TAG, "Trying to connect to %s", ssid);
    int64_t start_time = esp_timer_get_time();
    if (wifi_is_connected()) {
        esp_wifi_disconnect();
        while (wifi_is_connected()) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    esp_wifi_connect();
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
        if (esp_timer_get_time() - start_time > 15 * 1000 * 1000) {
            printf("Connection timeout\n");
            return false;
        }
    }
    return true;
}

void wifi_get_ap_info(const char** ssid, const char** password)
{
    static const char* s_ssid = WIFI_AP_SSID;
    static const char* s_password = WIFI_AP_PASSWD;
    *ssid = s_ssid;
    *password = s_password;
}

const char* wifi_get_ap_ip_address(void)
{
    static char ip_buffer[32];
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(s_apif, &ip_info);
    sprintf(ip_buffer, "http://" IPSTR "", IP2STR(&ip_info.ip));
    return ip_buffer;
}

const char* wifi_get_sta_ip_address(void)
{
    static char ip_buffer[32];
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(s_netif, &ip_info);
    sprintf(ip_buffer, IPSTR, IP2STR(&ip_info.ip));
    return ip_buffer;
}
