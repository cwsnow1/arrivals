#pragma once

#include <stdbool.h>
#include <stdint.h>

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
} config_param_t;

void config_init(void);
void config_set_string(const char* key, const char* value);
void config_set_int(const char* key, int64_t value);
char* config_get_string(const char* key);
bool config_get_int(const char* key, int64_t* out);
const config_param_t* config_get_params(size_t* count);

#ifdef __cplusplus
}
#endif
