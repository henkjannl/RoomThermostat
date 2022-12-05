#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

//#include "task.h"

#include "a_constants.h"
#include "b_interfaces.h"
#include "c_logger.h"
#include "d_keyboard.h"
#include "e_backlight.h"
#include "f_menu.h"
#include "g_jpeg.h"
#include "h_display.h"
#include "i_telegram.h"
#include "j_controller.h"
#include "k_temperature.h"
#include "l_wifi_time.h"

#include <esp_task_wdt.h>      // Watchdog timer
#define WATCHDOG_TIMEOUT 180   // 3 minute watchdog

/* TO DO:
  implement Off mode of the thermostat
  implement database to record all data
  icons in front of menus
  implement cmdBoilerSending and cmdBoilerNormal in display
  implement String dspDate in controller etc.
  merge userEventMessage_t with command_t
  display progress upon startup
  
  Update all Telegram chats every 15 minutes or so
  run TemperatureSensorConnect() as part of regular maintenance loop, if sensor is not connected
  Make central store of variables, that can also be saved to / retrieved from permanent memory
  Find replace action Leave > GoOut 
  Add save and load of persistent data
  Do autosave if persistent data is changed

  Implement icons for setpoint and measured value in Telegram
  Log data to database in the cloud

  Conflict between touchRead and SPIFFS is now resolved by disabling keyboard during use of screen. Perhaps sufficient to only disable during sprite.loadFont()
  Automatc updates to last message of known clients every 15 minutes
  Icons in front of menu items
  Create an additional debug screen (done in telegram)
  Allow user to modify water temperature of heater and shower
  Indicate day icons for overruled days with different color
  Replace setValue() and getValue() of DisplayParameter_t to property
  Try if SPI frequency in TFT_eSPI can be higher
  Check if D-action is implemented well in PID controller
*/

void setup() {
  // Initilize serial port
  Serial.begin(115200);

  // Initialize SPIFFS
  SPIFFS.begin();
  delay(500);

  Serial.println("Loading config data");
  controllerData.loadConfig(SPIFFS, CONFIG_FILE);

  Serial.println("Initializing temperature sensors and boiler");
  startTemperature();

  Serial.println("Initializing backlight");
  startBacklight();

  Serial.println("Initializing display");
  startDisplay();

  Serial.println("Initializing controller");
  startController();

  Serial.println("Initializing WiFi");
  setupWifi(controllerData);

  Serial.println("Initializing Telegram handler");
  beginTelegram(); // Start handler and link to FreeRTOS task

  Serial.println("Initializing menu");
  startMenu();

  Serial.println("Initializing keyboard");
  startKeyboard();

  // Request display to show the home screen
  userEventMessage_t message = userEventMessage_t(sndMenu, cmdUpdateScreen); 
  message.screen=scnHome;
  message.selectedMenuItem=cmdMenuMain;
  xQueueSend( displayQueue, & message, ( TickType_t ) 10 );  

  // Enable watchdog
  Serial.println("Configuring watchdog");
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.println("Setup finished\n\n");
}

void loop() {
  static unsigned long lastEvent = millis();  
  static int eventCounter = 0;
  static int counter =0;
  userEventMessage_t message; 

  /*
  // This is for debug purposes
  if(millis() - lastEvent > 2000){
    lastEvent=millis();
    
    switch(eventCounter++) {
      case 0:
        Serial.println("\nEvent 0: Send cmdOverruleTodayWorkFromHome from Menu to Controller");
        message.sender=sndMenu;
        message.command=cmdOverruleTodayWorkFromHome;
        xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 1:
        Serial.println("\nEvent 1: Send cmdBoilerSending from Boiler to Display");
        message.sender=sndBoiler;
        message.command=cmdBoilerSending;
        xQueueSend( displayQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 2:
        Serial.println("\nEvent 2: Send cmdBoilerNormal from Boiler to Display");
        message.sender=sndBoiler;
        message.command=cmdBoilerNormal;
        xQueueSend( displayQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 3:
        Serial.println("\nEvent 3: Send keyUp from Keyboard to Menu");
        message.sender=sndKeyboard;
        message.command=cmdKeyUp;
        xQueueSend( menuQueue, & message, ( TickType_t ) 10 );  
        break;
      
      case 4:
        Serial.println("\nEvent 4: Send keyDown from Keyboard to Menu");
        message.sender=sndKeyboard;
        message.command=cmdKeyDown;
        xQueueSend( menuQueue, & message, ( TickType_t ) 10 );  
        break;
      
      case 5:
        Serial.println("\nEvent 5: Send keySelect from Keyboard to Menu");
        message.sender=sndKeyboard;
        message.command=cmdKeySelect;
        xQueueSend( menuQueue, & message, ( TickType_t ) 10 );  
        break;
      
      case 6:
        Serial.println("\nEvent 6: Select offset settings menu");
        message.sender=sndMenu;
        message.screen=scnSettingsSensorOffset;
        message.selectedMenuItem=cmdSensorOffsetDown;
        xQueueSend( displayQueue, & message, ( TickType_t ) 10 );  
        break;      
    }
  }
  */
  
  if(++counter % 100 ==0) {
    checkTemperatureIfNeeded();
    reconnectIfNeeded(controllerData);
    requestTimeZoneIfNeeded(controllerData);
    syncTimeIfNeeded(controllerData);
  }
    
  // Reset watchdog timer
  esp_task_wdt_reset();      
}
