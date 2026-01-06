
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "raylib.h"
#include "lvgl.h"
#include "ui.h"

static uint16_t frame_buffer[LCD_H * LCD_V];

static lv_indev_t* indev;
static lv_indev_t* button_indev;

static void action_cb(void* user_data)
{
    printf("[ACTION] User data: %zu\n", (size_t) user_data);
}

static void lvgl_touch_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (IsMouseButtonDown(0)) {
        data->state = LV_INDEV_STATE_PRESSED;
        Vector2 pos = GetMousePosition();
        data->point.x = pos.x;
        data->point.y = pos.y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lvgl_button_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    if (IsKeyPressed(KEY_ENTER)) {
        data->btn_id = 1;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (IsKeyPressed(KEY_LEFT)) {
        data->btn_id = 0;
        data->state = LV_INDEV_STATE_PRESSED;
    } else if (IsKeyPressed(KEY_RIGHT)) {
        data->btn_id = 2;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    (void) disp;
    (void) area;
    memcpy(frame_buffer, px_map, sizeof(frame_buffer));
}

static void lvgl_flush_wait_cb(lv_display_t* disp)
{
    (void) disp;
    return;
}

static lv_display_t* init_lvgl(void)
{
    lv_init();

    lv_display_t* disp = lv_display_create(LCD_H, LCD_V);
    uint16_t* buffer = malloc(sizeof(frame_buffer));
    lv_display_set_buffers(disp, buffer, NULL, sizeof(frame_buffer), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);
    lv_display_set_flush_wait_cb(disp, lvgl_flush_wait_cb);
}

void lock(bool) {}

int main(void)
{
    InitWindow(LCD_H, LCD_V, "lvgl");
    SetTargetFPS(60);

    lv_display_t* disp = init_lvgl();

    Image im = {
        .data = frame_buffer,
        .width = LCD_H,
        .height = LCD_V,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R5G6B5
    };
    Texture2D tex = LoadTextureFromImage(im);

    float t = 0;
    float t_prev = 0;
    float last_timer_tick = 0;
    float t_next = 0;

    ui_init(lock);

    while (!WindowShouldClose()) {
        t += GetFrameTime();
        lv_display_flush_ready(disp);
        lv_tick_inc(1000 * (t - t_prev));
        if (t >= t_next) {
            uint32_t next_time = lv_timer_handler();
            t_next = t +next_time/ 1000.0f;
        }

        UpdateTexture(tex, frame_buffer);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tex, 0, 0, WHITE);
        EndDrawing();
        t_prev = t;
    }
    CloseWindow();
}
