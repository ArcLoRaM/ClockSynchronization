#include "gpio_control.h"
#include "driver/gpio.h"
#include "sdkconfig.h"  // Needed for CONFIG_ macros

void gpio_control_init(void) {
    gpio_reset_pin(CONFIG_GPIO_CONTROL_PIN);
    gpio_set_direction(CONFIG_GPIO_CONTROL_PIN, GPIO_MODE_OUTPUT);
}

void gpio_control_set(bool level) {
    gpio_set_level(CONFIG_GPIO_CONTROL_PIN, level);
}
