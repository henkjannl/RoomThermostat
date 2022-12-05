#pragma once

// Constants that define the keyboard
#define KEY_THRESHOLD     50      // Level to trigger capacitive keyboard button
#define PIN_BTN_UP        12      // Capacitive pin for up button
#define PIN_BTN_SELECT     4      // Capacitive pin for select button
#define PIN_BTN_DOWN      15      // Capacitive pin for down button

hw_timer_t * timer = NULL;

volatile bool keyUp     = false;
volatile bool keySelect = false;
volatile bool keyDown   = false;

void IRAM_ATTR onTimer(){

  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;

  // Read keys
  if (touchRead(PIN_BTN_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
  if (touchRead(PIN_BTN_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
  if (touchRead(PIN_BTN_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 

  // Send message on the queue if key is pressed long enough
  if(keyUpCounter     == 3) keyUp=true;
  if(keySelectCounter == 3) keySelect=true;
  if(keyDownCounter   == 3) keyDown=true;  
}

void disableKeyboard() {
  #ifdef USE_BUTTONS
  timerAlarmDisable(timer);  
  #endif
}

void enableKeyboard() {
  #ifdef USE_BUTTONS
  timerAlarmEnable(timer);
  #endif
}

void startKeyboard() {
  #ifdef USE_BUTTONS
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 30000, true);
  timerAlarmEnable(timer);
  #endif
}
