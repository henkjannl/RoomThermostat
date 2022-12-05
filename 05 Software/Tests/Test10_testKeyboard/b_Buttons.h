#pragma once

hw_timer_t * keyboardTimer = NULL; // Polls capacitive keys @~30Hz

bool btnUpTouched     = false;
bool btnSelectTouched = false;
bool btnDownTouched   = false;

void IRAM_ATTR onKeyboardTimer(){
  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;
  
  if (touchRead(PIN_BUT_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
  if (touchRead(PIN_BUT_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
  if (touchRead(PIN_BUT_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 

  if(keyUpCounter     == 3) { btnUpTouched     = true; }
  if(keySelectCounter == 3) { btnSelectTouched = true; }
  if(keyDownCounter   == 3) { btnDownTouched   = true; }
}; // onKeyboardTimer

void startKeyboard() {
  keyboardTimer = timerBegin(KEYBOARD_TIMER_ID, 80, true);
  timerAttachInterrupt(keyboardTimer, &onKeyboardTimer, true);
  timerAlarmWrite(keyboardTimer, 33000, true); // About 30 Hz
  timerAlarmEnable(keyboardTimer);
}
