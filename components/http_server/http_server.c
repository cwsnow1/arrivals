#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/param.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_wifi.h"
#include "esp_timer.h"

#include "config.h"
#include "http_server.h"
#include "wifi.h"
#include "json_parser.h"
#include "cta.h"

extern uint8_t index_html[];
extern size_t index_html_size;

static const char *TAG = "http_server";

static httpd_handle_t server;

static esp_err_t index_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        ESP_RETURN_ON_FALSE(buf, ESP_ERR_NO_MEM, TAG, "buffer alloc failed");
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
        }
        free(buf);
    }

    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    const char* resp_str = (const char*) index_html;
    httpd_resp_send(req, resp_str, index_html_size);

    return ESP_OK;
}

static const httpd_uri_t root_handler = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t index_handler = {
    .uri       = "/index.html",
    .method    = HTTP_GET,
    .handler   = index_get_handler,
    .user_ctx  = NULL
};

static esp_err_t config_post_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "POST received at /config");
    char* message_buffer = (char*) malloc(req->content_len + 1);
    if (message_buffer == NULL) {
        httpd_resp_send_500(req);
        return ESP_ERR_NO_MEM;
    }
    httpd_req_recv(req, message_buffer, req->content_len);
    message_buffer[req->content_len] = '\0';

    jparse_ctx_t ctx;
    if (json_parse_start(&ctx, message_buffer, req->content_len) != OS_SUCCESS) {
        httpd_resp_set_status(req, HTTPD_400);
        httpd_resp_send(req, "Unable to parse JSON", HTTPD_RESP_USE_STRLEN);
        json_parse_end(&ctx);
        return ESP_OK;
    }

    size_t param_count = 0;
    const config_param_t* params = config_get_params(&param_count);
    for (int i = 0; i < param_count; ++i) {
        char buffer[64];
        if (params[i].type == CONFIG_TYPE_STRING) {
            if (json_obj_get_string(&ctx, params[i].key, buffer, sizeof buffer) == OS_SUCCESS) {
                if (strlen(buffer) == 0) continue;
                config_set_string(params[i].key, buffer);
                if (params[i].update_cb) {
                    params[i].update_cb(buffer);
                }
            }
        } else {
            int64_t val = 0;
            if (json_obj_get_int64(&ctx, params[i].key, &val) != OS_SUCCESS) {
                if (json_obj_get_string(&ctx, params[i].key, buffer, sizeof buffer) != OS_SUCCESS) {
                    continue;
                }
                if (strlen(buffer) == 0) continue;
                val = strtoll(buffer, NULL, 10);
            }
            config_set_int(params[i].key, val);
            if (params[i].update_cb) {
                params[i].update_cb((void*) (uint32_t) val);
            }
        }
    }
    json_parse_end(&ctx);
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

typedef struct {
    char* buffer;
    size_t length;
    size_t capacity;
} string_t;

static void init_string(string_t* s)
{
    s->capacity = 32;
    s->length = 0;
    s->buffer = malloc(s->capacity);
}

#define APPEND_STRING(s, fmt, ...)                          \
    do {                                                    \
        size_t chars_needed = snprintf(s.buffer + s.length, \
            s.capacity - s.length,                          \
            fmt,                                            \
            __VA_ARGS__);                                   \
        if (s.capacity - s.length <= chars_needed) {        \
            s.capacity *= 2;                                \
            s.buffer = realloc(s.buffer, s.capacity);       \
        } else {                                            \
            s.length += chars_needed;                       \
            break;                                          \
        }                                                   \
    } while (true)

#define APPEND_STRING_NO_ARGS(s, fmt)                       \
    do {                                                    \
        size_t chars_needed = snprintf(s.buffer + s.length, \
            s.capacity - s.length,                          \
            fmt);                                           \
        if (s.capacity - s.length <= chars_needed) {        \
            s.capacity *= 2;                                \
            s.buffer = realloc(s.buffer, s.capacity);       \
        } else {                                            \
            s.length += chars_needed;                       \
            break;                                          \
        }                                                   \
    } while (true)

static esp_err_t config_get_handler(httpd_req_t* req)
{
    size_t param_count;
    const config_param_t* params = config_get_params(&param_count);
    string_t json;
    init_string(&json);
    if (json.buffer == NULL) {
        httpd_resp_send_500(req);
        return ESP_ERR_NO_MEM;
    }
    APPEND_STRING_NO_ARGS(json, "{");
    for (size_t i = 0; i < param_count; ++i) {
        if (!strcmp(params[i].key, "password")) continue;
        if (params[i].type == CONFIG_TYPE_STRING) {
            char* val = config_get_string(params[i].key);
            APPEND_STRING(json, "\"%s\":\"%s\",", params[i].key, val);
            free(val);
        } else {
            int64_t val = 0;
            config_get_int(params[i].key, &val);
            APPEND_STRING(json, "\"%s\":%lld,", params[i].key, val);
        }
    }
    json.buffer[json.length - 1] = '}';
    json.buffer[json.length] = '\0';
    httpd_resp_send(req, json.buffer, json.length);
    free(json.buffer);
    return ESP_OK;
}

