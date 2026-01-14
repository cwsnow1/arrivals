#pragma once

#include "cta.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float progress;
    stop_id_t next_stop;
    uint32_t eta_ms;
    uint16_t rn;
    uint16_t original_eta;
    uint8_t arr     : 1;
    uint8_t delayed : 1;
} train_t;

typedef struct {
    train_t* trains;
    size_t count;
} line_t;

typedef struct {
    char destination[128];
    line_name_t line;
    int rn;
    int eta;
} expected_train_t;

typedef struct {
    expected_train_t* trains;
    const char* station_name;
    size_t count;
} expected_trains_t;

line_t api_get(line_name_t line);
expected_trains_t api_get_expected(station_id_t station);
line_t api_update_eta(line_name_t line, uint16_t time_step);

#ifdef __cplusplus
}
#endif
