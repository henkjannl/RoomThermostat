#pragma once

// All touch reads happen in loop context, never in a hardware interrupt.
int16_t keyboardEnabed = 0; // Retained spelling for existing diagnostics.
int32_t keyboardMicroSeconds = 0;
unsigned long keyboardMaxPollGapUs = 0;
void disableKeyboard() { ++keyboardEnabed; }
void enableKeyboard() { if (keyboardEnabed > 0) --keyboardEnabed; }

void checkKeyboard() {
  static unsigned long lastPollUs = 0;
  static bool previous[3] = {false, false, false};
  static unsigned long changedAt[3] = {0, 0, 0};
  static bool sent[3] = {false, false, false};
  const uint8_t pins[] = {PIN_BUT_UP, PIN_BUT_SELECT, PIN_BUT_DOWN};
  const command_t commands[] = {cmdKeyUp, cmdKeySelect, cmdKeyDown};
  unsigned long nowUs = micros();
  if (nowUs - lastPollUs < 10000 || keyboardEnabed) return;
  if (lastPollUs) keyboardMaxPollGapUs = max(keyboardMaxPollGapUs, nowUs - lastPollUs);
  lastPollUs = nowUs;
  unsigned long now = millis();
  for (int i = 0; i < 3; ++i) {
    bool pressed = touchRead(pins[i]) < KEY_THRESHOLD;
    if (pressed != previous[i]) { previous[i] = pressed; changedAt[i] = now; }
    if (now - changedAt[i] >= 30) {
      if (!pressed) sent[i] = false;
      else if (!sent[i]) {
        userEventMessage_t message(sndKeyboard, commands[i]);
        sent[i] = xQueueSend(menuQueue, &message, 0) == pdPASS;
      }
    }
  }
  keyboardMicroSeconds = max(keyboardMicroSeconds, (int32_t)(micros() - nowUs));
}
void startKeyboard() {}
