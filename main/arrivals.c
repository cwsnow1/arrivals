#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

#include "nvs_flash.h"

#include "wifi.h"
#include "api.h"

void app_main(void)
{
    nvs_flash_init();

    wifi_init(CONFIG_SSID, CONFIG_PASSWORD);
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    for (;; vTaskDelay(pdMS_TO_TICKS(500))) {
        api_get();
        printf("Free heap: %lu\n", esp_get_free_heap_size());
    }
}
