#pragma once

#include <stdbool.h>

// Initialize the GPIO pin (from menuconfig)
void gpio_control_init(void);

// Set the output state of the GPIO pin
void gpio_control_set(bool level);
