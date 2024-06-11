#include "pins.h"

#if DISPLAY_TYPE == DISPLAY_TYPE_EINK

#include "freertos/FreeRTOS.h"

#define EINK_STACK_SIZE 1024 * 8

extern StaticTask_t einkTaskBuffer;
extern StackType_t einkTaskStack[EINK_STACK_SIZE];

void eink_task_run(void *pvParameters);

#endif
