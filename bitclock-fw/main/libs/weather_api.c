#include "weather_api.h"

#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "libs/http_buffer.h"
#include "libs/nvs.h"
#include <sys/param.h>

static const char *TAG = "weather_api";

typedef enum {
  WEATHER_PARSER_STATUS_LOOKING_FOR_WEATHER_1 = 0,
  WEATHER_PARSER_STATUS_LOOKING_FOR_WEATHER_2,
  WEATHER_PARSER_STATUS_DONE,
} weather_parser_status_t;
weather_parser_status_t parser_status = 0;
int parser_token_index = 0;

/*
  WEATHER_ICON_NONE
  WEATHER_ICON_CLEAR_NIGHT
  WEATHER_ICON_CLOUD
  WEATHER_ICON_FOGGY
  WEATHER_ICON_PARTLY_CLOUDY_DAY
  WEATHER_ICON_PARTLY_CLOUDY_NIGHT
  WEATHER_ICON_RAINY
  WEATHER_ICON_SNOWY
  WEATHER_ICON_SUNNY
  WEATHER_ICON_THUNDERSTORM
*/
typedef struct {
  const char *label;
  weather_icon_t icon_day;
  weather_icon_t icon_night;
} weather_icon_mapping_t;

// Icons: https://github.com/weather-gov/weather.gov/blob/main/docs/icons.md
static weather_icon_mapping_t icon_mapping[] = {
    {.label = "skc",
     .icon_day = WEATHER_ICON_SUNNY,
     .icon_night = WEATHER_ICON_CLEAR_NIGHT},
    {.label = "few",
     .icon_day = WEATHER_ICON_SUNNY,
     .icon_night = WEATHER_ICON_CLEAR_NIGHT},
    {.label = "sct",
     .icon_day = WEATHER_ICON_PARTLY_CLOUDY_DAY,
     .icon_night = WEATHER_ICON_PARTLY_CLOUDY_NIGHT},
    {.label = "bkn",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "ovc",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "wind_skc",
     .icon_day = WEATHER_ICON_SUNNY,
     .icon_night = WEATHER_ICON_CLEAR_NIGHT},
    {.label = "wind_few",
     .icon_day = WEATHER_ICON_SUNNY,
     .icon_night = WEATHER_ICON_CLEAR_NIGHT},
    {.label = "wind_sct",
     .icon_day = WEATHER_ICON_PARTLY_CLOUDY_DAY,
     .icon_night = WEATHER_ICON_PARTLY_CLOUDY_NIGHT},
    {.label = "wind_bkn",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "wind_ovc",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "snow",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "rain_snow",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "rain_sleet",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "snow_sleet",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "fzra",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "rain_fzra",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "snow_fzra",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "sleet",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "rain",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "rain_showers",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "rain_showers_hi",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "tsra",
     .icon_day = WEATHER_ICON_THUNDERSTORM,
     .icon_night = WEATHER_ICON_THUNDERSTORM},
    {.label = "tsra_sct",
     .icon_day = WEATHER_ICON_THUNDERSTORM,
     .icon_night = WEATHER_ICON_THUNDERSTORM},
    {.label = "tsra_hi",
     .icon_day = WEATHER_ICON_THUNDERSTORM,
     .icon_night = WEATHER_ICON_THUNDERSTORM},
    {.label = "tornado",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "hurricane",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "tropical_storm",
     .icon_day = WEATHER_ICON_RAINY,
     .icon_night = WEATHER_ICON_RAINY},
    {.label = "dust",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "smoke",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "haze",
     .icon_day = WEATHER_ICON_CLOUD,
     .icon_night = WEATHER_ICON_CLOUD},
    {.label = "hot",
     .icon_day = WEATHER_ICON_SUNNY,
     .icon_night = WEATHER_ICON_CLEAR_NIGHT},
    {.label = "cold",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "blizzard",
     .icon_day = WEATHER_ICON_SNOWY,
     .icon_night = WEATHER_ICON_SNOWY},
    {.label = "fog",
     .icon_day = WEATHER_ICON_FOGGY,
     .icon_night = WEATHER_ICON_FOGGY},
    {0}};
