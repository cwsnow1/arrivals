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

static const char* TAG = "wifi";

static SemaphoreHandle_t s_connected;

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

void wifi_init(const char* ssid, const char* password)
{
    s_connected = xSemaphoreCreateBinary();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

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

    wifi_config_t wifi_cfg = { 0 };
    strncpy((char*) wifi_cfg.sta.ssid, ssid, sizeof(wifi_cfg.sta.ssid) - 1);
    strncpy((char*) wifi_cfg.sta.password, password, sizeof(wifi_cfg.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    ESP_ERROR_CHECK(esp_wifi_connect());
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
