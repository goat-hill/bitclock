#include "scd4x.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "libs/i2c.h"
#include "libs/sensor_utils.h"
#include "pins.h"
#include "tasks/ble.h"

static const char *TAG = "scd4x";

StaticTask_t scd4xTaskBuffer;
StackType_t scd4xTaskStack[SCD4X_STACK_SIZE];

static i2c_master_dev_handle_t device_handle;

static bool ppm_available = false;
static uint16_t ppm;

#define I2C_DEVICE_ADDR 0x62
#define I2C_DEVICE_SPEED 100000 // 100K standard mode

static const uint8_t kStartPeriodicMeasurement[] = {0x21,
                                                    0xb1}; // No execution time
static const uint8_t kReadMeasurement[] = {0xec, 0x05};    // 1ms exceution time
static const uint8_t kStopPeriodicMeasurement[] = {
    0x3f, 0x86}; // 500ms execution time

static uint8_t kReadResponse[9];

static void scd4x_init() {
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = I2C_DEVICE_ADDR,
        .scl_speed_hz = I2C_DEVICE_SPEED,
        .flags.disable_ack_check =
            true, // not receiving ACK from scd4x_start_periodic_measurement()
    };
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &device_handle));

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c for init.");
  }
}

static void scd4x_start_periodic_measurement() {
  static esp_err_t ret;
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ESP_LOGI(TAG, "Starting periodic measurement...");
    ret = i2c_master_transmit(device_handle,
                              kStartPeriodicMeasurement,         // write buffer
                              sizeof(kStartPeriodicMeasurement), // write size
                              1000 // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to scd4x: %d", ret);
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }
}

static void scd4x_stop_periodic_measurement() {
  static esp_err_t ret;
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ESP_LOGI(TAG, "Stopping periodic measurement...");
    ret = i2c_master_transmit(device_handle,
                              kStopPeriodicMeasurement,         // write buffer
                              sizeof(kStopPeriodicMeasurement), // write size
                              1000 // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to scd4x: %d", ret);
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }
}

esp_err_t scd4x_read() {
  static esp_err_t ret;
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ret = i2c_master_transmit(device_handle,
                              kReadMeasurement,         // write buffer
                              sizeof(kReadMeasurement), // write size
                              1000                      // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to scd4x: %d", ret);
    }

    vTaskDelay(1);

    ret = i2c_master_receive(device_handle,
                             kReadResponse,         // read buffer
                             sizeof(kReadResponse), // read size
                             1000                   // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read from sht4x: %d", ret);
    }

    if (sensirion_crc(kReadResponse) == kReadResponse[2]) {
      ppm_available = true;
      ppm = 256 * kReadResponse[0] + kReadResponse[1];
      ESP_LOGD(TAG, "c02 ppm: %u", ppm);
      ble_notify_co2_update();
    } else {
      ret = ESP_ERR_INVALID_CRC;
      ESP_LOGE(TAG, "Invalid CRC");
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }

  return ret;
}

bool scd4x_current_ppm_available() { return ppm_available; }

uint16_t scd4x_current_co2_ppm() { return ppm; }

void scd4x_task_run(void *pvParameters) {
  scd4x_init();

  // Wait at least 30ms for device to power on.
  vTaskDelay(pdMS_TO_TICKS(100));

  scd4x_start_periodic_measurement();

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    scd4x_read();
  }
}
