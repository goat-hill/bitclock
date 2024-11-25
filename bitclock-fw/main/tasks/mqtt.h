#include "freertos/FreeRTOS.h"

#include "tasks.h"
#include "libs/mqtt.h"

#ifdef MQTT_TASK_ENABLED

#define MQTT_EVENT_WIFI_CONNECTED BIT0
#define MQTT_EVENT_URL_CHANGED BIT1

// 4kB
#define MQTT_STACK_SIZE 1024 * 4

extern EventGroupHandle_t mqtt_event_group_handle;

extern StaticTask_t mqttTaskBuffer;
extern StackType_t mqttTaskStack[MQTT_STACK_SIZE];

void mqtt_task_run(void *pvParameters);

#endif
