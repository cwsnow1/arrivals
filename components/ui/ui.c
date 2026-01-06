#include "lvgl.h"

#include "ui.h"

static lock_fn_t s_lock;

LV_FONT_DECLARE(arimo_24);

void ui_init(lock_fn_t lock_fn)
{
    s_lock = lock_fn;
    s_lock(true);

    lv_obj_t* lbl = lv_label_create(lv_screen_active());
    lv_label_set_text(lbl, "95th/Dan Ryan");
    lv_obj_set_style_text_font(lbl, &arimo_24, 0);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
    lv_obj_set_width(lbl, 100);
    lv_obj_align(lbl, LV_ALIGN_LEFT_MID, 10, 0);

    s_lock(false);
}