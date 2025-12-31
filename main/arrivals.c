#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "nvs_flash.h"
#include "led_strip.h"

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

    #define NUM_LEDS 400
    led_strip_config_t led_cfg = {
        .strip_gpio_num = GPIO_NUM_45,
        .max_leds = NUM_LEDS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = { .invert_out = 0 },
    };
    led_strip_spi_config_t spi_cfg = {
        .clk_src = SPI_CLK_SRC_DEFAULT,
        .spi_bus = SPI3_HOST,
        .flags = { .with_dma = 1 },
    };
    led_strip_handle_t led_handle = NULL;
    ESP_ERROR_CHECK(led_strip_new_spi_device(&led_cfg, &spi_cfg, &led_handle));
    for (size_t j = 0;;++j) {
        size_t lit_index = j % NUM_LEDS;
        for (size_t i = 0; i < NUM_LEDS; ++i) {
            if (i == lit_index)
                led_strip_set_pixel(led_handle, i, 1, 0, 0);
            else
                led_strip_set_pixel(led_handle, i, 0, 0, 0);
        }
        led_strip_refresh(led_handle);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return;

    wifi_init(CONFIG_SSID, CONFIG_PASSWORD);
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    sync_time(true);
    for (;; vTaskDelay(pdMS_TO_TICKS(10000))) {
        api_get();
    }
}
