#include "ui_callbacks.h"
#include "wifi.h"

void ui_get_ap_info(const char** ssid, const char** password)
{
    wifi_get_ap_info(ssid, password);
}

const char* ui_get_ap_ip_address(void)
{
    return wifi_get_ap_ip_address();
}