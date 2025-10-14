#include "tasks.h"

#ifdef LED_TASK_ENABLED
TaskHandle_t ledTask;
#endif

#ifdef DISP_TASK_ENABLED
TaskHandle_t einkTask;
TaskHandle_t dispTask;
#endif

#ifdef SHT4X_TASK_ENABLED
TaskHandle_t sht4xTask;
#endif

#ifdef SCD4X_TASK_ENABLED
TaskHandle_t scd4xTask;
#endif

#ifdef SGP4X_TASK_ENABLED
TaskHandle_t sgp4xTask;
#endif

#ifdef WIFI_TASK_ENABLED
TaskHandle_t wifiTask;
#endif

#ifdef BLE_TASK_ENABLED
TaskHandle_t bleTask;
#endif

#ifdef WEATHER_TASK_ENABLED
TaskHandle_t weatherTask;
#endif

#ifdef MQTT_TASK_ENABLED
TaskHandle_t mqttTask;
#endif
