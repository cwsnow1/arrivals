#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "nvs_flash.h"

#include "wifi.h"
#include "api.h"

static const char* TAG = "main";

static void sync_time(bool required)
{
    ESP_LOGI(TAG, "Synchronizing time");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    if (esp_netif_sntp_init(&config) != ESP_OK) {
        // time has already been or is being synchronized
        return;
    }
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(60000)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update system time within 60s timeout");
        if (required) {
            esp_restart();
        }
    } else {
        ESP_LOGI(TAG, "Time synchronized");
    }
}

void app_main(void)
{
    nvs_flash_init();
    setenv("TZ", "CST6CDT,M3.2.0/2:00:00,M11.1.0/2:00:00", 1);
    tzset();

    wifi_init(CONFIG_SSID, CONFIG_PASSWORD);
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    sync_time(true);
    for (;; vTaskDelay(pdMS_TO_TICKS(2000))) {
        api_get();
    }
}
