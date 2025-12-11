#include <time.h>

#include "esp_http_client.h"
#include "esp_log.h"
#include "json_parser.h"

#include "api.h"
#include "private/http_client.h"

static const char* TAG = "api";

#define API_ENDPOINT(line) "http://lapi.transitchicago.com/api/1.0/ttpositions.aspx?rt=" line "&outputType=JSON&key=" CONFIG_API_KEY

typedef enum {
    RED_LINE,
    BLUE_LINE,
    BROWN_LINE,

    LINE_COUNT
} __packed line_name_t;

typedef struct {
    uint32_t run;
    uint32_t next;
    uint32_t original_estimate;
} train_t;

typedef struct {
    train_t* trains;
    size_t count;
} line_t;

static line_t lines[LINE_COUNT];

int timestamp_subtract(const char* arrival_ts)
{
    int year, month, day, hour, minute, second;
    sscanf(arrival_ts, "%04d-%02d-%02dT%02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);

    struct tm t = {
        .tm_sec = second,
        .tm_min = minute,
        .tm_hour = hour,
        .tm_mday = day,
        .tm_mon = month - 1,
        .tm_year = year - 1900,
    };

    return mktime(&t) - time(NULL);
}

static void decode(http_response_t r)
{
    jparse_ctx_t ctx;
    int err = 0;
    int num_routes = 0;

    if (r.status != HttpStatus_Ok) {
        ESP_LOGE(TAG, "Bad status: %d\n", r.status);
        goto err;
    }
    if (json_parse_start(&ctx, (const char*) r.buffer, r.length) != OS_SUCCESS) {
        ESP_LOGE(TAG, "Error parsing response");
        goto err;
    }

    if (json_obj_get_object(&ctx, "ctatt") != OS_SUCCESS) {
        ESP_LOGE(TAG, "Error finding ctatt obj");
        goto cleanup;
    }

    json_obj_get_int(&ctx, "errCd", &err);
    if (err) {
        ESP_LOGE(TAG, "Error code received %d", err);
        goto cleanup;
    }

    json_obj_get_array(&ctx, "route", &num_routes);
    for (int i = 0; i < num_routes; ++i) {
        int num_trains = 0;
        json_arr_get_object(&ctx, i);
        json_obj_get_array(&ctx, "train", &num_trains);
        for (int j = 0; j < num_trains; ++j) {
            char buffer[64];
            json_arr_get_object(&ctx, j);

            json_obj_get_string(&ctx, "nextStaNm", buffer, sizeof buffer);
            printf("Train %-2d arrival at %-30s ", j, buffer);

            json_obj_get_string(&ctx, "arrT", buffer, sizeof buffer);
            printf("at %s, seconds remaining: %4d ", buffer, timestamp_subtract(buffer));

            json_obj_get_string(&ctx, "lat", buffer, sizeof buffer);
            printf("%8s ", buffer);

            json_obj_get_string(&ctx, "lon", buffer, sizeof buffer);
            printf("%8s ", buffer);

            json_obj_get_string(&ctx, "isApp", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                printf(" (APP)");
            }

            json_obj_get_string(&ctx, "isDly", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                printf(" (DELAY)");
            }
            printf("\n");

            json_arr_leave_object(&ctx);
        }
        json_obj_leave_array(&ctx);
        json_obj_leave_object(&ctx);
    }
cleanup:
    json_parse_end(&ctx);
err:
    free(r.buffer);
}

void api_get(void)
{
    ESP_LOGD(TAG, "Starting requests");
    http_response_t blue  = http_get_and_keep_open(API_ENDPOINT("blue"));
    http_response_t red   = http_get_and_keep_open(API_ENDPOINT("red"));
    http_response_t brown = http_get_and_keep_open(API_ENDPOINT("brn"));
    http_close();
    ESP_LOGD(TAG, "Done");
    decode(blue);
    decode(red);
    decode(brown);
}