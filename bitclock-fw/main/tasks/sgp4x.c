#include "sgp4x.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "libs/i2c.h"
#include "libs/sensirion_gas_index_algorithm.h"
#include "libs/sensor_utils.h"
#include "pins.h"
#include "sht4x.h"
#include "tasks/ble.h"
#include <inttypes.h>

static const char *TAG = "sgp4x";

StaticTask_t sgp4xTaskBuffer;
StackType_t sgp4xTaskStack[SGP4X_STACK_SIZE];

#define I2C_DEVICE_ADDR 0x59
#define I2C_DEVICE_SPEED 100000 // 100K standard mode

uint16_t DEFAULT_COMPENSATION_RH = 0x8000; // in ticks as defined by SGP41
uint16_t DEFAULT_COMPENSATION_T = 0x6666;  // in ticks as defined by SGP41

static i2c_master_dev_handle_t device_handle;

static const uint8_t kCommandExecuteConditioning[] = {
    0x26, 0x12, 0x80, 0x00, 0xa2, 0x66, 0x66, 0x93}; // Wait 50ms

static uint8_t sgp4xResponse[6];

static bool voc_available = false;
static bool nox_available = false;
static uint16_t sraw_voc = 0;
static uint16_t sraw_nox = 0;
static int32_t voc_index_value = 0;
static int32_t nox_index_value = 0;

static void sgp4x_init() {
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

esp_err_t sgp41_execute_conditioning() {
  static esp_err_t ret = ESP_OK;
  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ret = i2c_master_transmit(device_handle,
                              kCommandExecuteConditioning, // write buffer
                              sizeof(kCommandExecuteConditioning), // write size
                              1000 // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to sgp4x: %d", ret);
    }
    // FIXME: Check for ACK

    vTaskDelay(pdMS_TO_TICKS(50));

    ret = i2c_master_receive(device_handle,
                             sgp4xResponse, // read buffer
                             3,             // read size
                             1000           // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read raw signal: %d", ret);
    }

    if (sensirion_crc(sgp4xResponse) == sgp4xResponse[2]) {
      voc_available = true;
      sraw_voc = sgp4xResponse[0] * 256 + sgp4xResponse[1];
    } else {
      ESP_LOGE(TAG, "CRC check failed for sgp41_execute_conditioning()");
      ret = ESP_ERR_INVALID_CRC;
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }

  return ret;
}

esp_err_t sgp4x_measure_raw_signals(uint16_t compensation_rh,
                                    uint16_t compensation_t) {
  static uint8_t measure_raw_signal_cmd[8] = {
      0x26, 0x19,
      0x00, // rh
      0x00, // rh
      0x00, // rh crc
      0x00, // t
      0x00, // t
      0x00, // t crc
  };
  static esp_err_t ret;

  measure_raw_signal_cmd[2] = compensation_rh / 256;
  measure_raw_signal_cmd[3] = compensation_rh % 256;
  measure_raw_signal_cmd[4] = sensirion_crc(measure_raw_signal_cmd + 2);
  measure_raw_signal_cmd[5] = compensation_t / 256;
  measure_raw_signal_cmd[6] = compensation_t % 256;
  measure_raw_signal_cmd[7] = sensirion_crc(measure_raw_signal_cmd + 5);

  if (xSemaphoreTake(i2c_semaphore, pdMS_TO_TICKS(1000))) {
    ret = i2c_master_transmit(device_handle,
                              measure_raw_signal_cmd,         // write buffer
                              sizeof(measure_raw_signal_cmd), // write size
                              1000                            // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to transmit to sgp4x: %d", ret);
    }
    // FIXME: Check for ACK

    // Wait 50ms for reading
    vTaskDelay(pdMS_TO_TICKS(50));

    ret = i2c_master_receive(device_handle,
                             sgp4xResponse,         // read buffer
                             sizeof(sgp4xResponse), // read size
                             1000                   // xfer_timeout_ms
    );
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to read raw signal: %d", ret);
    }
    if (sensirion_crc(sgp4xResponse) == sgp4xResponse[2]) {
      voc_available = true;
      sraw_voc = sgp4xResponse[0] * 256 + sgp4xResponse[1];
    } else {
      ESP_LOGE(TAG, "CRC check failed for sgp4x_measure_raw_signals() voc");
      ret = ESP_ERR_INVALID_CRC;
    }
    if (sensirion_crc(sgp4xResponse + 3) == sgp4xResponse[5]) {
      nox_available = true;
      sraw_nox = sgp4xResponse[3] * 256 + sgp4xResponse[4];
    } else {
      ESP_LOGE(TAG, "CRC check failed for sgp4x_measure_raw_signals() nox");
      ret = ESP_ERR_INVALID_CRC;
    }

    xSemaphoreGive(i2c_semaphore);
  } else {
    ESP_LOGE(TAG, "Failed to grab i2c lock.");
  }

  return ret;
}

bool sgp4x_current_voc_available() { return voc_available; }
bool sgp41_current_nox_available() { return nox_available; }

uint16_t sgp4x_current_voc_raw() { return sraw_voc; }
uint16_t sgp41_current_nox_raw() { return sraw_nox; }

int32_t sgp4x_current_voc_index() { return voc_index_value; }
int32_t sgp41_current_nox_index() { return nox_index_value; }

void sgp4x_task_run(void *pvParameters) {
  sgp4x_init();

  esp_err_t error = 0;
  uint16_t nox_conditioning_s = 10;
  uint16_t compensation_rh = DEFAULT_COMPENSATION_RH;
  uint16_t compensation_t = DEFAULT_COMPENSATION_T;
  GasIndexAlgorithmParams voc_params;
  GasIndexAlgorithm_init(&voc_params, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
  GasIndexAlgorithmParams nox_params;
  GasIndexAlgorithm_init(&nox_params, GasIndexAlgorithm_ALGORITHM_TYPE_NOX);

  // Wait at least one cycle (10ms) for device to power on.
  vTaskDelay(1);

  while (1) {
    // 1. Algorithm expects measurement every 1 sec
    vTaskDelay(pdMS_TO_TICKS(1000));

    // 2. Pull SHT4x measurements and convert to SGP41 ticks
    if (sht4x_current_temp_available()) {
      float s_temperature = sht4x_current_temp_celsius();
      float s_rh = sht4x_current_relative_humidity();
      compensation_t = (uint16_t)(s_temperature + 45) * 65535 / 175;
      compensation_rh = (uint16_t)s_rh * 65535 / 100;
    }

    // 3. Measure SGP4x signals
    if (nox_conditioning_s > 0) {
      error = sgp41_execute_conditioning();
      nox_conditioning_s--;
    } else {
      error = sgp4x_measure_raw_signals(compensation_rh, compensation_t);
    }

    // 4. Process raw signals by Gas Index Algorithm to get VOC and NOx index
    // values
    if (!error) {
      if (voc_available) {
        GasIndexAlgorithm_process(&voc_params, sraw_voc, &voc_index_value);
        ESP_LOGD(TAG, "VOC Raw: %" PRIu16 "\tVOC Index: %" PRId32, sraw_voc,
                 voc_index_value);
        ble_notify_voc_index_update();
      }
      if (nox_available) {
        GasIndexAlgorithm_process(&nox_params, sraw_nox, &nox_index_value);
        ESP_LOGD(TAG, "NOx Raw: %" PRIu16 "\tNOx Index: %" PRId32, sraw_nox,
                 nox_index_value);
        ble_notify_nox_index_update();
      }
    }
  }
}
