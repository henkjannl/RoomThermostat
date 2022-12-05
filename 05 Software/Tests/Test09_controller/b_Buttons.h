#pragma once

static QueueHandle_t keyboardQueue;

void pollKeys(void *parameters) {

  static volatile Keys_t key;
  
  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;

  while(true) {
    // Read keys
    if (touchRead(PIN_BTN_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
    if (touchRead(PIN_BTN_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
    if (touchRead(PIN_BTN_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 
  
    // Send message on the queue if key is pressed long enough
    if(keyUpCounter     == 3) { key=kbUp;     xQueueSend(keyboardQueue, (void *)&key, 10); }
    if(keySelectCounter == 3) { key=kbSelect; xQueueSend(keyboardQueue, (void *)&key, 10); }
    if(keyDownCounter   == 3) { key=kbDown;   xQueueSend(keyboardQueue, (void *)&key, 10); }
  
    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
};

void startKeyboard() {
  keyboardQueue = xQueueCreate(10, sizeof(Keys_t));
  xTaskCreatePinnedToCore(pollKeys, "Keyboard", 1024, NULL, 1, NULL, app_cpu);
};
