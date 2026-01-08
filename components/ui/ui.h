#pragma once

#include <stdbool.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_W        (320)
#define UI_H        (240)
#define UI_NUM_ROWS (3)

typedef void(*lock_fn_t)(bool);

void ui_init(lock_fn_t lock_fn);
void ui_ip(void);
void ui_arrivals(void);
void ui_set_station(const char* station_name);
void ui_set_row(int row_idx,
                const char* dest,
                int rn,
                const char* line_name,
                lv_color_t line_color,
                int min_remaining);
void ui_clear_row(int row_idx);

#ifdef __cplusplus
}
#endif