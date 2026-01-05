#pragma once

#include "cta.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float progress;
    stop_id_t next_stop;
    uint16_t rn;
} train_t;

typedef struct {
    train_t* trains;
    size_t count;
} line_t;

line_t* api_get(void);

#ifdef __cplusplus
}
#endif
