#ifndef OLED_H
#define OLED_H

#include <stdbool.h>
#include <stdint.h>

#define OLED_COLS 128

bool oled_init(void);
bool oled_is_ready(void);
uint8_t oled_address(void);
void oled_clear(void);
void oled_flush(void);
void oled_text(uint8_t x, uint8_t y, const char *str);

#endif
