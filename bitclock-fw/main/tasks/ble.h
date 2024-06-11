#include "freertos/FreeRTOS.h"

#define BLE_STACK_SIZE 1024 * 4

extern StaticTask_t bleTaskBuffer;
extern StackType_t bleTaskStack[BLE_STACK_SIZE];

#define PASSKEY_NOT_ACTIVE 1000000
uint32_t ble_active_passkey();

void ble_task_run(void *pvParameters);

// Enqueue events of updated values to subscribers
void ble_notify_temperature_update();
void ble_notify_humidity_update();
void ble_notify_co2_update();
void ble_notify_voc_index_update();
void ble_notify_nox_index_update();
void ble_notify_wifi_status_update();
