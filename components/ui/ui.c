#include "lvgl.h"

#include "ui.h"
#include "callbacks/ui_callbacks.h"

static lock_fn_t s_lock;

LV_FONT_DECLARE(arimo_18);
LV_FONT_DECLARE(arimo_24);
LV_FONT_DECLARE(arimo_32);

static lv_obj_t* rows[UI_NUM_ROWS];
static lv_style_t row_styles[UI_NUM_ROWS];
static lv_obj_t* station_label;

void ui_init(lock_fn_t lock_fn)
{
    s_lock = lock_fn;
    s_lock(true);
    lv_obj_set_style_bg_color(lv_screen_active(), (lv_color_t) { 33, 33, 33 }, 0);
    s_lock(false);
}

void ui_ip(void)
{
    s_lock(true);

    const char* ssid, *password;
    ui_get_ap_info(&ssid, &password);
    const char* ip_address = ui_get_ap_ip_address();

    lv_obj_set_style_bg_color(lv_screen_active(), (lv_color_t) { 33, 33, 33 }, 0);
    lv_obj_set_style_text_color(lv_screen_active(), (lv_color_t) { 255, 255, 255 }, 0);

    lv_obj_t* lbl;

    lbl = lv_label_create(lv_screen_active());
    lv_label_set_text(lbl, "To configure:");
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(lbl, &arimo_24, 0);

    lbl = lv_label_create(lv_screen_active());
    lv_label_set_text_fmt(lbl, "SSID: %s", ssid);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_text_font(lbl, &arimo_24, 0);

    lbl = lv_label_create(lv_screen_active());
    lv_label_set_text_fmt(lbl, "Pass: %s", password);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_text_font(lbl, &arimo_24, 0);

    lbl = lv_label_create(lv_screen_active());
    lv_label_set_text_fmt(lbl, "%s", ip_address);
    lv_obj_align(lbl, LV_ALIGN_TOP_MID, 0, 130);
    lv_obj_set_style_text_font(lbl, &arimo_24, 0);

    s_lock(false);
}

void close_msgbox(lv_timer_t* timer)
{
    lv_obj_t* msg = lv_timer_get_user_data(timer);
    lv_msgbox_close(msg);
    lv_timer_delete(timer);
}

void ui_connecting(bool connected)
{
    static lv_obj_t* lbl;

    s_lock(true);

    if (connected) {
        if (lbl)
            lv_obj_delete(lbl);
        lv_obj_t* msg = lv_msgbox_create(NULL);
        lv_obj_set_style_text_font(msg, &arimo_32, 0);
        lv_msgbox_add_text(msg, ui_get_sta_ip_address());
        lv_timer_create(close_msgbox, 5 * 1000, msg);
        s_lock(false);
        return;
    }
    lbl = lv_label_create(lv_screen_active());
    lv_label_set_text_static(lbl, "Connecting...");
    lv_obj_set_style_text_font(lbl, &arimo_32, 0);
    lv_obj_set_style_text_color(lbl, (lv_color_t) { 255, 255, 255 }, 0);
    lv_obj_center(lbl);

    s_lock(false);
}

