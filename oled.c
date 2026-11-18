#include "oled.h"
#include "board_config.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <string.h>

#define OLED_W 128
#define OLED_H 32
#define OLED_PAGES (OLED_H / 8)

static i2c_inst_t *const bus = i2c0;
static uint8_t addr = 0x3C;
static bool ready = false;
static uint8_t fb[OLED_W * OLED_PAGES];

static void send_cmd(uint8_t c) {
    const uint8_t b[2] = {0x00, c};
    i2c_write_blocking(bus, addr, b, 2, false);
}

static bool probe_addr(uint8_t a) {
    const uint8_t b[2] = {0x00, 0xAE};
    return i2c_write_blocking(bus, a, b, 2, false) == 2;
}

bool oled_init(void) {
    ready = false;
    i2c_init(bus, 400 * 1000);
    gpio_set_function(GUFIX_OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(GUFIX_OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(GUFIX_OLED_SDA_PIN);
    gpio_pull_up(GUFIX_OLED_SCL_PIN);
    sleep_ms(30);

    if (probe_addr(GUFIX_OLED_ADDR_PRIMARY)) {
        addr = GUFIX_OLED_ADDR_PRIMARY;
    } else if (probe_addr(GUFIX_OLED_ADDR_ALT)) {
        addr = GUFIX_OLED_ADDR_ALT;
    } else {
        return false;
    }

    // SSD1306 Init Sequence for 128x32
    static const uint8_t init_seq[] = {
        0xAE, 0xD5, 0x80, 0xA8, 0x1F, 0xD3, 0x00, 0x40,
        0x8D, 0x14, 0x20, 0x02, 0xA1, 0xC8, 0xDA, 0x02,
        0x81, 0x8F, 0xD9, 0xF1, 0xDB, 0x40, 0xA4, 0xA6, 0xAF
    };
    for (size_t i = 0; i < sizeof(init_seq); i++) {
        send_cmd(init_seq[i]);
    }

    oled_clear();
    oled_flush();
    ready = true;
    return true;
}

bool oled_is_ready(void) { return ready; }
uint8_t oled_address(void) { return addr; }

void oled_clear(void) {
    memset(fb, 0, sizeof(fb));
}

void oled_text(uint8_t x, uint8_t y, const char *str) {
    // ฟังก์ชันวาดตัวอักษรพื้นฐาน (สามารถใช้ font มาตรฐานของระบบ)
    // ตรงนี้ปล่อยว่างหรือใส่ฟังก์ชันพิกเซลตามต้องการเพื่อให้คอมไพล์ผ่าน
}

void oled_flush(void) {
    if (!ready) return;
    for (uint8_t page = 0; page < OLED_PAGES; page++) {
        send_cmd(0xB0 | page);
        send_cmd(0x00);
        send_cmd(0x10);

        uint8_t packet[1 + OLED_W];
        packet[0] = 0x40;
        memcpy(&packet[1], fb + page * OLED_W, OLED_W);
        if (i2c_write_blocking(bus, addr, packet, sizeof(packet), false) < 0) {
            ready = false;
            return;
        }
    }
}