/*
{
  "number": 1,
  "name": "This Afternoon",
  "startTime": "2024-06-05T17:00:00-07:00",
  "endTime": "2024-06-05T18:00:00-07:00",
  "isDaytime": true,
  "temperature": 81,
  "temperatureUnit": "F",
  "temperatureTrend": "falling",
  "probabilityOfPrecipitation": {
      "unitCode": "wmoUnit:percent",
      "value": null
  },
  "dewpoint": {
      "unitCode": "wmoUnit:degC",
      "value": 15.555555555555555
  },
  "relativeHumidity": {
      "unitCode": "wmoUnit:percent",
      "value": 54
  },
  "windSpeed": "10 mph",
  "windDirection": "W",
  "icon": "https://api.weather.gov/icons/land/day/skc?size=medium",
  "shortForecast": "Sunny",
  "detailedForecast": "Sunny. High near 81, with temperatures falling to around
78 in the afternoon. West wind around 10 mph."
},
*/

// We want .properties.periods[0], [1]
// .temperature, .isDaytime
// If .isDaytime is false, use 1 (no high available for today)
// If .isDaytime is true, use 2

weather_icon_t icon_for_image_url(const char *icon_url, bool day_icon) {
  // Get position of filename in URL
  const char *filename = strrchr(icon_url, '/');

  if (filename == NULL) {
    ESP_LOGE(TAG, "No filename found in icon URL");
    return WEATHER_ICON_NONE;
  }

  // Skip the slash
  filename++;

  weather_icon_mapping_t *cur_icon = icon_mapping;
  for (; *((uint8_t *)cur_icon) != 0; cur_icon++) {
    // Log current comparison
    ESP_LOGD(TAG, "Comparing icon label: %s to filename: %s",
             cur_icon->label, filename);
    if (strncmp(cur_icon->label, filename, strlen(cur_icon->label)) == 0) {
      return day_icon ? cur_icon->icon_day : cur_icon->icon_night;
    }
  }

  ESP_LOGE(TAG, "No weather icon for: %s", filename);
  return WEATHER_ICON_NONE;
}

static esp_err_t weather_http_event_handler(esp_http_client_event_t *evt) {
  static int output_len;

  switch (evt->event_id) {
  case HTTP_EVENT_ERROR:
  case HTTP_EVENT_ON_CONNECTED:
  case HTTP_EVENT_HEADER_SENT:
  case HTTP_EVENT_ON_HEADER:
  case HTTP_EVENT_REDIRECT:
    break;
  case HTTP_EVENT_ON_DATA:
    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
    // Clean the buffer in case of a new request
    if (output_len == 0 && evt->user_data) {
      memset(evt->user_data, 0, MAX_HTTP_OUTPUT_BUFFER);
    }

    // Only handling non-chunked for now
    if (!esp_http_client_is_chunked_response(evt->client)) {
      // If user_data buffer is configured, copy the response into the buffer
      int copy_len = 0;
      if (evt->user_data) {
        copy_len = MIN(evt->data_len, (MAX_HTTP_OUTPUT_BUFFER - output_len));
        if (copy_len) {
          memcpy(evt->user_data + output_len, evt->data, copy_len);
        }
      }
      output_len += copy_len;
    }
    break;
  case HTTP_EVENT_ON_FINISH:
  case HTTP_EVENT_DISCONNECTED:
    output_len = 0;
    break;
  }
  return ESP_OK;
}

