#include "button.h"
#include "board_config.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

void button_init(void) {
    gpio_init(GUFIX_BUTTON_PIN);
    gpio_set_dir(GUFIX_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(GUFIX_BUTTON_PIN); // ใช้ Pull-up ภายใน (กดแล้วต่อลง GND)
}
