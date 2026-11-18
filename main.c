/*
 * Usbliter8 - Tenstar RP2350 USB-A Edition
 * Integrated with OLED 0.91" (GP12/GP13), Button (GP15), and Heartbeat LED (GP25)
 */
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "led.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"

#include "bus.h"
#include "exploit.h"
#include "usb.h"
#include "log.h"
#include "oled.h"
#include "button.h"

/* กำหนดขา On-board LED สำหรับกระพริบสถานะการทำงาน */
#ifndef LED_PIN
#define LED_PIN 25 
#endif

static absolute_time_t next_blink_time;
static bool led_state = false;

static void heartbeat_init(void) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    next_blink_time = make_timeout_time_ms(500); // กระพริบทุกๆ 500ms
}

static void heartbeat_task(void) {
    if (time_reached(next_blink_time)) {
        led_state = !led_state;
        gpio_put(LED_PIN, led_state);
        next_blink_time = make_timeout_time_ms(500);
    }
}

#if WITH_AUTO_MODE

#define WITH_AUTO_REBOOT            (0)
#define FAILURE_REBOOT_DELAY_SEC    (3)
#define SUCCESS_REBOOT_DELAY_SEC    (5)

#define CONNECTION_FAIL_SLEEP_MS    (500)

__attribute__((noreturn))
void fatal_failure(void) {
    led_set_state(LED_STATE_ERROR);

#if WITH_AUTO_REBOOT
    printf("\nfatal failure, rebooting in %d seconds\n", FAILURE_REBOOT_DELAY_SEC);
    sleep_ms(FAILURE_REBOOT_DELAY_SEC * 1000);
    watchdog_reboot(0, SRAM_END, 1);
    while (1) {}
#else
    printf("\nfatal failure, spinning forever\n");
    while (1) {
        sleep_ms(100);
        heartbeat_task();
    }
#endif
}

__attribute__((noreturn))
void do_auto(void) {
    while (true) {
        heartbeat_task();
        int ret = exploit_run();

        if (ret == -2) {
            sleep_ms(CONNECTION_FAIL_SLEEP_MS);
            usb_bus_reset_open_ep0();
            continue;
        }

        if (ret != 0) {
            fatal_failure();
        }

        break;
    }

#if WITH_AUTO_REBOOT
    printf("\nsuccess, rebooting in %d seconds\n", SUCCESS_REBOOT_DELAY_SEC);
    sleep_ms(SUCCESS_REBOOT_DELAY_SEC * 1000);
    watchdog_reboot(0, SRAM_END, 1);
    while (1) {}
#else
    printf("\nsuccess, spinning forever\n");
    while (1) {
        sleep_ms(100);
        heartbeat_task();
    }
#endif
}

#else

static void help(void) {
    printf("'e'\texploit\n");
    printf("'r'\tbus reset\n");
    printf("'p'\treboot\n");
    printf("'h'\thelp\n");
}

void do_shell(void) {
    char buf[2] = { 0 };

    printf("\nDEBUG build, starting command prompt\n\n");
    help();
    printf("\n");

    while (1) {
        printf("> ");
        char c = stdio_getchar();
        printf("%c\n", c);

        switch (c) {
            case 'r':
                printf("Resetting the bus...\n");
                usb_bus_reset_open_ep0();
                break;
            case 'e':
                printf("Running exploit...\n");
                usb_bus_reset_open_ep0();
                if (exploit_run() == -2) {
                    printf("failed to discover a device\n");
                }
                break;
            case 'p':
                printf("Rebooting the board...\n");
                sleep_ms(100);
                watchdog_reboot(0, SRAM_END, 1);
                while (1) {}
            case 'h':
                help();
                break;
            default:
                printf("Unknown command '%s'\n", buf);
                break;
        }
    }
}

#endif

int main(void) {
#if PICO_RP2350
    set_sys_clock_khz(156000, true);
#elif PICO_RP2040
    set_sys_clock_khz(120000, true);
#else
#error What is this MCU even?
#endif

    led_init();
    led_set_state(LED_STATE_BOOTING);
    heartbeat_init();

    stdio_init_all();
    sleep_ms(2000);

    /* เริ่มต้นใช้งานจอ OLED ที่ขา GP12 (SDA) และ GP13 (SCL) */
    if (oled_init()) {
        printf("OLED initialized successfully at 0x%02X (SDA=GP12, SCL=GP13)\n", oled_address());
    } else {
        printf("OLED initialization failed (Check wiring: SDA=GP12, SCL=GP13)\n");
    }

    button_init();

    led_set_state(LED_STATE_IDLE);

    printf("\n============ %s v%s ============\n", PICO_PROGRAM_NAME, PICO_PROGRAM_VERSION_STRING);
    printf("built for %s, PIO USB @ GP%d/%d (D+/D-)\n\n", BOARD_NAME, PIO_USB_DP_PIN_DEFAULT, PIO_USB_DP_PIN_DEFAULT + 1);

    usb_start();
    usb_bus_init();
    usb_bus_wait_for_device();
    usb_bus_reset_open_ep0();

#if WITH_AUTO_MODE
    do_auto();
#else
    do_shell();
#endif
}
