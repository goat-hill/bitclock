#include "nvs.h"

#include <nvs.h>
#include <string.h>

static const char *NVS_NAMESPACE =
    "canary"; // original project codename was canary
static const char *NVS_ID_TIMEZONE = "tz";
static const char *NVS_ID_TEMP_UNIT = "temp_unit";
static const char *NVS_ID_CLOCK_FORMAT = "clk_fmt";
static const char *NVS_ID_WEATHER_PATH = "wea_path";
static const char *NVS_ID_APP_SELECTION = "app";

static bitclock_nvs_temp_unit_val_t temp_unit = BITCLOCK_NVS_TEMP_UNIT_VAL_NONE;
static bitclock_nvs_clock_format_val_t clock_format =
    BITCLOCK_NVS_CLOCK_FORMAT_VAL_NONE;
static bitclock_nvs_app_selection_val_t app_selection =
    BITCLOCK_NVS_APP_SELECTION_VAL_NONE;

static const char *timezone_str = NULL;
static const char *weather_path_str = NULL;

esp_err_t bitclock_nvs_init() {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  // Timezone
  size_t required_size = 0;
  err = nvs_get_blob(handle, NVS_ID_TIMEZONE, NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    return err;
  }
  if (required_size > 0) {
    timezone_str = malloc(required_size);
    err = nvs_get_blob(handle, NVS_ID_TIMEZONE, (void *)timezone_str,
                       &required_size);
    if (err != ESP_OK) {
      free((void *)timezone_str);
      return err;
    }
  }

  // Weather path
  required_size = 0;
  err = nvs_get_blob(handle, NVS_ID_WEATHER_PATH, NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    return err;
  }
  if (required_size > 0) {
    weather_path_str = malloc(required_size);
    err = nvs_get_blob(handle, NVS_ID_WEATHER_PATH, (void *)weather_path_str,
                       &required_size);
    if (err != ESP_OK) {
      free((void *)weather_path_str);
      return err;
    }
  }

  // Temp unit
  err = nvs_get_u8(handle, NVS_ID_TEMP_UNIT, &temp_unit);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    return err;
  }

  // Clock format
  err = nvs_get_u8(handle, NVS_ID_CLOCK_FORMAT, &clock_format);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    return err;
  }

  // App selection
  err = nvs_get_u8(handle, NVS_ID_APP_SELECTION, &app_selection);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
    return err;
  }

  nvs_close(handle);

  return ESP_OK;
}

bitclock_nvs_temp_unit_val_t bitclock_nvs_get_temp_unit() { return temp_unit; }

esp_err_t
bitclock_nvs_set_temp_unit(bitclock_nvs_temp_unit_val_t new_temp_unit) {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_u8(handle, NVS_ID_TEMP_UNIT, new_temp_unit);
  if (err != ESP_OK) {
    return err;
  }
  temp_unit = new_temp_unit;

  nvs_close(handle);

  return ESP_OK;
}

const char *bitclock_nvs_get_tz() { return timezone_str; }

esp_err_t bitclock_nvs_set_tz(const char *tz, size_t size) {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_blob(handle, NVS_ID_TIMEZONE, tz, size);
  if (err != ESP_OK) {
    return err;
  }
  // FIXME: Free the old string? Malloc for the new one?
  timezone_str = tz;

  nvs_close(handle);

  return ESP_OK;
}

const char *bitclock_nvs_get_weather_path() { return weather_path_str; }

esp_err_t bitclock_nvs_set_weather_path(const char *weather_path, size_t size) {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_blob(handle, NVS_ID_WEATHER_PATH, weather_path, size);
  if (err != ESP_OK) {
    return err;
  }
  // FIXME: Free the old string? Malloc for the new one?
  weather_path_str = weather_path;

  nvs_close(handle);

  return ESP_OK;
}

bitclock_nvs_clock_format_val_t bitclock_nvs_get_clock_format() {
  return clock_format;
}

esp_err_t bitclock_nvs_set_clock_format(
    bitclock_nvs_clock_format_val_t new_clock_format) {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_u8(handle, NVS_ID_CLOCK_FORMAT, new_clock_format);
  if (err != ESP_OK) {
    return err;
  }
  clock_format = new_clock_format;

  nvs_close(handle);

  return ESP_OK;
}

bitclock_nvs_app_selection_val_t bitclock_nvs_get_app_selection() {
  return app_selection;
}

esp_err_t bitclock_nvs_set_app_selection(
    bitclock_nvs_app_selection_val_t new_app_selection) {
  nvs_handle_t handle;
  esp_err_t err;

  err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    return err;
  }

  err = nvs_set_u8(handle, NVS_ID_APP_SELECTION, new_app_selection);
  if (err != ESP_OK) {
    return err;
  }
  app_selection = new_app_selection;

  nvs_close(handle);

  return ESP_OK;
}
