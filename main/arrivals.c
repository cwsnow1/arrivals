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
#include "cta.h"
#include "display.h"

#include "lvgl.h"
#include "lv_demos.h"

static const char* TAG = "main";

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

static const color_t line_colors[LINE_COUNT] = {
    [RED_LINE]    = { .r = 3, .g = 0, .b = 0 },
    [BLUE_LINE]   = { .r = 0, .g = 2, .b = 4 },
    [GREEN_LINE]  = { .r = 3, .g = 3, .b = 0 },
    [BROWN_LINE]  = { .r = 1, .g = 1, .b = 0 },
    [PURPLE_LINE] = { .r = 1, .g = 1, .b = 4 },
    [YELLOW_LINE] = { .r = 3, .g = 3, .b = 0 },
    [PINK_LINE]   = { .r = 3, .g = 1, .b = 1 },
    [ORANGE_LINE] = { .r = 3, .g = 1, .b = 0 },
};

static void sync_time(void)
{
    ESP_LOGI(TAG, "Synchronizing time");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    if (esp_netif_sntp_init(&config) != ESP_OK) {
        // time has already been or is being synchronized
        return;
    }
    if (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(60000)) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to update system time within 60s timeout");
        esp_restart();
    } else {
        ESP_LOGI(TAG, "Time synchronized");
    }
}

static void light_stop(led_strip_handle_t led_handle, led_segment_t segment, color_t color, float progress)
{
    if (progress == 1.0f || segment.rail.count == 0) {
        led_strip_set_pixel(led_handle, segment.station, color.r, color.g, color.b);
    } else {
        int8_t increment = 1;
        if (segment.rail.count < 0) {
            increment = -1;
            segment.rail.count = -segment.rail.count;
        }
        for (size_t i = 0; i < segment.rail.count; ++i) {
            led_strip_set_pixel(led_handle, segment.rail.start, 1 * color.r, 1 * color.g, 1 * color.b);
            segment.rail.start += increment;
        }
    }
}

void app_main(void)
{
    nvs_flash_init();
    setenv("TZ", "CST6CDT,M3.2.0/2:00:00,M11.1.0/2:00:00", 1);
    tzset();

    i2c_master_bus_config_t i2c_cfg = {
        .i2c_port          = I2C_NUM_1,
        .sda_io_num        = GPIO_NUM_46,
        .scl_io_num        = GPIO_NUM_3,
        .clk_source        = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
    };
    i2c_master_bus_handle_t i2c_bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_cfg, &i2c_bus));
    static display_cfg_t display_cfg = {
        .lcd = {
            .mosi      = GPIO_NUM_11,
            .sclk      = GPIO_NUM_12,
            .dc        = GPIO_NUM_9,
            .cs        = GPIO_NUM_10,
            .reset     = GPIO_NUM_14,
            .backlight = GPIO_NUM_18,
        },
        .touch = {
            .interrupt = GPIO_NUM_21,
            .reset     = GPIO_NUM_8
        }
    };
    display_cfg.touch.bus = i2c_bus;
    display_init(&display_cfg);

    display_lock();
    lv_demo_widgets();
    display_unlock();

    #define NUM_LEDS 511
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

    wifi_init(CONFIG_SSID, CONFIG_PASSWORD);
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    sync_time();

    for (;;) {
        line_t* lines = api_get();
        for (size_t i = 0; i < NUM_LEDS; ++i) {
            led_strip_set_pixel(led_handle, i, 0, 0, 0);
        }
        for (size_t i = 0; i < LINE_COUNT; ++i) {
            for (size_t j = 0; j < lines[i].count; ++j) {
                led_segment_t segment = cta_get_leds(lines[i].trains[j].next_stop, i);
                if (segment.station == 0) {
                    ESP_LOGW(TAG, "Station LED index is 0, Line: %zu, stop_id=%d\n", i, lines[i].trains[j].next_stop);
                }
                light_stop(led_handle, segment, line_colors[i], lines[i].trains[j].progress);
            }
        }

        led_strip_refresh(led_handle);

        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
