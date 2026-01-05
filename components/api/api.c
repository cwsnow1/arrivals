#include <time.h>

#include "esp_http_client.h"
#include "esp_log.h"
#include "json_parser.h"

#include "api.h"
#include "private/http_client.h"

static const char* TAG = "api";

#define API_ENDPOINT "http://lapi.transitchicago.com/api/1.0/ttpositions.aspx?rt=%s&outputType=JSON&key=" CONFIG_API_KEY

static const char* line_names[] = {
    [RED_LINE] = "red",
    [BLUE_LINE] = "blue",
    [GREEN_LINE] = "g",
    [BROWN_LINE] = "brn",
    [PURPLE_LINE] = "p",
    [YELLOW_LINE] = "y",
    [PINK_LINE] = "pink",
    [ORANGE_LINE] = "org",
};

static line_t s_lines[LINE_COUNT];

static int timestamp_subtract(const char* arrival_ts)
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

static void decode(http_response_t r, line_name_t line)
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

            json_obj_get_string(&ctx, "rn", buffer, sizeof buffer);
            int rn = strtol(buffer, NULL, 10);

            train_t* train = NULL;
            for (size_t i = 0; i < s_lines[line].count; ++i) {
                if (s_lines[line].trains[i].rn == rn) {
                    train = &s_lines[line].trains[i];
                    break;
                }
            }
            if (!train) {
                s_lines[line].count++;
                s_lines[line].trains = realloc(s_lines[line].trains, sizeof(*s_lines[line].trains) * s_lines[line].count);
                train = &s_lines[line].trains[s_lines[line].count - 1];
            }
            train->rn = rn;

            json_obj_get_string(&ctx, "nextStpId", buffer, sizeof buffer);
            train->next_stop = strtoul(buffer, NULL, 10);

            json_obj_get_string(&ctx, "nextStaNm", buffer, sizeof buffer);
            //printf("Run %d arrival at %-30s ", rn, buffer);

            json_obj_get_string(&ctx, "arrT", buffer, sizeof buffer);
            //printf("at %s, seconds remaining: %4d ", buffer, timestamp_subtract(buffer));

            json_obj_get_string(&ctx, "lat", buffer, sizeof buffer);
            //printf("%8s ", buffer);

            json_obj_get_string(&ctx, "lon", buffer, sizeof buffer);
            //printf("%8s ", buffer);

            json_obj_get_string(&ctx, "isApp", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                train->progress = 1.0f;
            } else {
                train->progress = 0.0f;
            }

            json_obj_get_string(&ctx, "isDly", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                //printf(" (DELAY)");
            }
            //printf("\n");

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

line_t* api_get(void)
{
    ESP_LOGD(TAG, "Starting requests");
    http_response_t responses[LINE_COUNT];
    char url[128];
    for (size_t i = 0; i < LINE_COUNT; ++i) {
        sprintf(url, API_ENDPOINT, line_names[i]);
        responses[i] = http_get_and_keep_open(url);
    }
    http_close();
    ESP_LOGD(TAG, "Done");
    for (size_t i = 0; i < LINE_COUNT; ++i) {
        decode(responses[i], i);
    }
    return s_lines;
}