static esp_err_t connect_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST received at /connect");
    char *message_buffer = (char*) malloc(req->content_len + 1);
    httpd_req_recv(req, message_buffer, req->content_len);
    message_buffer[req->content_len] = '\0';

    size_t ssid_length = 0;
    for (; message_buffer[ssid_length] != ','; ++ssid_length) {
        if (message_buffer[ssid_length] == '\0') {
            ESP_LOGE(TAG, "Error parsing connect POST\n");
            free(message_buffer);
            return ESP_FAIL;
        }
    }

    char ssid_buffer[32] = { 0 };
    char password_buffer[64] = { 0 };
    memcpy(ssid_buffer, message_buffer, ssid_length);
    memcpy(password_buffer, message_buffer + ssid_length + 1, req->content_len - ssid_length - 1);
    free(message_buffer);

    if (wifi_test_connection(ssid_buffer, password_buffer)) {
        httpd_resp_send(req, NULL, 0);
        config_set_string("ssid", ssid_buffer);
        config_set_string("password", password_buffer);
        vTaskDelay(pdMS_TO_TICKS(100));
        esp_restart();
    }
    httpd_resp_send_500(req);

    return ESP_OK;
}

static esp_err_t scan_wifi_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "POST received at /scan-wifi");
    esp_wifi_scan_start(NULL, true);
    uint16_t num_records = 5;
    wifi_ap_record_t* records = (wifi_ap_record_t*) malloc(sizeof(wifi_ap_record_t) * num_records);
    esp_wifi_scan_get_ap_records(&num_records, records);
    char* response_json = (char*) malloc(sizeof(records->ssid) * num_records + 40);
    if (response_json == NULL) {
        free(records);
        return ESP_ERR_NO_MEM;
    }
    int chars_written = sprintf(response_json, "{\"networks\":[");
    for (size_t i = 0; i < num_records; ++i) {
        chars_written += sprintf(response_json + chars_written, "\"%s\",", records[i].ssid);
    }
    response_json[chars_written - 1] = ']';
    response_json[chars_written] = '}';
    response_json[chars_written + 1] = '\0';

    free(records);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response_json, chars_written + 1);
    free(response_json);
    return ESP_OK;
}

static esp_err_t reboot_post_handler(httpd_req_t *req)
{
    httpd_resp_send(req, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(250));
    esp_restart();
    return ESP_OK;
}

static esp_err_t stations_get_handler(httpd_req_t* req)
{
    char buffer[32];
    char val_buffer[32];
    httpd_req_get_url_query_str(req, buffer, sizeof buffer);
    if (httpd_query_key_value(buffer, "station", val_buffer, sizeof val_buffer) != ESP_OK) {
        httpd_resp_set_status(req, HTTPD_400);
        httpd_resp_send(req, "Please provide the station index", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
    int index = strtol(val_buffer, NULL, 10);
    if (index >= LINE_COUNT || index < 0) {
        httpd_resp_set_status(req, HTTPD_400);
        httpd_resp_send(req, "Invalid station index", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    size_t station_count;
    const station_t* stations = cta_get_stations(&station_count);
    string_t json;
    init_string(&json);
    APPEND_STRING_NO_ARGS(json, "[");
    for (size_t i = 0; i < station_count; ++i) {
        if (stations[i].led_index[index]) {
            APPEND_STRING(json, "{\"id\":%d,\"name\":\"%s\"},", stations[i].id, stations[i].name);
        }
    }
    json.buffer[json.length - 1] = ']';
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json.buffer, json.length);
    free(json.buffer);
    return ESP_OK;
}

static const httpd_uri_t scan = {
    .uri       = "/scan-wifi",
    .method    = HTTP_POST,
    .handler   = scan_wifi_post_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t connect = {
    .uri       = "/connect",
    .method    = HTTP_POST,
    .handler   = connect_post_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t config_endpoint = {
    .uri       = "/config",
    .method    = HTTP_POST,
    .handler   = config_post_handler,
    .user_ctx  = NULL,
};

static const httpd_uri_t config_get_endpoint = {
    .uri       = "/config",
    .method    = HTTP_GET,
    .handler   = config_get_handler,
    .user_ctx  = NULL,
};

static const httpd_uri_t reboot = {
    .uri       = "/reboot",
    .method    = HTTP_POST,
    .handler   = reboot_post_handler,
    .user_ctx  = NULL,
};

static const httpd_uri_t stations_endpoint = {
    .uri       = "/stations",
    .method    = HTTP_GET,
    .handler   = stations_get_handler,
    .user_ctx  = NULL,
};

// HTTP Error (404) Handler - Redirects all requests to the root page
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGI(TAG, "Redirecting to root");
    return ESP_OK;
}

void http_server_start(void)
{
    server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root_handler);
        httpd_register_uri_handler(server, &index_handler);
        httpd_register_uri_handler(server, &scan);
        httpd_register_uri_handler(server, &connect);
        httpd_register_uri_handler(server, &config_endpoint);
        httpd_register_uri_handler(server, &config_get_endpoint);
        httpd_register_uri_handler(server, &reboot);
        httpd_register_uri_handler(server, &stations_endpoint);
        httpd_register_err_handler(server, HTTPD_404_NOT_FOUND, http_404_error_handler);
        return;
    }

    ESP_LOGI(TAG, "Error starting server!");
}
