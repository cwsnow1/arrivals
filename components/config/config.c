#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "config.h"
#include "display.h"

#ifndef numberof
#define numberof(x) (sizeof(x) / sizeof(*(x)))
#endif

static const char* TAG = "config";
static nvs_handle_t s_handle;

static void update_brightness(void*);

static const config_param_t s_params[] = {
    { .key = "ssid",        .type = CONFIG_TYPE_STRING },
    { .key = "password",    .type = CONFIG_TYPE_STRING },
    { .key = "led_mode",    .type = CONFIG_TYPE_INT,    .default_val = { .i = LED_MODE_LINE_COLORS } },
    { .key = "led_bright",  .type = CONFIG_TYPE_INT,    .default_val = { .i = 0 }},
    { .key = "lcd_mode",    .type = CONFIG_TYPE_INT,    .default_val = { .i = LED_BRIGHTNESS_LOW } },
    { .key = "brightness",  .type = CONFIG_TYPE_INT,    .update_cb = update_brightness, .default_val = { .i = MAX_BRIGHTNESS } },
    { .key = "ap",          .type = CONFIG_TYPE_INT,    .default_val = { .i = 1 } },
    { .key = "api_key",     .type = CONFIG_TYPE_STRING, .default_val = { .s = CONFIG_API_KEY }},
    { .key = "station",     .type = CONFIG_TYPE_INT,    .default_val = { .i = 40380 }},
    { .key = "station_line",.type = CONFIG_TYPE_INT,    .default_val = { .i = 1 }},
    { .key = "m_num_train", .type = CONFIG_TYPE_INT,    .default_val = { .i = MARQUEE_TRAIN_COUNT_LOW }},
    { .key = "m_train_len", .type = CONFIG_TYPE_INT,    .default_val = { .i = 2 }},
    { .key = "m_period",    .type = CONFIG_TYPE_INT,    .default_val = { .i = 50 }},
    { .key = "lcd_period",  .type = CONFIG_TYPE_INT,    .default_val = { .i = 15000 }},
    { .key = "led_period",  .type = CONFIG_TYPE_INT,    .default_val = { .i = 4000 }},
    { .key = "autoupdate",  .type = CONFIG_TYPE_INT,    .default_val = { .i = 1 }},
    { .key = "ota_url",     .type = CONFIG_TYPE_STRING, .default_val = { .s = CONFIG_OTA_URL }},
};

static int get_param_index(const char* key)
{
    for (int i = 0; i < numberof(s_params); ++i) {
        if (!strcmp(s_params[i].key, key)) {
            return i;
        }
    }
    return -1;
}

void config_init(void)
{
    ESP_ERROR_CHECK(nvs_open("ns", NVS_READWRITE, &s_handle));
}

void config_set_string(const char* key, const char* value)
{
    int param_index = get_param_index(key);
    if (param_index == -1) {
        ESP_LOGE(TAG, "Param with key %s does not exist, check spelling or add it to master list", key);
        return;
    }
    if (s_params[param_index].type != CONFIG_TYPE_STRING) {
        ESP_LOGE(TAG, "Param with key %s does not match string type", key);
        return;
    }
    nvs_set_str(s_handle, key, value);
}

void config_set_int(const char* key, int64_t value)
{
    int param_index = get_param_index(key);
    if (param_index == -1) {
        ESP_LOGE(TAG, "Param with key %s does not exist, check spelling or add it to master list", key);
        return;
    }
    if (s_params[param_index].type != CONFIG_TYPE_INT) {
        ESP_LOGE(TAG, "Param with key %s does not match int type", key);
        return;
    }
    nvs_set_i64(s_handle, key, value);
}

char* config_get_string(const char* key)
{
    size_t len;
    if (nvs_get_str(s_handle, key, NULL, &len) != ESP_OK) {
        int index = get_param_index(key);
        if (index > 0 && s_params[index].default_val.s) {
            nvs_set_str(s_handle, key, s_params[index].default_val.s);
            char* ret = malloc(strlen(s_params[index].default_val.s));
            strcpy(ret, s_params[index].default_val.s);
            return ret;
        }
        return NULL;
    }
    char* ret = malloc(len);
    nvs_get_str(s_handle, key, ret, &len);
    return ret;
}

bool config_get_int(const char* key, int64_t* out)
{
    if (nvs_get_i64(s_handle, key, out) != ESP_OK) {
        int index = get_param_index(key);
        if (index > 0) {
            nvs_set_i64(s_handle, key, s_params[index].default_val.i);
            *out = s_params[index].default_val.i;
            return true;
        }
        *out = -1;
        return false;
    }
    return true;
}

const config_param_t* config_get_params(size_t* count)
{
    *count = numberof(s_params);
    return s_params;
}

static void update_brightness(void* user_data)
{
    uint8_t brightness = (uint8_t) (uintptr_t) user_data;
    display_set_brightness(brightness, 500);
}
