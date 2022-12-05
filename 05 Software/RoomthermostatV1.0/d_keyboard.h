#pragma once

#define KEYBOARD_TIMER_ID  0      // Number of the keyboard timer (0..3)
#define KEY_THRESHOLD     25      // Level to trigger capacitive keyboard button

hw_timer_t * keyboardTimer = NULL; // Polls capacitive keys

// Disable touchRead() during screen update. This resolves a conflict between touchRead() and SPIFFS

// Since multiple processes can block and enable the keyboard, we need to count how many
// processes block the keyboard rather than provide a boolean
volatile int16_t keyboardEnabed   = 0;  
volatile uint32_t keyboardInterruptCounter1 = 0;
volatile uint32_t keyboardInterruptCounter2 = 0;
volatile uint32_t keyboardInterruptCounter3 = 0;

// If true, device is controlled through keyboard
// if false, device is controlled by Telegram
// key press activates this mode, backlight switches it off

bool keyboardMode = false;


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

  keyboardInterruptCounter1++;
  
  if(keyboardEnabed==0) {

      keyboardInterruptCounter2++;

    if (touchRead(PIN_BUT_UP    )<KEY_THRESHOLD) keyUpCounter++;     else keyUpCounter     = 0; 
    if (touchRead(PIN_BUT_SELECT)<KEY_THRESHOLD) keySelectCounter++; else keySelectCounter = 0; 
    if (touchRead(PIN_BUT_DOWN  )<KEY_THRESHOLD) keyDownCounter++;   else keyDownCounter   = 0; 
  
    if(keyUpCounter     == 3) { 
      userEventMessage_t message = userEventMessage_t(sndKeyboard, cmdKeyUp); 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
      keyboardMode=true; // Pause Telegram to get better response of keyboard
      keyboardInterruptCounter3++;
    }

    if(keySelectCounter == 3) { 
      userEventMessage_t message = userEventMessage_t(sndKeyboard, cmdKeySelect); 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
      keyboardMode=true; // Pause Telegram to get better response of keyboard
      keyboardInterruptCounter3++;
    }

    if(keyDownCounter   == 3) { 
      userEventMessage_t message = userEventMessage_t(sndKeyboard, cmdKeyDown); 
      xQueueSendFromISR( menuQueue, &message, ( TickType_t ) 0 );  
      keyboardMode=true; // Pause Telegram to get better response of keyboard
      keyboardInterruptCounter3++;
    }
  }
  
}; // onKeyboardTimer

void startKeyboard() {
  keyboardTimer = timerBegin(KEYBOARD_TIMER_ID, 80, true);
  timerAttachInterrupt(keyboardTimer, &onKeyboardTimer, true);
  timerAlarmWrite(keyboardTimer, 33000, true); // About 30 Hz
  timerAlarmEnable(keyboardTimer);
}
