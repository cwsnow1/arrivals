#include <math.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_log.h"
#include "esp_lcd_io_spi.h"
#include "esp_timer.h"

#include "esp_lcd_touch.h"
#include "esp_lcd_touch_ft6x36.h"

#include "display.h"
#include "config.h"

#include "lvgl.h"

static const char* TAG = "display";

#define LCD_BPP             (16)

#ifndef MIN
#define MIN(x, y)   (x < y ? x : y)
#endif
#ifndef MAX
#define MAX(x, y)   (x < y ? y : x)
#endif

#define TOUCH_QUEUE_SIZE    (4)

#define LVGL_TICK_PERIOD_MS (5)

#define BACKLIGHT_FREQUENCY (250)
#define MIN_BRIGHTNESS      ((1 << BACKLIGHT_RESOULTION) / 8)

#define SPI_FREQ_HZ         (24 * 1000 * 1000)

static QueueHandle_t s_touch_q;
static _lock_t lvgl_api_lock;
static int64_t s_last_touch_time;
static bool s_display_initialized = false;
static TaskHandle_t s_lvgl_task = NULL;
static TaskHandle_t s_touch_task = NULL;
static esp_timer_handle_t adjust_brightness_timer = NULL;
static uint8_t target_brightness;

static bool get_touch(esp_lcd_touch_point_data_t* out)
{
    return xQueueReceive(s_touch_q, out, 0) == pdTRUE;
}

static void touch_isr(esp_lcd_touch_handle_t tp)
{
    SemaphoreHandle_t mutex = (SemaphoreHandle_t) tp->config.user_data;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(mutex, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void touch_task(void* ctx)
{
    const display_cfg_t* cfg = (const display_cfg_t*) ctx;
    SemaphoreHandle_t touch_mux = xSemaphoreCreateBinary();
    s_touch_q = xQueueCreate(TOUCH_QUEUE_SIZE, sizeof(esp_lcd_touch_point_data_t));
    /* Initialize touch HW */
    __unused const esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H,
        .y_max = LCD_V,
        .rst_gpio_num = cfg->touch.reset,
        .int_gpio_num = cfg->touch.interrupt,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
        .interrupt_callback = touch_isr,
        .user_data = touch_mux,
    };

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_touch_handle_t touch_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_FT6x36_CONFIG();
    tp_io_config.scl_speed_hz = 400 * 1000;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(cfg->touch.bus, &tp_io_config, &tp_io_handle));
    //esp_lcd_touch_new_i2c_ft6x36(tp_io_handle, &tp_cfg, &touch_handle);
    ESP_LOGI(TAG, "Touch controller init complete");

    for (;;) {
        xSemaphoreTake(touch_mux, portMAX_DELAY);
        s_last_touch_time = esp_timer_get_time();
        if (esp_lcd_touch_read_data(touch_handle) != ESP_OK) {
            continue;
        }
        uint8_t num;
        esp_lcd_touch_point_data_t t;
        esp_lcd_touch_get_data(touch_handle, &t, &num, 1);
        xQueueSendToBack(s_touch_q, &t, 0);
    }
}

static void deinit_backlight(void)
{
    display_set_brightness(0, 500);
    vTaskDelay(pdMS_TO_TICKS(600));
}

static void init_backlight(gpio_num_t backlight_pin)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = BACKLIGHT_RESOULTION,
        .timer_num        = LEDC_TIMER_0,
        .freq_hz          = BACKLIGHT_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK,
        .deconfigure      = false,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .gpio_num       = backlight_pin,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = LEDC_TIMER_0,
        .duty           = 0,
        .hpoint         = 0,
        .flags          =
            { .output_invert = 0, }
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    ESP_ERROR_CHECK(ledc_fade_func_install(0));
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 0);
    int64_t brightness = 0;
    if (!config_get_int("brightness", &brightness)) {
        brightness = MAX_BRIGHTNESS;
        config_set_int("brightness", brightness);
    }
    display_set_brightness((uint8_t) brightness, 500);
    esp_register_shutdown_handler(deinit_backlight);
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // because SPI LCD is big-endian, we need to swap the RGB bytes order
    lv_draw_sw_rgb565_swap(px_map, (offsetx2 + 1 - offsetx1) * (offsety2 + 1 - offsety1));
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static void lvgl_touch_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    esp_lcd_touch_point_data_t t;
    if (get_touch(&t)) {
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = t.x;
        data->point.y = t.y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io,
                                    esp_lcd_panel_io_event_data_t* edata,
                                    void* user_ctx)
{
    lv_display_t *disp = (lv_display_t*) user_ctx;
    lv_display_flush_ready(disp);
    return false;
}

static void lvgl_port_task(void* arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1) {
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        vTaskDelay(pdMS_TO_TICKS(time_till_next_ms));
    }
}

void display_lock(bool en)
{
    if (en)
        _lock_acquire(&lvgl_api_lock);
    else
        _lock_release(&lvgl_api_lock);
}

