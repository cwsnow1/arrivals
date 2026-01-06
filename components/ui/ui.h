#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*lock_fn_t)(bool);

void ui_init(lock_fn_t lock_fn);

#ifdef __cplusplus
}
#endif