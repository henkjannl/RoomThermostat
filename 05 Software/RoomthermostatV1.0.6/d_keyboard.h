#pragma once

hw_timer_t * keyboardTimer = NULL; // Polls capacitive keys

// Disable touchRead() during screen update. This resolves a conflict between touchRead() and SPIFFS

// Since multiple processes can block and enable the keyboard, we need to count how many
// processes block the keyboard rather than provide a boolean
volatile int16_t  keyboardEnabed = 0;  
volatile int16_t  keyboardMicroSeconds = 0;  

// If true, device is controlled through keyboard
// if false, device is controlled by Telegram
// key press activates this mode, backlight switches it off

void disableKeyboard() {
  keyboardEnabed++;
};

void enableKeyboard() {
  keyboardEnabed--;
  if(keyboardEnabed<0) Serial.println("keyboardEnabed < 0");
};

void IRAM_ATTR onKeyboardTimer(){
  static volatile uint32_t keyUpCounter     = 0;
  static volatile uint32_t keySelectCounter = 0;
  static volatile uint32_t keyDownCounter   = 0;
  static userEventMessage_t message; 
  
  if(keyboardEnabed==0) {

    volatile unsigned long startKeyboard = micros();
    
    if ( touchRead(PIN_BUT_UP    )<KEY_THRESHOLD ) keyUpCounter++;     else keyUpCounter     = 0; 
    if ( touchRead(PIN_BUT_SELECT)<KEY_THRESHOLD ) keySelectCounter++; else keySelectCounter = 0; 
    if ( touchRead(PIN_BUT_DOWN  )<KEY_THRESHOLD ) keyDownCounter++;   else keyDownCounter   = 0; 

    keyboardMicroSeconds = micros() - startKeyboard;
  
    if( keyUpCounter == 2 ) { 
      message.sender  = sndKeyboard;
      message.command = cmdKeyUp; 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
    }

    if(keySelectCounter == 2 ) { 
      message.sender  = sndKeyboard;
      message.command = cmdKeySelect; 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
    }

    if(keyDownCounter == 2 ) { 
      message.sender  = sndKeyboard;
      message.command = cmdKeyDown; 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
    }
  } // keyboardEnabed==0
 
}; // onKeyboardTimer

void startKeyboard() {
  keyboardTimer = timerBegin(KEYBOARD_TIMER_ID, 80, true);
  timerAttachInterrupt(keyboardTimer, &onKeyboardTimer, true);
  timerAlarmWrite(keyboardTimer, 10000, true); // About 100 Hz
  timerAlarmEnable(keyboardTimer);
}
