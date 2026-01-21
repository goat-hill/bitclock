#include "esp_err.h"

typedef uint8_t bitclock_nvs_temp_unit_val_t;
typedef uint8_t bitclock_nvs_clock_format_val_t;
typedef uint8_t bitclock_nvs_app_selection_val_t;

#define BITCLOCK_NVS_TEMP_UNIT_VAL_NONE 0
#define BITCLOCK_NVS_TEMP_UNIT_VAL_CELSIUS 1
#define BITCLOCK_NVS_TEMP_UNIT_VAL_FAHRENHEIT 2

#define BITCLOCK_NVS_CLOCK_FORMAT_VAL_NONE 0
#define BITCLOCK_NVS_CLOCK_FORMAT_VAL_12HR 1
#define BITCLOCK_NVS_CLOCK_FORMAT_VAL_24HR 2

#define BITCLOCK_NVS_APP_SELECTION_VAL_NONE 0
#define BITCLOCK_NVS_APP_SELECTION_VAL_CLOCK 1
#define BITCLOCK_NVS_APP_SELECTION_VAL_WEATHER 2
#define BITCLOCK_NVS_APP_SELECTION_VAL_AQI 3

esp_err_t bitclock_nvs_init();

const char *bitclock_nvs_get_tz();
esp_err_t bitclock_nvs_set_tz(const char *tz, size_t size);

const char *bitclock_nvs_get_weather_path();
esp_err_t bitclock_nvs_set_weather_path(const char *tz, size_t size);

bitclock_nvs_temp_unit_val_t bitclock_nvs_get_temp_unit();
esp_err_t bitclock_nvs_set_temp_unit(bitclock_nvs_temp_unit_val_t unit);

bitclock_nvs_clock_format_val_t bitclock_nvs_get_clock_format();
esp_err_t bitclock_nvs_set_clock_format(bitclock_nvs_clock_format_val_t format);

bitclock_nvs_app_selection_val_t bitclock_nvs_get_app_selection();
esp_err_t
bitclock_nvs_set_app_selection(bitclock_nvs_app_selection_val_t format);
