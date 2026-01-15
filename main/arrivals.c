#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_ota_ops.h"
#include "esp_timer.h"
#include "led_strip.h"
#include "nvs_flash.h"

#include "api.h"
#include "config.h"
#include "cta.h"
#include "display.h"
#include "http_server.h"
#include "http_client.h"
#include "ui.h"
#include "wifi.h"

#define RANDOM_COLOR_MAX    (4)
#ifndef MIN
#define MIN(x, y) (x < y ? x : y)
#endif

static const char* TAG = "main";

static void firmware_update_task(void*)
{
    int64_t autoupdate = 0;
    for (;; vTaskDelay(pdMS_TO_TICKS(60 * 60 * 1000))) {
        config_get_int("autoupdate", &autoupdate);
        if (autoupdate)
            http_firmware_upgrade();
    }
}

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

static void _led_strip_set_pixel(led_strip_handle_t led_handle, uint32_t index, uint32_t r, uint32_t g, uint32_t b)
{
    static uint32_t scale = 0;
    if (!scale) {
        int64_t tmp;
        config_get_int("led_bright", &tmp);
        switch (tmp) {
            case LED_BRIGHTNESS_LOW:
            scale = 1;
            break;
            case LED_BRIGHTNESS_MED:
            scale = 2;
            break;
            case LED_BRIGHTNESS_HIGH:
            scale = 4;
            break;
        }
    }
    r = MIN(UINT8_MAX, r * scale);
    g = MIN(UINT8_MAX, g * scale);
    b = MIN(UINT8_MAX, b * scale);
    led_strip_set_pixel(led_handle, index, r, g, b);
}

static void light_stop(led_strip_handle_t led_handle, led_segment_t segment, color_t color, float progress)
{
    if (progress == 1.0f || segment.rail.count == 0) {
        _led_strip_set_pixel(led_handle, segment.station, color.r, color.g, color.b);
    } else {
        size_t index = segment.rail.start + (int) roundf(progress * segment.rail.count);
        _led_strip_set_pixel(led_handle, index, 1 * color.r, 1 * color.g, 1 * color.b);
    }
}

static void marquee(void* user_data)
{
    ESP_LOGI(TAG, "Marquee task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    const uint16_t* indices[LINE_COUNT];
    size_t counts[LINE_COUNT];
    for (size_t i = 0; i < LINE_COUNT; ++i) {
        indices[i] = cta_get_leds_for_line(i, &counts[i]);
    }
    int64_t ret;
    config_get_int("m_train_len", &ret);
    size_t trail_size = (size_t) ret;
    config_get_int("m_num_train", &ret);
    marquee_train_count_t count = (marquee_train_count_t) ret;
    size_t num_trains[LINE_COUNT];
    for (line_name_t l = 0; l < LINE_COUNT; ++l) {
        if (count == MARQUEE_TRAIN_COUNT_LOW) {
            num_trains[l] = 1;
        } else if (count == MARQUEE_TRAIN_COUNT_MEDIUM) {
            num_trains[l] = (size_t) ceilf((float) counts[l] /  60.0f);
        } else {
            num_trains[l] = (size_t) ceilf((float) counts[l] /  30.0f);
        }
    }

    config_get_int("m_period", &ret);
    TickType_t delay = pdMS_TO_TICKS(ret);


    for (size_t i = 0;; ++i) {
        for (line_name_t line = RED_LINE; line < LINE_COUNT; ++line) {
            for (size_t j = 0; j < num_trains[line]; ++j) {
                size_t index = (i + (j * counts[line] / num_trains[line])) % counts[line];
                size_t prev_index = (index - trail_size) % counts[line];
                color_t color = cta_get_led_color(line);
                _led_strip_set_pixel(led_handle, indices[line][prev_index], 0, 0, 0);
                _led_strip_set_pixel(led_handle, indices[line][index], color.r, color.g, color.b);
            }
        }
        led_strip_refresh(led_handle);
        vTaskDelay(delay);
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
            _led_strip_set_pixel(led_handle, indices[line][j], c.r, c.g, c.b);
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
        _led_strip_set_pixel(led_handle, i, color.r, color.g, color.b);
    }
    // Refreshing every 10 seconds because sometimes single-time writes don't work
    for (;; vTaskDelay(pdMS_TO_TICKS(10000))) {
        led_strip_refresh(led_handle);
    }
}

static void light_line(led_strip_handle_t led_handle, line_name_t line_index, line_t line, bool update_delay)
{
    static bool delay_flag[LINE_COUNT];
    if (update_delay)
        delay_flag[line_index] = !delay_flag[line_index];
    for (size_t j = 0; j < line.count; ++j) {
        led_segment_t segment = cta_get_leds(line.trains[j].next_stop, line_index);
        if (segment.station == 0) {
            continue;
        }
        if (line.trains[j].delayed && delay_flag[line_index]) {
            light_stop(led_handle, segment, (color_t) { .r = 1, .g = 0 , .b = 0}, line.trains[j].progress);
        } else {
            light_stop(led_handle, segment, cta_get_led_color(line_index), line.trains[j].progress);
        }
    }
}

static _lock_t tracking_lock;

static void update_lines_task(void* user_data)
{
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    int64_t t = esp_timer_get_time();
    for (;; vTaskDelay(pdMS_TO_TICKS(500))) {
        _lock_acquire(&tracking_lock);
        for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
            _led_strip_set_pixel(led_handle, i, 0, 0, 0);
        }
        int64_t now = esp_timer_get_time();
        uint16_t inc = (uint16_t)((esp_timer_get_time() - t) / 1000);
        t = now;
        for (line_name_t l = 0; l < LINE_COUNT; ++l) {
            line_t line = api_update_eta(l, inc);
            light_line(led_handle, l, line, true);
        }
        led_strip_refresh(led_handle);
        _lock_release(&tracking_lock);
    }
}

