#include "freertos/FreeRTOS.h"

#define SGP4X_STACK_SIZE 1024 * 3

extern StaticTask_t sgp4xTaskBuffer;
extern StackType_t sgp4xTaskStack[SGP4X_STACK_SIZE];

bool sgp4x_current_voc_available();
bool sgp41_current_nox_available();

uint16_t sgp4x_current_voc_raw();
uint16_t sgp41_current_nox_raw();

int32_t sgp4x_current_voc_index();
int32_t sgp41_current_nox_index();

void sgp4x_task_run(void *pvParameters);
