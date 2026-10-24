#pragma once
#include <Arduino.h>

#ifndef CYD_BATTERY_ONLY
#define CYD_BATTERY_ONLY 0
#endif
#ifndef CYD_TOUCH_MONITOR
#define CYD_TOUCH_MONITOR 1
#endif

constexpr int LCD_CS = 10;
constexpr int LCD_DC = 46;
constexpr int LCD_MOSI = 11;
constexpr int LCD_SCK = 12;
constexpr int LCD_BL = 45;
constexpr int TOUCH_SDA = 16;
constexpr int TOUCH_SCL = 15;
constexpr int TOUCH_RST = 18;
constexpr int TOUCH_INT = 17;
constexpr int AMP_DISABLE = 1;  // HIGH disables the amplifier.
constexpr int RGB_PIN = 42;
constexpr uint8_t TOUCH_ADDR = 0x38;
constexpr uint8_t CODEC_ADDR = 0x18;
constexpr uint32_t MESSAGE_MS = 15000;
constexpr int BATTERY_ADC = 9;  // ADC1, 200k/200k divider: Vbattery = 2 * Vadc.
constexpr float BATTERY_CAL = 1.0f; // Set to multimeter_voltage / displayed_voltage.
constexpr bool BATTERY_ONLY = CYD_BATTERY_ONLY != 0; // Set false when USB/external power is connected.

constexpr bool TOUCH_MONITOR = CYD_TOUCH_MONITOR != 0;
constexpr const char *FIRMWARE_VERSION = "3.0.0-alpha.6";

// Portrait, USB at bottom: confirm four-corner mapping on the physical panel.
constexpr uint8_t PORTRAIT_ROTATION=0;
constexpr bool TOUCH_SWAP_XY=false, TOUCH_MIRROR_X=false, TOUCH_MIRROR_Y=false;
