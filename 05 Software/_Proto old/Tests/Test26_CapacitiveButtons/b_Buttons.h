#pragma once

// Constants that define the keyboard
#define KEY_THRESHOLD     45      // Level to trigger capacitive keyboard button
#define PIN_BTN_UP        12      // Capacitive pin for up button
#define PIN_BTN_SELECT     4      // Capacitive pin for select button
#define PIN_BTN_DOWN      15      // Capacitive pin for down button

hw_timer_t * timer = NULL;
bool timerEnabled = false;

// Global variables to be interpreted by the main program
volatile bool keyUp     = false;
volatile bool keySelect = false;
volatile bool keyDown   = false;
volatile unsigned long tbtns;

void IRAM_ATTR onTimer(){
  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;

  static volatile unsigned long tstart;

  if(timerEnabled) {
    // Read keys
    tstart=micros();

    if (touchRead(PIN_BTN_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
    if (touchRead(PIN_BTN_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
    if (touchRead(PIN_BTN_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 

    tbtns=micros()-tstart;
  
    // Send message on the queue if key is pressed long enough
    if(keyUpCounter     == 3) keyUp=true;
    if(keySelectCounter == 3) keySelect=true;
    if(keyDownCounter   == 3) keyDown=true;  
  }
  else {
    keyUpCounter     = 0;   keyUp     = false;
    keySelectCounter = 0;   keySelect = false;
    keyDownCounter   = 0;   keyDown   = false;
  }
}

void disableKeyboard() {
  if(timerEnabled) {
    timerAlarmDisable(timer);  
    delay(10); // Just for test, ensure pending interrupt has fired one last time
    timerEnabled=false;
  }
}

void enableKeyboard() {
  if(!timerEnabled) {
    timerAlarmEnable(timer);
    timerEnabled=true;
  }
}

void startKeyboard() {
  timer = timerBegin(3, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
}
