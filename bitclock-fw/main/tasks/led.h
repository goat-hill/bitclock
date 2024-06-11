#include "freertos/FreeRTOS.h"

#include "pins.h"
#include "tasks.h"

#ifdef LED_TASK_ENABLED

// 4kB
#define LED_STACK_SIZE 1024 * 4

extern StaticTask_t ledTaskBuffer;
extern StackType_t ledTaskStack[LED_STACK_SIZE];

void led_task_run(void *pvParameters);

#endif
