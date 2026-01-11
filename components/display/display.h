#pragma once

#include <stdbool.h>

#include "driver/i2c_master.h"

#define LCD_H                   (320)
#define LCD_V                   (240)
#define BACKLIGHT_RESOULTION    (8)
#define MAX_BRIGHTNESS          (1 << BACKLIGHT_RESOULTION)

typedef struct {
    struct {
        int mosi;
        int sclk;
        int dc;
        int cs;
        int backlight;
        int reset;
    } lcd;
    struct {
        i2c_master_bus_handle_t bus;
        int reset;
        int interrupt;
    } touch;
} display_cfg_t;

/**
 * @brief Initializes SPI bus and display.
 *        Must be called before any other display_*
 *        functions
 * 
 */
void display_init(const display_cfg_t* cfg);

/**
 * @brief Sets display to its internal sleep mode
 * 
 */
void display_sleep(void);

uint8_t display_get_brightness(void);
void display_set_brightness(uint8_t brightness, uint32_t transition_time_ms);
void display_lock(bool en);
