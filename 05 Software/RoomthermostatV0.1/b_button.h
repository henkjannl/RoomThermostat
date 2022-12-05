#pragma once

#define KEYBOARD_TIMER_ID  0      // Number of the keyboard timer (0..3)
#define KEY_THRESHOLD     45      // Level to trigger capacitive keyboard button

typedef void (*keyboardFunction)(); // To create pointers to enableKeyboard and disablekeyboard

hw_timer_t * keyboardTimer = NULL; // Polls capacitive keys

volatile bool keyboardEnabed   = true;  // Disable touchRead() during screen update. This resolves a conflict between touchRead() and SPIFFS
volatile bool btnUpTouched     = false;
volatile bool btnSelectTouched = false;
volatile bool btnDownTouched   = false;

void enableKeyboard() {
  keyboardEnabed   = true;
};

void disableKeyboard() {
  keyboardEnabed   = false;
};

void IRAM_ATTR onKeyboardTimer(){
  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;

  if(keyboardEnabed) {
    
    if (touchRead(PIN_BUT_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
    if (touchRead(PIN_BUT_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
    if (touchRead(PIN_BUT_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 
  
    if(keyUpCounter     == 3) { btnUpTouched     = true; }
    if(keySelectCounter == 3) { btnSelectTouched = true; }
    if(keyDownCounter   == 3) { btnDownTouched   = true; }
  }
  
}; // onKeyboardTimer

void buttonsStart() {
  keyboardTimer = timerBegin(KEYBOARD_TIMER_ID, 80, true);
  timerAttachInterrupt(keyboardTimer, &onKeyboardTimer, true);
  timerAlarmWrite(keyboardTimer, 50000, true); // About 20 Hz
  timerAlarmEnable(keyboardTimer);
}
