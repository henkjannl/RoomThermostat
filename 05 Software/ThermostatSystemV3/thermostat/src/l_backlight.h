#pragma once

const uint8_t BACKLIGHT_BRIGHTNESS[] = {0,1,2,3,4,5,6,7,9,11,13,15,18,22,26,31,37,45,53,63,75,90,107,127,151,180,214,255};
uint8_t backlightIndex = 21; // Initial brightness 64.
void checkBacklight() {
  static unsigned long lastOn = millis();
  static unsigned long lastStep = 0;
  userEventMessage_t message;
  while (xQueueReceive(backlightQueue, &message, 0) == pdPASS) {
    if (message.command == cmdBacklightOn) {
      lastOn = millis();
      controllerData.backLightOn = true;
    }
  }
  unsigned long now = millis();
  if (controllerData.backLightOn && now - lastOn > BACKLIGHT_TIMEOUT) {
    controllerData.backLightOn = false;
    sendMessage(sndBacklight, cmdEnableTelegram, telegramQueue);
  }
  const uint8_t last = sizeof(BACKLIGHT_BRIGHTNESS) - 1;
  if (now - lastStep >= (controllerData.backLightOn ? 10UL : 40UL)) {
    lastStep = now;
    if (controllerData.backLightOn && backlightIndex < last) ++backlightIndex;
    else if (!controllerData.backLightOn && backlightIndex > 0) --backlightIndex;
    ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
  }
}
void startBacklight() {
  ledcSetup(BACKLIGHT_LED_CHANNEL, BACKLIGHT_FREQUENCY, BACKLIGHT_RESOLUTION);
  ledcAttachPin(PIN_BACKLIGHT, BACKLIGHT_LED_CHANNEL);
  ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
  sendMessage(sndBacklight, cmdBacklightOn, backlightQueue);
}
