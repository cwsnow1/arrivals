#include "esp_http_client.h"
#include "esp_log.h"
#include "json_parser.h"

#include "api.h"
#include "private/http_client.h"

static const char* TAG = "api";

void api_get(void)
{
    jparse_ctx_t ctx;
    int err = 0;
    int num_routes = 0;
    http_response_t r = http_get("http://lapi.transitchicago.com/api/1.0/ttpositions.aspx?rt=blue&outputType=JSON&key=" CONFIG_API_KEY);

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
    ESP_LOGI(TAG, "%d routes", num_routes);
    for (int i = 0; i < num_routes; ++i) {
        int num_trains = 0;
        json_arr_get_object(&ctx, i);
        json_obj_get_array(&ctx, "train", &num_trains);
        for (int j = 0; j < num_trains; ++j) {
            char buffer[64];
            json_arr_get_object(&ctx, j);
            json_obj_get_string(&ctx, "arrT", buffer, sizeof(buffer));
            json_arr_leave_object(&ctx);
            ESP_LOGI(TAG, "Train %d arrival: %s", j, buffer);
        }
        json_obj_leave_array(&ctx);
        json_obj_leave_object(&ctx);
    }
cleanup:
    json_parse_end(&ctx);
err:
    free(r.buffer);
}