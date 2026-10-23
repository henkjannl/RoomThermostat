// ES3C28P battery monitor v0.1.0
#include "config.h"
#include <initializer_list>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_NeoPixel.h>
#include "remote_ui.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"


// LCD reset is connected to the board reset circuit, not a separate GPIO.
// The SPIClass overload takes DC before CS (unlike the default-SPI overload).
Adafruit_ILI9341 screen(&SPI, LCD_DC, LCD_CS, -1);
Adafruit_NeoPixel pixel(1, RGB_PIN, NEO_GRB + NEO_KHZ800);

// Only application-owned pins; never include flash/PSRAM or USB pins.
const int heldPins[] = {
  LCD_BL, AMP_DISABLE, TOUCH_RST, LCD_CS, LCD_DC,
  LCD_MOSI, LCD_SCK, RGB_PIN, 4, 5, 7, 8
};

bool writeReg(uint8_t address, uint8_t reg, uint8_t value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool readReg(uint8_t address, uint8_t reg, uint8_t &value) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(address, uint8_t(1)) != 1) return false;
  value = Wire.read();
  return true;
}

void outputLevel(int pin, int level) {
  digitalWrite(pin, level);
  pinMode(pin, OUTPUT);
}

void showMessage(const char *message) {
  screen.fillScreen(ILI9341_BLACK);
  screen.setCursor(12, 30);
  screen.setTextColor(ILI9341_WHITE);
  screen.setTextSize(2);
  screen.setTextWrap(true);
  screen.println(message);
}


#include <WiFi.h>
#include <remote_transport.h>
#include <ui_theme.h>
#include "esp_system.h"
#include <math.h>
float percentFromVoltage(float volts) {
  // Illustrative 1S Li-ion curve; replace with your cell's measured curve.
  // Zero means our practical 3.30 V endpoint, not the protection cutoff.
  const float voltage[] = {3.30f, 3.50f, 3.60f, 3.70f, 3.75f, 3.80f,
                           3.85f, 3.90f, 4.00f, 4.10f, 4.20f};
  const float percent[] = {0, 5, 10, 20, 35, 50, 60, 70, 80, 90, 100};
  if (volts <= voltage[0]) return 0;
  for (unsigned i = 1; i < sizeof(voltage) / sizeof(voltage[0]); ++i) {
    if (volts <= voltage[i]) {
      return percent[i - 1] + (volts - voltage[i - 1]) *
          (percent[i] - percent[i - 1]) / (voltage[i] - voltage[i - 1]);
    }
  }
  return 100;
}

uint16_t measureBatteryMillivolts() {
  pinMode(BATTERY_ADC, INPUT);
  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_ADC, ADC_11db);
  delay(30); // Divider has high resistance and a capacitor on the ADC node.
  for (int i = 0; i < 8; ++i) {
    analogReadMilliVolts(BATTERY_ADC);
    delay(2);
  }
  uint32_t sum = 0;
  for (int i = 0; i < 64; ++i) {
    sum += analogReadMilliVolts(BATTERY_ADC);
    delay(2);
  }
  float mv = (sum / 64.0f) * 2.0f * BATTERY_CAL;
  return uint16_t(constrain(mv, 0.0f, 65535.0f) + 0.5f);
}

void stopWithError(const char *message) {
  digitalWrite(LCD_BL, HIGH);
  showMessage(message);
  Serial.println(message);
  // Stay awake with the error visible rather than sleeping without wakeup.
  while (true) delay(1000);
}

void releaseSleepHolds() {
  // Configure retained outputs before releasing their holds to avoid glitches.
  outputLevel(LCD_BL, LOW);
  outputLevel(AMP_DISABLE, HIGH);
  outputLevel(TOUCH_RST, HIGH);
  gpio_deep_sleep_hold_dis();
  for (int pin : heldPins) gpio_hold_dis(gpio_num_t(pin));
  // EXT1 temporarily assigns its wake pin to the RTC IO subsystem.
  rtc_gpio_deinit(gpio_num_t(TOUCH_INT));
}

bool suspendCodec() {
  // ES8311 suspend register sequence used by Espressif's codec driver.
  // Audio remains off after wake; this demonstration never starts I2S.
  const uint8_t sequence[][2] = {
    {0x32, 0x00}, {0x17, 0x00}, {0x0E, 0xFF}, {0x12, 0x02},
    {0x14, 0x00}, {0x0D, 0xFA}, {0x15, 0x00}, {0x02, 0x10},
    {0x00, 0x00}, {0x00, 0x1F}, {0x01, 0x30}, {0x01, 0x00},
    {0x45, 0x00}, {0x0D, 0xFC}, {0x02, 0x00}
  };
  for (const auto &entry : sequence) {
    if (!writeReg(CODEC_ADDR, entry[0], entry[1])) return false;
  }
  return true;
}

