#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int status;
    int length;
    uint8_t* buffer;
} http_response_t;

http_response_t http_get(const char* url);

#ifdef __cplusplus
}
#endif
