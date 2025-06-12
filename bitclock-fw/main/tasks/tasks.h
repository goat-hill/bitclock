#include "freertos/FreeRTOS.h"

#include "pins.h"

//
// Task control
//
#ifdef PIN_NUM_BLINK
#define LED_TASK_ENABLED 2
#endif

#define DISP_TASK_ENABLED 1
#define SHT4X_TASK_ENABLED 1
#define SCD4X_TASK_ENABLED 1
#define SGP4X_TASK_ENABLED 1
#define WIFI_TASK_ENABLED 1
#define BLE_TASK_ENABLED 1
#define WEATHER_TASK_ENABLED 1
#define MQTT_TASK_ENABLED 1

//
// Task objects
//
#ifdef DISP_TASK_ENABLED
#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP
#define DISP_TASK_PRIORITY 20
extern TaskHandle_t dispTask;
#elif DISPLAY_TYPE == DISPLAY_TYPE_EINK
#define EINK_TASK_PRIORITY 20
extern TaskHandle_t einkTask;
#endif // DISPLAY_TYPE ==
#endif // DISP_TASK_ENABLED

#ifdef LED_TASK_ENABLED
#define LED_TASK_PRIORITY 1
extern TaskHandle_t ledTask;
#endif

#ifdef SHT4X_TASK_ENABLED
#define SHT4X_TASK_PRIORITY 10
extern TaskHandle_t sht4xTask;
#endif

#ifdef SCD4X_TASK_ENABLED
#define SCD4X_TASK_PRIORITY 10
extern TaskHandle_t scd4xTask;
#endif

#ifdef SGP4X_TASK_ENABLED
#define SGP4X_TASK_PRIORITY 10
extern TaskHandle_t sgp4xTask;
#endif

#ifdef WIFI_TASK_ENABLED
#define WIFI_TASK_PRIORITY 5
extern TaskHandle_t wifiTask;
#endif

#ifdef BLE_TASK_ENABLED
#define BLE_TASK_PRIORITY 15
extern TaskHandle_t bleTask;
#endif

#ifdef WEATHER_TASK_ENABLED
#define WEATHER_TASK_PRIORITY 10
extern TaskHandle_t weatherTask;
#endif

#ifdef MQTT_TASK_ENABLED
#define MQTT_TASK_PRIORITY 10
extern TaskHandle_t mqttTask;
#endif
