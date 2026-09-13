include(waveshare_rp2350_base.cmake)

add_compile_definitions(
    BOARD_NAME="Tenstar RP2350 USB-A"
    PIO_USB_DP_PIN_DEFAULT=9
)