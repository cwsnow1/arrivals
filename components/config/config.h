#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CONFIG_TYPE_INT,
    CONFIG_TYPE_STRING,
} config_param_type_t;

typedef struct {
    const char* key;
    config_param_type_t type;
    void (*update_cb)(void*);
    union {
        const char* s;
        int64_t i;
    } default_val;
} config_param_t;

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
    LED_BRIGHTNESS_LOW,
    LED_BRIGHTNESS_MED,
    LED_BRIGHTNESS_HIGH,
} led_brightness_t;

typedef enum {
    LCD_MODE_OFF,
    LCD_MODE_ARRIVALS,
} lcd_mode_t;

void config_init(void);
void config_set_string(const char* key, const char* value);
void config_set_int(const char* key, int64_t value);
char* config_get_string(const char* key);
bool config_get_int(const char* key, int64_t* out);
const config_param_t* config_get_params(size_t* count);

#ifdef __cplusplus
}
#endif
