#include "freertos/FreeRTOS.h"
#include "pins.h"

#if DISPLAY_TYPE == DISPLAY_TYPE_SHARP

#define DISP_STACK_SIZE 1024 * 8

extern StaticTask_t dispTaskBuffer;
extern StackType_t dispTaskStack[DISP_STACK_SIZE];

void disp_tick_hook();
void disp_task_run(void *pvParameters);

#endif
