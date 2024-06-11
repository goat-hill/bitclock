#include "freertos/FreeRTOS.h"

#include "libs/weather_api.h"

#define WEATHER_STACK_SIZE 1024 * 4

#define WEATHER_EVENT_WIFI_CONNECTED BIT0
#define WEATHER_EVENT_LOCATION_CHANGED BIT1

extern EventGroupHandle_t weather_event_group_handle;

extern StaticTask_t weatherTaskBuffer;
extern StackType_t weatherTaskStack[WEATHER_STACK_SIZE];

extern wifi_weather_t latest_weather;

void weather_task_run(void *pvParameters);
