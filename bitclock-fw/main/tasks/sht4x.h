#include "freertos/FreeRTOS.h"

#define SHT4X_STACK_SIZE 1024 * 3

extern StaticTask_t sht4xTaskBuffer;
extern StackType_t sht4xTaskStack[SHT4X_STACK_SIZE];

bool sht4x_current_temp_available();
float sht4x_current_temp_celsius();
float sht4x_current_relative_humidity();

void sht4x_task_run(void *pvParameters);
