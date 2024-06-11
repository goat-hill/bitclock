#include "driver/i2c_master.h"

#include "freertos/freertos.h"

void i2c_bus_init();

extern i2c_master_bus_handle_t i2c_bus_handle;

// Take this before doing any i2c operation
extern SemaphoreHandle_t i2c_semaphore;
