#include "ui_callbacks.h"

void ui_get_ap_info(const char** ssid, const char** password)
{
    *ssid = "TestAP";
    *password = "abc123";
}

const char* ui_get_ap_ip_address(void)
{
    return "http://192.168.1.4";
}

const char* ui_get_sta_ip_address(void)
{
    return "192.168.255.255";
}
