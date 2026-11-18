#ifndef GUFIX_BOARD_CONFIG_H
#define GUFIX_BOARD_CONFIG_H

#if !defined(GUFIX_USB_HOST_PIO)
#  define GUFIX_USB_HOST_PIO 1
#endif

#ifndef BOARD_TUH_RHPORT
#  define BOARD_TUH_RHPORT 0
#endif

#ifndef GUFIX_PIO_USB_DP_PIN
#  define GUFIX_PIO_USB_DP_PIN 2
#endif

/* กำหนดพิน I2C สำหรับ OLED 0.91" (ใช้ GP12 และ GP13 แทน GP17 ที่ไม่มีรูพิน) */
#ifndef GUFIX_OLED_SDA_PIN
#  define GUFIX_OLED_SDA_PIN  12
#endif
#ifndef GUFIX_OLED_SCL_PIN
#  define GUFIX_OLED_SCL_PIN  13
#endif

#define GUFIX_OLED_ADDR_PRIMARY 0x3C
#define GUFIX_OLED_ADDR_ALT     0x3D

/* ปุ่มกด (Pushbutton) ต่อเข้ากับ GP15 และลง GND */
#ifndef GUFIX_BUTTON_PIN
#  define GUFIX_BUTTON_PIN 15
#endif

#ifndef GUFIX_OLED_RETRY_MS
#  define GUFIX_OLED_RETRY_MS 2000
#endif

#ifndef GUFIX_WATCHDOG_MS
#  define GUFIX_WATCHDOG_MS 5000
#endif

#endif