static void init_lvgl(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t io)
{
    lv_init();

    lv_display_t* display = lv_display_create(LCD_H, LCD_V);
    uint16_t* frame_buffer = heap_caps_calloc(1, LCD_H * (LCD_V / 4) * sizeof(*frame_buffer), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint16_t* frame_buffer2 = heap_caps_calloc(1, LCD_H * (LCD_V / 4) * sizeof(*frame_buffer), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    assert(frame_buffer);
    lv_display_set_buffers(display, frame_buffer, frame_buffer2, LCD_H * (LCD_V / 4) * sizeof(*frame_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_user_data(display, panel);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(display, lvgl_flush_cb);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Tick interface for LVGL (using esp_timer to generate 2ms periodic event)
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = notify_lvgl_flush_ready,
    };
    /* Register done callback */
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io, &cbs, display));

    static lv_indev_t *indev;
    indev = lv_indev_create(); // Input device driver (Touch)
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_display(indev, display);
    lv_indev_set_read_cb(indev, lvgl_touch_cb);

    ESP_LOGI(TAG, "Create LVGL task");
    _lock_init(&lvgl_api_lock);
    xTaskCreate(lvgl_port_task, "LVGL", 8192, NULL, 2, &s_lvgl_task);
}

void display_init(const display_cfg_t* cfg)
{
    xTaskCreate(touch_task, "touch_task", 3072, (void*) cfg, 12, &s_touch_task);

    // --- Init SPI Bus
    ESP_LOGI(TAG, "Initializing SPI Bus...");
    const spi_bus_config_t buscfg = {
        .mosi_io_num = cfg->lcd.mosi,
        .miso_io_num = -1,
        .sclk_io_num = cfg->lcd.sclk,
        .data2_io_num = -1,
        .data3_io_num = -1,
        .max_transfer_sz = LCD_H * 80 * LCD_BPP / 8,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    // --- Init ESP_LCD IO
    ESP_LOGI(TAG, "Initializing panel IO...");
    esp_lcd_panel_io_handle_t io_handle = NULL;

    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = cfg->lcd.cs,
        .dc_gpio_num = cfg->lcd.dc,
        .pclk_hz = SPI_FREQ_HZ,
        .trans_queue_depth = 4,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t) SPI2_HOST, &io_config, &io_handle));
    // --- Create esp_lcd panel
    ESP_LOGI(TAG, "Creating st7789 panel driver...");
    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = cfg->lcd.reset,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = LCD_BPP,
    };
    esp_lcd_panel_handle_t panel_handle;
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    // --- Reset the display
    ESP_LOGI(TAG, "Resetting display...");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    // --- Initialize LCD panel
    ESP_LOGI(TAG, "Initializing display...");
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    // Turn on the screen
    ESP_LOGI(TAG, "Turning display on...");
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    init_lvgl(panel_handle, io_handle);
    s_display_initialized = true;
    init_backlight(cfg->lcd.backlight);
    esp_register_shutdown_handler(display_sleep);

    ESP_LOGI(TAG, "Init complete");
}

void display_sleep(void) {
    if (!s_display_initialized) {
        return;
    }
    esp_lcd_panel_handle_t panel = lv_display_get_user_data(lv_display_get_default());
    esp_lcd_panel_disp_on_off(panel, false);

    if (xTaskGetCurrentTaskHandle() != s_lvgl_task)
        vTaskSuspend(s_lvgl_task);
    vTaskSuspend(s_touch_task);
}

static void adjust_brightness(void* ctx)
{
    uint8_t current_brightness = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    if (current_brightness == target_brightness) {
        esp_timer_stop(adjust_brightness_timer);
        esp_timer_delete(adjust_brightness_timer);
        adjust_brightness_timer = NULL;
        return;
    }
    if (current_brightness < target_brightness) {
        ++current_brightness;
    } else {
        --current_brightness;
    }
    ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, current_brightness, 0);
}

void display_set_brightness(uint8_t brightness, uint32_t transition_time_ms)
{
    if (!s_display_initialized) {
        return;
    }
    if (transition_time_ms == 0) {
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, brightness, 0);
        return;
    }
    uint8_t current_brightness = ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    uint8_t diff = MAX(brightness, current_brightness) - MIN(brightness, current_brightness);
    if (diff == 0) {
        // After a crash, ledc_get_duty gives value before crash when the real value is 0
        ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, brightness, 0);
        return;
    }
    target_brightness = brightness;
    uint64_t period_us = (transition_time_ms * 1000) / diff;
    const esp_timer_create_args_t adjust_brightness_args = {
        .callback = &adjust_brightness,
        .name = "brightness_adjust"
    };
    if (esp_timer_is_active(adjust_brightness_timer)) {
        esp_timer_stop(adjust_brightness_timer);
        esp_timer_delete(adjust_brightness_timer);
    }
    ESP_ERROR_CHECK(esp_timer_create(&adjust_brightness_args, &adjust_brightness_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(adjust_brightness_timer, period_us));
}

uint8_t display_get_brightness(void)
{
    return ledc_get_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

bool display_is_initialized(void)
{
    return s_display_initialized;
}
