#pragma once

#define BACKLIGHT_TIME_OUT      60*1000
#define BACKLIGHT_HOME_SCREEN 2*60*1000
#define BACKLIGHT_LED_CHANNEL         0
#define BACKLIGHT_FREQUENCY        5000
#define BACKLIGHT_RESOLUTION          8

// setting PWM properties for the backlight
static const uint8_t BACKLIGHT_BRIGHTNESS[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 18, 22, 26, 31, 37, 45, 53, 63, 75, 90, 107, 127, 151, 180, 214, 255 }; 

void runBacklight( void *pvParameters );

void startBacklight() {

  // configure LED PWM functionalitites
  ledcSetup(BACKLIGHT_LED_CHANNEL, BACKLIGHT_FREQUENCY, BACKLIGHT_RESOLUTION);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(PIN_BACKLIGHT, BACKLIGHT_LED_CHANNEL);

  // Setup task
  xTaskCreatePinnedToCore(
    runBacklight,
    "Backlight",   // A name just for humans
    4096,                   // This stack size can be checked & adjusted by reading the Stack Highwater
    NULL,
    2,                      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    NULL, 
    ARDUINO_RUNNING_CORE);

}

void runBacklight( void *pvParameters ) {
  static unsigned long lastTimeKeyPressed  = 0;
  static uint8_t backlightIndex = 0;
  static unsigned long interval = 0;
  static bool homeMessageSent = false;
  userEventMessage_t message;

  while(true) {

    // Switch backlight off if no key was pressed for some time
    if (millis() - lastTimeKeyPressed > BACKLIGHT_TIME_OUT) {

      if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
        controllerData.backLightOn=false; 
        xSemaphoreGive( controllerDataSemaphore );
      }
    }

    // After even more time, switch the menu back to the home screen
    // if Telegram users change controller settings, the display light up in the home screen
    if (millis() - lastTimeKeyPressed > BACKLIGHT_HOME_SCREEN) {
      if(!homeMessageSent) {
        message = userEventMessage_t(sndBacklight, cmdMenuHome);
        xQueueSend( menuQueue, &message, ( TickType_t ) 60 ); 
        homeMessageSent=true;
      };
    }


    // Switch baklight on for come time if someone has talked to us
    if( xQueueReceive( backlightQueue, &message, 0) == pdPASS ) {
      Serial.printf("%s > Backlight [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed);

      if( message.command == cmdBacklightOn ) {
        controllerData.backLightOn=true;
        homeMessageSent=false;
        lastTimeKeyPressed = millis();
      }
    }

    if(controllerData.backLightOn) {
      // Make the backlight brighter
      if(BACKLIGHT_BRIGHTNESS[backlightIndex]<255) backlightIndex++;
    }
    else {
      // Dim the backlight (8x slower)
      if( (interval++ % 8) == 0) {
        if(backlightIndex>0) 
          backlightIndex--;
      }
    }

    ledcWrite(BACKLIGHT_LED_CHANNEL, BACKLIGHT_BRIGHTNESS[backlightIndex]);

    //controllerData->logBusyTime.finish(btBacklight);  

    vTaskDelay(100 / portTICK_PERIOD_MS);

  } // while true
}