static void live_tracking(void* user_data)
{
    ESP_LOGI(TAG, "Live tracking task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    _lock_init(&tracking_lock);

    int64_t update_interval;
    config_get_int("led_period", &update_interval);

    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    for (line_name_t l = RED_LINE; l < LINE_COUNT; ++l) {
        light_line(led_handle, l, api_get(l), true);
    }
    led_strip_refresh(led_handle);

    xTaskCreate(update_lines_task, "update_lines_task", 4096, led_handle, 4, NULL);

    int64_t t = esp_timer_get_time();
    for (line_name_t line_to_update = RED_LINE;; ++line_to_update, vTaskDelay(pdMS_TO_TICKS(update_interval))) {
        _lock_acquire(&tracking_lock);
        for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
            _led_strip_set_pixel(led_handle, i, 0, 0, 0);
        }
        if (line_to_update == LINE_COUNT)
            line_to_update = RED_LINE;
        line_t line = api_get(line_to_update);
        light_line(led_handle, line_to_update, line, false);
        int64_t now = esp_timer_get_time();
        uint16_t inc = (uint16_t) ((now - t) / 1000);
        t = now;
        for (line_name_t l = 0; l < LINE_COUNT; ++l) {
            if (l == line_to_update) continue;
            line_t line = api_update_eta(l, inc);
            light_line(led_handle, l, line,false);
        }

        led_strip_refresh(led_handle);
        _lock_release(&tracking_lock);
    }
}

static void change_color(uint8_t* c)
{
    int8_t diff = rand() % 3;
    switch (diff) {
        case 0:
        if (*c > 0) {
            *c -= 1;
        }
        break;
        case 1:
        break;
        case 2:
        if (*c < RANDOM_COLOR_MAX) {
            *c += 1;
        }
        break;
    }
}

static void random_colors(void* user_data)
{
    ESP_LOGI(TAG, "Random color task entered");
    led_strip_handle_t led_handle = (led_strip_handle_t) user_data;
    color_t* color = malloc(sizeof(*color) * CTA_NUM_LEDS);
    for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
        color[i].r = rand() % RANDOM_COLOR_MAX;
        color[i].g = rand() % RANDOM_COLOR_MAX;
        color[i].b = rand() % RANDOM_COLOR_MAX;
    }
    for (;; vTaskDelay(pdMS_TO_TICKS(20))) {
        for (size_t i = 0; i < CTA_NUM_LEDS; ++i) {
            switch (rand() % 3) {
                case 0:
                change_color(&color[i].r);
                break;
                case 1:
                change_color(&color[i].g);
                break;
                case 2:
                change_color(&color[i].b);
                break;
            }
            _led_strip_set_pixel(led_handle, i, color[i].r, color[i].g, color[i].b);
        }
        led_strip_refresh(led_handle);
    }
}

static void arrivals_lcd(void* user_data)
{
    ESP_LOGI(TAG, "arrivals_lcd entry");
    ui_connecting(false);
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    ui_connecting(true);
    ui_arrivals();
    int64_t ret;
    config_get_int("lcd_period", &ret);
    TickType_t delay_ticks = pdMS_TO_TICKS(ret);
    config_get_int("station", &ret);
    const station_id_t station = (station_id_t) ret;
    for (;; vTaskDelay(delay_ticks)) {
        expected_trains_t trains = api_get_expected(station);
        if (trains.count == 0) continue;
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
        task = random_colors;
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

    char* ssid = config_get_string("ssid");
    char* password = config_get_string("password");
    if (ssid == NULL || ap) {
        printf("ssid=%p, ap=%lld\n", ssid, ap);
        wifi_init_softap();
        http_server_start();
        display_set_brightness(255, 500);
        ui_ip();
    } else {
        int64_t lcd_mode;
        config_get_int("lcd_mode", &lcd_mode);
        if (lcd_mode == LCD_MODE_ARRIVALS) {
            xTaskCreate(arrivals_lcd, "lcd_task", 8192, NULL, 4, NULL);
        }
        wifi_connect(ssid, password);
        while (!wifi_is_connected()) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if (lcd_mode == LCD_MODE_OFF) {
            ui_connecting(true);
            display_set_brightness(0, 5000);
        }
        http_server_start();
        sync_time();
        xTaskCreate(firmware_update_task, "firmware_update_task", 8192, NULL, tskIDLE_PRIORITY, NULL);
    }
    vTaskDelay(pdMS_TO_TICKS(5000));
    esp_ota_mark_app_valid_cancel_rollback();
}
