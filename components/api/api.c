#include <math.h>
#include <stdio.h>
#include <time.h>

#include "esp_log.h"
#include "json_parser.h"

#include "api.h"
#include "config.h"
#include "http_client.h"
#include "wifi.h"

static const char* TAG = "api";

#define API_ENDPOINT "http://lapi.transitchicago.com/api/1.0/ttpositions.aspx?rt=%s&outputType=JSON&key=%s"
#define STATION_ENDPOINT "http://lapi.transitchicago.com/api/1.0/ttarrivals.aspx?mapid=%d&max=3&outputType=JSON&key=%s"
#define DIST_THRESHOLD (0.002f)

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

static expected_train_t trains[3];
static char station_name_buffer[128];
static char* s_api_key = NULL;

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

line_t api_update_eta(line_name_t line, uint16_t time_step_ms)
{
    for (size_t i = 0; i < s_lines[line].count; ++i) {
        train_t* train = &s_lines[line].trains[i];
        if (train->progress == 1.0f) continue;
        if (train->eta_ms > time_step_ms) {
            train->eta_ms -= time_step_ms;
        } else {
            train->eta_ms = 0;
        }
        if (train->original_eta != 0) {
            float progress = 1.0f - ((float) train->eta_ms / (train->original_eta * 1000));
            if (progress > train->progress) {
                train->progress = progress;
            }
        } else {
            train->progress = 1.0f;
        }
    }
    return s_lines[line];
}

static void decode(http_response_t r, line_name_t line)
{
    jparse_ctx_t ctx;
    int err = 0;
    int num_routes = 0;

    if (r.status != 200) {
        ESP_LOGE(TAG, "Bad status: %d", r.status);
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
                memset(train, 0, sizeof *train);
            }
            train->rn = rn;

            json_obj_get_string(&ctx, "nextStaId", buffer, sizeof buffer);
            station_id_t station = (station_id_t) strtol(buffer, NULL, 10);

            json_obj_get_string(&ctx, "arrT", buffer, sizeof buffer);
            int eta = timestamp_subtract(buffer);
            if (eta < 0) {
                eta = 0;
            }

            json_obj_get_string(&ctx, "nextStpId", buffer, sizeof buffer);
            stop_id_t next_stop = strtoul(buffer, NULL, 10);
            if (next_stop != train->next_stop) {
                train->original_eta = eta;
                train->progress = 0.0f;
            } else if (train->original_eta < eta) {
                train->original_eta = eta;
            } else {
                if (train->original_eta != 0) {
                    float progress = 1.0f - ((float) eta / train->original_eta);
                    if (progress > train->progress) {
                        // don't move trains backwards even if the ETA is updated
                        train->progress = progress;
                    }
                } else {
                    train->progress = 1.0f;
                }
            }
            train->eta_ms = eta * 1000;
            train->next_stop = next_stop;

            json_obj_get_string(&ctx, "isApp", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                json_obj_get_string(&ctx, "lat", buffer, sizeof buffer);
                float lat = strtod(buffer, NULL);

                json_obj_get_string(&ctx, "lon", buffer, sizeof buffer);
                float lon = strtof(buffer, NULL);
                location_t loc = cta_get_station_location(station);

                if (loc.lat - lat < DIST_THRESHOLD &&
                    loc.lat - lat > -DIST_THRESHOLD &&
                    loc.lon - lon < DIST_THRESHOLD &&
                    loc.lon - lon > -DIST_THRESHOLD) {
                    train->progress = 1.0f;
                }
            }

            json_obj_get_string(&ctx, "isDly", buffer, sizeof buffer);
            if (!strcmp(buffer, "1")) {
                train->delayed = 1;
            } else {
                train->delayed = 0;
            }

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

static expected_trains_t decode_arrivals(http_response_t r)
{
    expected_trains_t ret = { .trains = trains, .station_name = station_name_buffer };
    jparse_ctx_t ctx;
    int err = 0;
    int num_trains = 0;

    if (r.status != 200) {
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

    json_obj_get_array(&ctx, "eta", &num_trains);
    ret.count = num_trains >= 3 ? 3 : num_trains;
    for (int i = 0; i < ret.count; ++i) {
        char buffer[128];
        json_arr_get_object(&ctx, i);

        json_obj_get_string(&ctx, "staNm", station_name_buffer, sizeof station_name_buffer);

        json_obj_get_string(&ctx, "rt", buffer, sizeof buffer);
        ret.trains[i].line = cta_get_line_from_name(buffer);

        json_obj_get_string(&ctx, "rn", buffer, sizeof buffer);
        ret.trains[i].rn = strtol(buffer, NULL, 10);

        json_obj_get_string(&ctx, "destNm", ret.trains[i].destination, sizeof ret.trains[i].destination);

        json_obj_get_string(&ctx, "arrT", buffer, sizeof buffer);
        ret.trains[i].eta = timestamp_subtract(buffer) / 60;
        if (ret.trains[i].eta > 60) {
            ret.trains[i].eta = -1;
        }
    
        json_obj_get_string(&ctx, "isApp", buffer, sizeof buffer);
        if (!strcmp(buffer, "1")) {
            ret.trains[i].eta = 0;
        }
        json_obj_get_string(&ctx, "isDly", buffer, sizeof buffer);
        if (!strcmp(buffer, "1")) {
            ret.trains[i].eta = -1;
        }
        json_arr_leave_object(&ctx);
    }

cleanup:
    json_parse_end(&ctx);
err:
    free(r.buffer);
    return ret;
}

line_t api_get(line_name_t line)
{
    if (!wifi_is_connected()) {
        ESP_LOGI(TAG, "WiFi not connected, skipping");
        return (line_t) { 0 };
    }
    if (!s_api_key) {
        s_api_key = config_get_string("api_key");
    }
    ESP_LOGD(TAG, "Starting request");
    char url[128];
    sprintf(url, API_ENDPOINT, line_names[line], s_api_key);
    decode(http_get(url), line);
    return s_lines[line];
}

expected_trains_t api_get_expected(station_id_t station)
{
    char url[128];
    if (!s_api_key) {
        s_api_key = config_get_string("api_key");
    }
    sprintf(url, STATION_ENDPOINT, station, s_api_key);
    http_response_t resp = http_get(url);
    return decode_arrivals(resp);
}