void initializeCYD() {
  releaseSleepHolds();
  outputLevel(AMP_DISABLE, HIGH);
  outputLevel(LCD_BL, LOW);

  pixel.begin();
  pixel.clear();
  pixel.show();

  SPI.begin(LCD_SCK, -1, LCD_MOSI, LCD_CS);
  screen.begin(20000000);
  // This IPS panel needs inversion enabled for the intended RGB565 colours.
  // Reapply after every deep-sleep wake because begin() resets the controller.
  screen.invertDisplay(true);
  screen.setRotation(PORTRAIT_ROTATION);
  RemoteUi::wake(screen,FIRMWARE_VERSION);
  digitalWrite(LCD_BL,HIGH); // Feedback before touch reset, ADC, logging and radio.

  Wire.begin(TOUCH_SDA, TOUCH_SCL, 100000);
  Wire.setTimeOut(100);
  pinMode(TOUCH_INT, INPUT);  // External pull-up on the PCB.
  outputLevel(TOUCH_RST, LOW);
  delay(10);
  digitalWrite(TOUCH_RST, HIGH);
  delay(300);

  uint8_t status;
  if (!readReg(TOUCH_ADDR, 0x02, status)) {
    stopWithError("Touch controller not found");
  }
  // Keep active while preparing; interrupt mode generates touch IRQ pulses.
  if (!writeReg(TOUCH_ADDR, 0x86, 0x00) ||
      !writeReg(TOUCH_ADDR, 0xA4, 0x01)) {
    stopWithError("Touch setup failed");
  }
  if (!suspendCodec()) stopWithError("Codec shutdown failed");

  // Hold unused audio inputs low; codec-to-CPU data is an input.
  for (int pin : {4, 5, 7, 8}) outputLevel(pin, LOW);
  pinMode(6, INPUT);

  // No MicroSD card in this demonstration. Do not drive SD data lines low:
  // the board has pull-ups, which would cause unnecessary current.
  for (int pin : {38, 39, 40, 41, 47, 48}) pinMode(pin, INPUT);
}

bool waitForTouchRelease() {
  uint32_t started = millis();
  uint32_t quietSince = millis();
  while (millis() - started < 10000) {
    uint8_t touches;
    if (!readReg(TOUCH_ADDR, 0x02, touches)) return false;
    if ((touches & 0x0F) != 0 || digitalRead(TOUCH_INT) == LOW) {
      quietSince = millis();
    } else if (millis() - quietSince >= 200) {
      return true;
    }
    delay(10);
  }
  return false;
}

void holdOutput(int pin, int level) {
  outputLevel(pin, level);
  ESP_ERROR_CHECK(gpio_hold_en(gpio_num_t(pin)));
}

bool sleepUntilTap() {
  // Return without sleeping if touch is held or the bus is faulty.
  if (!waitForTouchRelease()) return false;
  if (RemoteTransport::ready.load()) RemoteTransport::end();
  WiFi.mode(WIFI_OFF);
  if (!suspendCodec()) stopWithError("Codec shutdown failed");
  digitalWrite(AMP_DISABLE, HIGH);
  pixel.clear();
  pixel.show();

  // Allow automatic monitor mode, retaining the factory timeout/scan rate.
  // Do not write 0x03 to POWER_MODE (0xA5): that would hibernate touch.
  // Touch firmware differs between panels: verify monitor-mode IRQ on yours.
  if (!writeReg(TOUCH_ADDR, 0x86, TOUCH_MONITOR ? 0x01 : 0x00)) {
    stopWithError("Touch monitor setup failed");
  }
  // No further I2C reads: some touch firmware closes I2C in monitor mode.
  Wire.end();

  digitalWrite(LCD_BL, LOW);
  screen.sendCommand(ILI9341_DISPOFF);
  screen.sendCommand(ILI9341_SLPIN);
  delay(120);
  SPI.end();

  holdOutput(LCD_BL, LOW);
  holdOutput(AMP_DISABLE, HIGH);
  holdOutput(TOUCH_RST, HIGH);  // Touch must remain out of reset.
  holdOutput(LCD_CS, HIGH);
  holdOutput(LCD_DC, LOW);
  holdOutput(LCD_MOSI, LOW);
  holdOutput(LCD_SCK, LOW);
  holdOutput(RGB_PIN, LOW);
  for (int pin : {4, 5, 7, 8}) holdOutput(pin, LOW);
  gpio_deep_sleep_hold_en();

  ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
  ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup(
      1ULL << TOUCH_INT, ESP_EXT1_WAKEUP_ANY_LOW));
  ESP_ERROR_CHECK(esp_sleep_pd_config(
      ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF));
  // Other domains stay on AUTO: ESP-IDF powers them down as permitted.
  Serial.println("Deep sleep: tap screen to wake");
  Serial.flush();
  esp_deep_sleep_start();  // Does not return; wake runs setup() again.
  return true;
}


#include "remote_app.h"