void ui_arrivals(void)
{
    s_lock(true);

    station_label = lv_label_create(lv_screen_active());
    lv_label_set_text(station_label, "Next 'L' services at");
    lv_obj_set_style_text_font(station_label, &arimo_18, 0);
    lv_obj_set_style_text_color(station_label, (lv_color_t) { 255, 255, 255 }, 0);
    lv_obj_align(station_label, LV_ALIGN_TOP_LEFT, UI_W / 20 + 5, 5);

    station_label = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(station_label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(station_label, &arimo_18, 0);
    lv_obj_set_style_text_color(station_label, (lv_color_t) { 255, 255, 255 }, 0);
    lv_obj_align(station_label, LV_ALIGN_TOP_LEFT, 175, 5);
    lv_label_set_text_static(station_label, "");
    lv_obj_set_width(station_label, 140);

    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);


    for (size_t i = 0; i < UI_NUM_ROWS; ++i) {
        lv_obj_t* row_lbl = lv_label_create(lv_screen_active());
        lv_label_set_text_fmt(row_lbl, "%zu", i + 1);
        lv_obj_set_style_text_font(row_lbl, &arimo_18, 0);
        lv_obj_set_style_text_color(row_lbl, (lv_color_t) { 255, 255, 255 }, 0);
        lv_obj_align(row_lbl, LV_ALIGN_TOP_LEFT, 3, 5 + (UI_H / 8) + (7 * UI_H * i / 24));

        rows[i] = lv_obj_create(lv_screen_active());
        lv_obj_set_style_border_width(rows[i], 1, 0);
        lv_obj_set_style_border_color(rows[i], (lv_color_t) { 0, 0, 0 }, 0);
        lv_obj_set_size(rows[i], 19 * UI_W / 20, 7 * UI_H / 24);
        lv_style_init(&row_styles[i]);
        lv_style_set_bg_color(&row_styles[i], (lv_color_t) { 100, 100, 100 });
        lv_style_set_text_color(&row_styles[i], (lv_color_t) { 255, 255, 255 });
        lv_obj_add_style(rows[i], &row_styles[i], 0);
        lv_obj_align(rows[i], LV_ALIGN_TOP_RIGHT, 0, (UI_H / 8) + (7 * UI_H * i / 24));
        lv_obj_set_style_radius(rows[i], 0, 0);
        lv_obj_set_style_pad_all(rows[i], 0, 0);

        lv_obj_t* lbl;

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "Not connnected");
        lv_obj_set_style_text_font(lbl, &arimo_18, 0);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_width(lbl, 3 * UI_W / 5);
        lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, 5);

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "Not connnected");
        lv_obj_set_style_text_font(lbl, &arimo_32, 0);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_width(lbl, 3 * UI_W / 5);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_LEFT, 5, -5);

        lbl = lv_label_create(rows[i]);
        lv_label_set_text(lbl, "10 min");
        lv_obj_set_style_text_font(lbl, &arimo_32, 0);
        lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_align(lbl, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
        lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
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
    lv_obj_invalidate(rows[row_idx]);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 0, &lv_label_class);
    lv_label_set_text_fmt(lbl, "%s Line #%d to", line_name, rn);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 1, &lv_label_class);
    lv_label_set_text(lbl, dest);

    lbl = lv_obj_get_child_by_type(rows[row_idx], 2, &lv_label_class);
    if (min_remaining > 0) {
        lv_label_set_text_fmt(lbl, "%d min", min_remaining);
        lv_obj_refr_size(lbl);
    } else if (min_remaining == 0) {
        lv_label_set_text_static(lbl, "Due");
        lv_obj_refr_size(lbl);
    } else {
        lv_label_set_text_static(lbl, "Delayed");
        lv_point_t dest_size;
        lv_text_get_size(&dest_size, dest, &arimo_32, 0, 0, LV_COORD_MAX, 0);
        if (dest_size.x > UI_W / 2) {
            lv_obj_set_width(lbl, 95);
        } else {
            lv_obj_refr_size(lbl);
        }
    }

    s_lock(false);
}

void ui_clear_row(int row_idx)
{
    lv_obj_t* lbl;

    s_lock(true);

    lv_style_set_bg_color(&row_styles[row_idx], (lv_color_t) { 33, 33, 33 });
    lv_obj_invalidate(rows[row_idx]);

    for (size_t i = 0; i < 3; ++i) {
        lbl = lv_obj_get_child_by_type(rows[row_idx], i, &lv_label_class);
        lv_label_set_text_static(lbl, "");
    }

    s_lock(false);
}

void ui_set_station(const char* station_name)
{
    s_lock(true);

    lv_label_set_text(station_label, station_name);

    s_lock(false);
}
