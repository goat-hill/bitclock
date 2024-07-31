#include "sht4x.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "libs/i2c.h"
#include "libs/sensor_utils.h"
#include "pins.h"
#include "tasks/ble.h"
#include <inttypes.h>

static const char *TAG = "sht4x";

StaticTask_t sht4xTaskBuffer;
StackType_t sht4xTaskStack[SHT4X_STACK_SIZE];

static bool tempAvailable = false;
static float degC;
static float relativeHumidity;

#define I2C_DEVICE_ADDR 0x44
#define I2C_DEVICE_SPEED 100000 // 100K standard mode

static i2c_master_dev_handle_t device_handle;

// I2C commands
// All 6 byte response except reset, which is no response.

static const uint8_t kMeasureHighPrecision = 0xfd; // 8.5ms max
#define MEASURE_HIGH_PRECISION_DELAY pdMS_TO_TICKS(10)
// static const uint8_t kMeasureMediumPrecision = 0xf6; // 4.5ms max
// #define MEASURE_MEDIUM_PRECISION_DELAY pdMS_TO_TICKS(6)
// static const uint8_t kMeasureLowPrecision = 0xe0; // 1.6ms max
// #define MEASURE_LOW_PRECISION_DELAY pdMS_TO_TICKS(3)
static const uint8_t kReadSerialNum = 0x89;

// static const uint8_t kSoftReset = 0x94;

// static const uint8_t kActivateHeater_200mW_1000ms = 0x39;
// static const uint8_t kActivateHeater_200mW_100ms = 0x32;
// static const uint8_t kActivateHeater_110mW_1000ms = 0x2f;
// static const uint8_t kActivateHeater_110mW_100ms = 0x24;
// static const uint8_t kActivateHeater_20mW_1000ms = 0x1e;
// static const uint8_t kActivateHeater_20mW_100ms = 0x15;

static uint8_t sht4xResponse[6];

static void sht4x_init() {
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = I2C_DEVICE_ADDR,
        .scl_speed_hz = I2C_DEVICE_SPEED,
    };
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &device_handle));

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock on init.");
  }
}

esp_err_t sht4x_read_serial() {
  static esp_err_t ret;

  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ret = i2c_master_transmit(device_handle,
                              &kReadSerialNum, // write buffer
                              1,               // write size
                              2000             // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to sht4x: %d", ret);
    }

    vTaskDelay(1);

    ret = i2c_master_receive(device_handle,
                             sht4xResponse,         // read buffer
                             sizeof(sht4xResponse), // read size
                             2000                   // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read from sht4x: %d", ret);
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }

  ESP_LOGI(TAG, "serial: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", sht4xResponse[0],
           sht4xResponse[1], sht4xResponse[2], sht4xResponse[3],
           sht4xResponse[4], sht4xResponse[5]);
  if (sensirion_crc(sht4xResponse) == sht4xResponse[2] &&
      sensirion_crc(sht4xResponse + 3) == sht4xResponse[5]) {
    uint32_t serial_number = (sht4xResponse[0] << 24) +
                             (sht4xResponse[1] << 16) +
                             (sht4xResponse[3] << 8) + sht4xResponse[4];
    ESP_LOGI(TAG, "Serial: %" PRIu32, serial_number);
  } else {
    ret = ESP_ERR_INVALID_CRC;
    ESP_LOGE(TAG, "Invalid CRC for reading serial");
  }

  return ret;
}

esp_err_t sht4x_measure_high_precision() {
  static uint16_t t_ticks, rh_ticks;
  static float rh_pRH;
  static esp_err_t ret;

  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ret = i2c_master_transmit(device_handle,
                              &kMeasureHighPrecision, // write buffer
                              1,                      // write size
                              1000                    // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to sht4x: %d", ret);
    }

    vTaskDelay(MEASURE_HIGH_PRECISION_DELAY);

    ret = i2c_master_receive(device_handle,
                             sht4xResponse,         // read buffer
                             sizeof(sht4xResponse), // read size
                             1000                   // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read from sht4x: %d", ret);
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }

  if (sensirion_crc(sht4xResponse) == sht4xResponse[2]) {
    t_ticks = sht4xResponse[0] * 256 + sht4xResponse[1];
    degC = -45 + (175 * ((float)t_ticks) / 65535);

    // SHT4x is supposed to be "factory calibrated" but at least
    // in my testing it reads overly hot by 4 deg C.
    //
    // This could be from various factors:
    // - Self-heating via sensor / PCB
    // - Bad enclosure ventilation
    // - PCBA manufacturing errors
    // - Sensor not meeting specification
    //
    // So let's modify it in a linear fashion based on a reference thermometer
    // at 2 points.
    //
    // Linear calibration based on semi-scientific testing with the rev5
    // enclosure Observations:  29.06 deg C should be 25.61
    //                7.59 deg C should be 3.67
    degC = degC * 1.022 - 4.08;

    tempAvailable = true;
  } else {
    ret = ESP_ERR_INVALID_CRC;
    ESP_LOGE(TAG, "Invalid CRC for temp");
  }

  if (sensirion_crc(sht4xResponse + 3) == sht4xResponse[5]) {
    rh_ticks = sht4xResponse[3] * 256 + sht4xResponse[4];
    rh_pRH = -6 + (125 * ((float)rh_ticks) / 65535);
    // ESP_LOGI(TAG, "rel humidity ticks 0x%x 0x%x %" PRIu16 " %f",
    // sht4xResponse[3], sht4xResponse[4], rh_ticks, rh_pRH);
    if (rh_pRH > 100) {
      rh_pRH = 100;
    } else if (rh_pRH < 0) {
      rh_pRH = 0;
    }
    relativeHumidity = rh_pRH;
  } else {
    ret = ESP_ERR_INVALID_CRC;
    ESP_LOGE(TAG, "Invalid CRC for humidity");
  }

  if (ret == ESP_OK) {
    ESP_LOGD(TAG, "T: %f\t RH: %f", degC, relativeHumidity);
    ble_notify_temperature_update();
    ble_notify_humidity_update();
  }

  return ret;
}

bool sht4x_current_temp_available() { return tempAvailable; }

float sht4x_current_temp_celsius() { return degC; }

float sht4x_current_relative_humidity() { return relativeHumidity; }

void sht4x_task_run(void *pvParameters) {
  sht4x_init();

  // Wait at least one cycle (10ms) for device to power on.
  vTaskDelay(1);

  while (1) {
    sht4x_measure_high_precision();
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}
