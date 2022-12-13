#pragma once

//static const uint8_t BACKLIGHT_BRIGHTNESS[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 18, 22, 26, 31, 37, 45, 53, 63, 75, 90, 107, 127, 151, 180, 214, 255 }; 
const std::list<uint8_t> BACKLIGHT_BRIGHTNESS = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 18, 22, 26, 31, 37, 45, 53, 63, 75, 90, 107, 127, 151, 180, 214, 255 }; 

//#define MAX_BACKLIGHT_BRIGHTNESS 27
//uint8_t backlightIndex=MAX_BACKLIGHT_BRIGHTNESS;
 
void changeBacklight(bool on) {

  if( on ) {
    Serial.println("Really trying to switch the backlight on..");
    for (auto brightness = BACKLIGHT_BRIGHTNESS.begin(); brightness != BACKLIGHT_BRIGHTNESS.end(); ++brightness) {
      ledcWrite(BACKLIGHT_LED_CHANNEL, *brightness);
      delay(10);
    }
  }
  else {
    Serial.println("Really trying to switch the backlight off..");
    for (auto brightness=BACKLIGHT_BRIGHTNESS.rbegin(); brightness != BACKLIGHT_BRIGHTNESS.rend(); ++brightness) {
      ledcWrite(BACKLIGHT_LED_CHANNEL, *brightness);
      delay(40);
    }
  }

  /*
  // Brighten the backlight if desired
  while( backlightIndex<newIndex ) {
    backlightIndex++;
    ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
    delay(10);
  }

  // Dim the backlight if desired
  while( backlightIndex>newIndex ) {
    backlightIndex--;
    ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
    delay(30);
  }
  */
}

void checkBacklight() {
  static unsigned long lastTimeBacklightOn  = 0; // When this timer expires, the backlight starts dimming
  static unsigned long lastBacklightDim = 0;     // This corresponds to each dim step
  static uint8_t backlightIndex = 0;
  static unsigned long interval = 0;
  userEventMessage_t message, messageOut;

  // Switch backlight on for some time if someone has talked to us
  if( xQueueReceive( backlightQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Backlight [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed );

    if( (message.command == cmdBacklightOn) ) {
        Serial.println("cmdBacklightOn: Backlight switching on");
        lastTimeBacklightOn = millis();               
        
        if( !controllerData.backLightOn ) { 
          //changeBacklight(MAX_BACKLIGHT_BRIGHTNESS);
          changeBacklight(true);
          controllerData.backLightOn=true;          
        }
    } // if( message.command == cmdBacklightOn) 
  } // xQueueReceive

  // Switch off backlight after some time 
  if ( (millis() - lastTimeBacklightOn > BACKLIGHT_TIMEOUT) and controllerData.backLightOn ) {

      if( controllerData.backLightOn ) {
        controllerData.backLightOn=false; 
        //changeBacklight(0);
        changeBacklight(false);
      };
      sendMessage(sndBacklight, cmdEnableTelegram, telegramQueue);  
  }

/*  
    
  // Brighten or dim the backlight
  if ( (millis() - lastBacklightDim > BACKLIGHT_DIM_STEP) ) {
    lastBacklightDim = millis();

    if( controllerData.backLightOn ) {
        // Make the backlight brighter
        if( BACKLIGHT_BRIGHTNESS[backlightIndex] < 255 ) {
          backlightIndex++;
          ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
        }
      }
      else {
        // Dim the backlight (8x slower)
        if( (++interval % 8) == 0) {
          
          // Continue to dim the backlight until backlight fully switched off
          if (backlightIndex>0) {
            backlightIndex--;
            ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);
          }
  
          if( backlightIndex == 1 ) {
                      
            // Backlight almost dimmed. Tell menu to pop back into homescreen and enable Telegram
            message.sender = sndBacklight; 
            message.command = cmdMenuHome;
            xQueueSend( menuQueue, &message, ( TickType_t ) 10 );
  
            message.sender = sndMenu;
            message.command = cmdEnableTelegram;
            xQueueSend( telegramQueue, &message, ( TickType_t ) 10 );
            
          } // backlightIndex == 1
          
        } // interval % 8 == 0
      } // if( backlightSwitchedOn )
  } // if ( (millis() - lastBacklightDim > BACKLIGHT_DIM_STEP) ) 
*/
};

void startBacklight() {
  userEventMessage_t message;

  // configure LED PWM functionalities
  ledcSetup(BACKLIGHT_LED_CHANNEL, BACKLIGHT_FREQUENCY, BACKLIGHT_RESOLUTION);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(PIN_BACKLIGHT, BACKLIGHT_LED_CHANNEL);

  // Start with a partially dimmed screen to display the welcome message
  ledcWrite(BACKLIGHT_LED_CHANNEL, 64);

  message = userEventMessage_t(sndBacklight, cmdBacklightOn);
  xQueueSend( backlightQueue, &message, ( TickType_t ) 0 );
  checkBacklight();
};
