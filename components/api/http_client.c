#include "esp_http_client.h"
#include "esp_log.h"

#include "wifi.h"
#include "private/http_client.h"

static const char* TAG = "http_client";

static esp_http_client_handle_t s_client;

static esp_err_t _http_event_handler(esp_http_client_event_t* evt)
{
    http_response_t* response = (http_response_t*) evt->user_data;
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_HEADERS_COMPLETE:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADERS_COMPLETE");
        break;
    case HTTP_EVENT_ON_STATUS_CODE:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_STATUS_CODE");
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        response->buffer = realloc(response->buffer, response->length + evt->data_len + 2);
        memcpy(response->buffer + response->length, evt->data, evt->data_len);
        response->length += evt->data_len;
        response->buffer[response->length + 1] = 0;
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        esp_http_client_set_redirection(evt->client);
        break;
    }
    return ESP_OK;
}

http_response_t http_get(const char* url)
{
    http_response_t r = { 0 };
    esp_http_client_config_t cfg = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = &r // using address of local variable is OK because esp_http_client_perform blocks
    };
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return r;
    esp_http_client_perform(client);
    r.status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);
    return r;
}

http_response_t http_get_and_keep_open(const char* url)
{
    http_response_t r = { 0 };
    if (s_client == NULL) {
        esp_http_client_config_t cfg = {
            .url = url,
            .event_handler = _http_event_handler,
            .user_data = &r, // using address of local variable is OK because esp_http_client_perform blocks
        };
        s_client = esp_http_client_init(&cfg);
        if (!s_client) return r;
    } else {
        esp_http_client_set_url(s_client, url);
        esp_http_client_set_user_data(s_client, &r);
    }

    esp_http_client_perform(s_client);
    r.status = esp_http_client_get_status_code(s_client);
    return r;
}

void http_close(void)
{
    if (s_client) {
        esp_http_client_cleanup(s_client);
        s_client = NULL;
    }
}
