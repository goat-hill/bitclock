#include "freertos/FreeRTOS.h"

// 4kB
#define SCD4X_STACK_SIZE 1024 * 4

extern StaticTask_t scd4xTaskBuffer;
extern StackType_t scd4xTaskStack[SCD4X_STACK_SIZE];

bool scd4x_current_ppm_available();

uint16_t scd4x_current_co2_ppm();

void scd4x_task_run(void *pvParameters);
