#include "esp_err.h"

#include <stdbool.h>

esp_err_t bitclock_ota_on_boot();

esp_err_t bitclock_ota_start(size_t image_size);
esp_err_t bitclock_ota_write(const void *data, size_t size);
bool bitclock_ota_ready_to_update();
esp_err_t bitclock_ota_abort();
esp_err_t bitclock_ota_complete();
