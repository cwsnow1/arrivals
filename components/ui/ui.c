#include "lvgl.h"

#include "ui.h"

static lock_fn_t s_lock;

LV_FONT_DECLARE(arimo_24);
LV_FONT_DECLARE(arimo_32);

static lv_obj_t* rows[UI_NUM_ROWS];
static lv_style_t row_styles[UI_NUM_ROWS];
static lv_obj_t* station_label;

void ui_init(lock_fn_t lock_fn)
{
    s_lock = lock_fn;
    s_lock(true);

    lv_obj_set_style_bg_color(lv_screen_active(), (lv_color_t) { 11, 11, 11 }, 0);

    station_label = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(station_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_label_set_text(station_label, "N/A");
    lv_obj_set_style_text_font(station_label, &arimo_24, 0);
    lv_obj_set_style_text_color(station_label, (lv_color_t) { 255, 255, 255 }, 0);
    lv_obj_align(station_label, LV_ALIGN_TOP_LEFT, 5, 5);

    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);


    for (size_t i = 0; i < UI_NUM_ROWS; ++i) {
        rows[i] = lv_obj_create(lv_screen_active());
        lv_obj_set_style_border_width(rows[i], 1, 0);
        lv_obj_set_style_border_color(rows[i], (lv_color_t) { 0, 0, 0 }, 0);
        lv_obj_set_size(rows[i], UI_W, 7 * UI_H / 24);
        lv_style_init(&row_styles[i]);
        lv_style_set_bg_color(&row_styles[i], (lv_color_t) { 100, 100, 100 });
        lv_style_set_text_color(&row_styles[i], (lv_color_t) { 255, 255, 255 });
        lv_obj_add_style(rows[i], &row_styles[i], 0);
        lv_obj_align(rows[i], LV_ALIGN_TOP_LEFT, 0, (UI_H / 8) + (7 * UI_H * i / 24));
        lv_obj_set_style_radius(rows[i], 0, 0);
        lv_obj_set_style_pad_all(rows[i], 0, 0);

        lv_obj_t* lbl;

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "Not connnected");
        lv_obj_set_style_text_font(lbl, &arimo_24, 0);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_width(lbl, 2 * UI_W / 3);
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, 5);

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "Not connnected");
        lv_obj_set_style_text_font(lbl, &arimo_32, 0);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_width(lbl, 2 * UI_W / 3);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_LEFT, 5, 0);

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "10 min");
        lv_obj_set_style_text_font(lbl, &arimo_32, 0);
        lv_obj_align(lbl, LV_ALIGN_RIGHT_MID, -5, 0);
    }

    s_lock(false);
}

void ui_set_row(int row_idx,
                const char* dest,
                int rn,
                const char* line_name,
                lv_color_t line_color,
                int min_remaining)
{
    s_lock(true);
    lv_obj_t* lbl;

    lv_style_set_bg_color(&row_styles[row_idx], line_color);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 0, &lv_label_class);
    lv_label_set_text_fmt(lbl, "%s Line #%d to", line_name, rn);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 1, &lv_label_class);
    lv_label_set_text(lbl, dest);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 2, &lv_label_class);
    lv_label_set_text_fmt(lbl, "%d min", min_remaining);

    s_lock(false);
}

void ui_set_station(const char* station_name)
{
    s_lock(true);

    lv_label_set_text_fmt(station_label, "Next 'L' service at %s", station_name);

    s_lock(false);
}
