#pragma once

#include <stdbool.h>

void wifi_init(void);
void wifi_connect(const char* ssid, const char* password);
void wifi_init_softap(void);
bool wifi_is_connected(void);
bool wifi_test_connection(const char* ssid, const char* password);