esp_err_t parse_weather_response(char *response_buffer, wifi_weather_t *weather,
                                 struct tm *timeinfo) {
  char current_date_str[11];
  strftime(current_date_str, sizeof(current_date_str), "%Y-%m-%d", timeinfo);

  cJSON *root = cJSON_Parse(response_buffer);
  if (root == NULL) {
    ESP_LOGE(TAG, "Error on JSON parse");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  cJSON *properties = cJSON_GetObjectItem(root, "properties");
  if (properties == NULL) {
    ESP_LOGE(TAG, "No properties found in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  cJSON *periods = cJSON_GetObjectItem(properties, "periods");
  if (periods == NULL) {
    ESP_LOGE(TAG, "No periods found in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  // Find first period for today
  // The api.weather.gov gridpoints API caches for 12+ hours and returns old
  // forecasts sometimes
  cJSON *period = NULL;
  int start_period_i = 0;
  int num_periods = cJSON_GetArraySize(periods);
  for (; start_period_i < num_periods; start_period_i++) {
    cJSON *cur_period = cJSON_GetArrayItem(periods, start_period_i);
    if (cur_period == NULL) {
      ESP_LOGE(TAG, "No period found at index %d", start_period_i);
      break;
    }

    cJSON *startTime = cJSON_GetObjectItem(cur_period, "startTime");
    if (startTime == NULL || startTime->valuestring == NULL) {
      ESP_LOGE(TAG, "No startTime found in response");
      continue;
    }

    // Compare date part of startTime with current date
    if (strncmp(startTime->valuestring, current_date_str, 10) == 0) {
      period = cur_period;
      break;
    }
  }

  if (period == NULL) {
    ESP_LOGE(TAG, "No period found for today in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  // If we only have one temperature, that's ok
  // If isDayTime is false, that's the last temperature to parse. Store it as
  // the min If isDaytime is true for the first value, store that as max and
  // store next (if available) as min Parse icon string and store appropriate
  // local image enum value

  cJSON *temperature = cJSON_GetObjectItem(period, "temperature");
  if (temperature == NULL) {
    ESP_LOGE(TAG, "No temperature found in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  cJSON *isDaytime = cJSON_GetObjectItem(period, "isDaytime");
  if (isDaytime == NULL) {
    ESP_LOGE(TAG, "No isDaytime found in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  cJSON *icon = cJSON_GetObjectItem(period, "icon");
  if (icon == NULL) {
    ESP_LOGE(TAG, "No icon found in response");
    cJSON_Delete(root);
    return ESP_FAIL;
  }

  weather->icon = icon_for_image_url(icon->valuestring, isDaytime->valueint);

  // Store temperature
  if (isDaytime->valueint) {
    weather->temp_max = temperature->valueint;
  } else {
    weather->temp_min = temperature->valueint;
  }

  // Get second temperature if available (the period immediately after the
  // first one found). Don't error if missing.
  if (isDaytime->valueint && (start_period_i + 1) < num_periods) {
    cJSON *period2 = cJSON_GetArrayItem(periods, start_period_i + 1);
    if (period2 != NULL) {
      cJSON *temperature2 = cJSON_GetObjectItem(period2, "temperature");
      if (temperature2 != NULL) {
        weather->temp_min = temperature2->valueint;
      }
    }
  }

  ESP_LOGI(TAG, "Max temp: %" PRId16 ", Min temp: %" PRId16, weather->temp_max,
           weather->temp_min);

  time(&weather->requested_at);

  cJSON_Delete(root);
  return ESP_OK;
}

esp_err_t refresh_daily_weather(wifi_weather_t *weather, const char *path,
                                struct tm *timeinfo) {
  // TODO: Add ?units=si for SI units?
  // https://www.weather.gov/documentation/services-web-api#/default/gridpoint_forecast
  esp_http_client_config_t config = {
      .host = "api.weather.gov",
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .path = path,
      .crt_bundle_attach = esp_crt_bundle_attach,
      .user_data = http_response_buffer,
      .event_handler = weather_http_event_handler,
      .timeout_ms = 10000,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_header(client, "User-agent",
                             "(bitclock.io, weather@bitclock.io)");
  esp_http_client_set_header(client, "Accept", "application/geo+json");

  esp_err_t err = esp_http_client_perform(client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    esp_http_client_cleanup(client);
    return err;
  }

  ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64,
           esp_http_client_get_status_code(client),
           esp_http_client_get_content_length(client));

  parse_weather_response(http_response_buffer, weather, timeinfo);

  esp_http_client_cleanup(client);
  return ESP_OK;
}
