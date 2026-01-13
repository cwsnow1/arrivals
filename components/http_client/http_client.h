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
http_response_t http_get_and_keep_open(const char* url);
void http_close(void);
void http_firmware_upgrade(void);

#ifdef __cplusplus
}
#endif
