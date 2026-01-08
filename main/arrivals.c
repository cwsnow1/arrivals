#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "nvs_flash.h"
#include "led_strip.h"

#include "wifi.h"
#include "api.h"
#include "cta.h"
#include "display.h"
#include "ui.h"
#include "config.h"

static const char* TAG = "main";

typedef enum {
    LED_MODE_OFF,
    LED_MODE_LINE_COLORS,
    LED_MODE_LIVE_TRACKING,
    LED_MODE_RANDOM_COLORS,
    LED_MODE_SOLID_COLOR,
    LED_MODE_MARQUEE,
    LED_MODE_BREATHING,
} led_mode_t;

typedef enum {
    LCD_MODE_OFF,
    LCD_MODE_ARRIVALS,
} lcd_mode_t;


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

static void marquee(void* user_data)
{
    ESP_LOGI(TAG, "Marquee task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    const uint16_t* indices[LINE_COUNT];
    size_t counts[LINE_COUNT];
    uint16_t prev[LINE_COUNT];
    for (size_t i = 0; i < LINE_COUNT; ++i) {
        indices[i] = cta_get_leds_for_line(i, &counts[i]);
        prev[i] = UINT16_MAX;
    }
    for (size_t i = 0;; ++i) {
        for (line_name_t line = RED_LINE; line < LINE_COUNT; ++line) {
            size_t index = i % counts[line];
            if (prev[line] != UINT16_MAX)
                led_strip_set_pixel(led_handle, prev[line], 0, 0, 0);
            led_strip_set_pixel(led_handle, indices[line][index], 64, 64, 64);
            prev[line] = indices[line][index];
        }
        led_strip_refresh(led_handle);
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static void line_colors(void* user_data)
{
    ESP_LOGI(TAG, "Line colors task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    const uint16_t* indices[LINE_COUNT];
    size_t counts[LINE_COUNT];
    for (line_name_t line = 0; line < LINE_COUNT; ++line) {
        indices[line] = cta_get_leds_for_line(line, &counts[line]);
        for (size_t j = 0; j < counts[line]; ++j) {
            color_t c = cta_get_led_color(line);
            led_strip_set_pixel(led_handle, indices[line][j], c.r, c.g, c.b);
        }
    }
    // Refreshing every 10 seconds because sometimes single-time writes don't work
    for (;; vTaskDelay(pdMS_TO_TICKS(10000))) {
        led_strip_refresh(led_handle);
    }
}

static void solid_color(void* user_data)
{
    ESP_LOGI(TAG, "Solid color task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    int64_t color_int;
    color_t color;
    if (!config_get_int("led_color", &color_int)) {
        ESP_LOGE(TAG, "Color not found! Defaulting to white");
        color.r = 1;
        color.g = 1;
        color.b = 1;
    } else {
        memcpy(&color, &color_int, sizeof color);
    }
    for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
        led_strip_set_pixel(led_handle, i, color.r, color.g, color.b);
    }
    // Refreshing every 10 seconds because sometimes single-time writes don't work
    for (;; vTaskDelay(pdMS_TO_TICKS(10000))) {
        led_strip_refresh(led_handle);
    }
}

static void live_tracking(void* user_data)
{
    ESP_LOGI(TAG, "Live tracking task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    for (;; vTaskDelay(pdMS_TO_TICKS(10000))) {
        line_t* lines = api_get();
        if (lines == NULL) {
            continue;
        }
        for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
            led_strip_set_pixel(led_handle, i, 0, 0, 0);
        }
        for (size_t i = 0; i < LINE_COUNT; ++i) {
            for (size_t j = 0; j < lines[i].count; ++j) {
                led_segment_t segment = cta_get_leds(lines[i].trains[j].next_stop, i);
                if (segment.station == 0) {
                    continue;
                }
                light_stop(led_handle, segment, cta_get_led_color(i), lines[i].trains[j].progress);
            }
        }

        led_strip_refresh(led_handle);
    }
}

static void arrivals_lcd(void* user_data)
{
    ui_arrivals();
    for (;; vTaskDelay(pdMS_TO_TICKS(150000))) {
        expected_trains_t trains = api_get_expected(CLARK_LAKE_BLUE_BROWN_GREEN_ORANGE_PURPLE_PINK);
        for (size_t i = 0; i < UI_NUM_ROWS; ++i) {
            if (i < trains.count) {
                lv_color_t lv_color;
                color_t color = cta_get_lcd_color(trains.trains[i].line);
                memcpy(&lv_color, &color, sizeof color);
                ui_set_row(i,
                            trains.trains[i].destination,
                            trains.trains[i].rn,
                            cta_get_line_name(trains.trains[i].line),
                            lv_color,
                            trains.trains[i].eta);
            } else {
                ui_clear_row(i);
            }
        }
        ui_set_station(trains.station_name);
    }
}

static void reset_button_cb(void* ctx)
{
    config_set_int("ap", 1);
    esp_restart();
}

void app_main(void)
{
    nvs_flash_init();
    config_init();
    setenv("TZ", "CST6CDT,M3.2.0/2:00:00,M11.1.0/2:00:00", 1);
    tzset();

    int64_t ap;
    if (!config_get_int("ap", &ap)) {
        config_set_int("ap", 1);
    }
    config_get_int("ap", &ap);
    if (ap) {
        config_set_int("ap", 0);
    }

    gpio_config_t reset_button_cfg = {
        .pin_bit_mask = BIT64(GPIO_NUM_0),
        .mode         = GPIO_MODE_INPUT,
        .intr_type    = GPIO_INTR_NEGEDGE,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&reset_button_cfg);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_0, reset_button_cb, NULL);

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

    ui_init(display_lock);

    if (ap) {
        ui_ip();
    } else {
        int64_t lcd_mode;
        config_get_int("lcd_mode", &lcd_mode);
        if (lcd_mode == LCD_MODE_ARRIVALS) {
            xTaskCreate(arrivals_lcd, "lcd_task", 8192, NULL, 4, NULL);
        } else if (lcd_mode == LCD_MODE_OFF) {
            display_set_brightness(0, 0);
        }
    }

    led_strip_config_t led_cfg = {
        .strip_gpio_num = GPIO_NUM_45,
        .max_leds = CTA_NUM_LEDS,
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

    led_strip_clear(led_handle);

    wifi_init();
    char* ssid = config_get_string("ssid");
    char* password = config_get_string("password");
    if (ssid == NULL || ap) {
        wifi_init_softap();
    } else {
        wifi_connect(ssid, password);
        while (!wifi_is_connected()) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        sync_time();
    }

    int64_t led_mode;
    config_get_int("led_mode", &led_mode);
    TaskFunction_t task = NULL;
    switch (led_mode) {
    case LED_MODE_OFF:
        break;
    case LED_MODE_LINE_COLORS:
        task = line_colors;
        break;
    case LED_MODE_LIVE_TRACKING:
        task = live_tracking;
        break;
    case LED_MODE_RANDOM_COLORS:
        break;
    case LED_MODE_SOLID_COLOR:
        task = solid_color;
        break;
    case LED_MODE_MARQUEE:
        task = marquee;
        break;
    case LED_MODE_BREATHING:
        break;
    }
    if (task)
        xTaskCreate(task, "led_task", 8192, led_handle, 4, NULL);
}
