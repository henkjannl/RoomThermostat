
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"

#define VERSION "1.0.6"

/* 
  VERSION INFO:
  1.0.0 resolved some errors, improved telegramEnabled()
  1.0.1 timing report removed to prevent locking telegram
  1.0.2 added detailed debug report
        software version added
  1.0.3 boiler message shorter to prevend lockup Telegram
  1.0.4 removed the semaphore since we are no longer using FreeRTOS
        alternative queue for keyboard with simple message
        merge userEventMessage_t with command_t
        BugFix: Telegram responds only is ChatID is in message
  1.0.5 New structure of events and messages implemented
        unity of keyboard message restored
        drive backlight dimming with hardware timer
        increased value of bot.maxMessageLength to prevent lockup of Telegram
        return 7 day icons in Telegram
        implement cmdBoilerSending and cmdBoilerNormal in display
        create an additional debug screen (done in telegram)
  1.0.6 Simple logger added
        Low temperature of boiler control reduced to 0°C

  TO DO:
  cleanup Serial.print..
  skip unicode from Telegram until it is working better
  explore if unicode is the problem why Telegram is not working as well as expected
  check is the menu still needs to send currentScreen and currentMenuItem
  include over the air updates
  implement Off mode of the thermostat
  implement database to record all data
  icons in front of menus
  implement String dspDate in controller etc.
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
  Automatic updates to last message of known clients every 15 minutes
  Allow user to modify water temperature of heater and shower
  Indicate day icons for overruled days with different color
  Replace setValue() and getValue() of DisplayParameter_t to property
  Try if SPI frequency in TFT_eSPI can be higher
  Check if D-action is implemented well in PID controller
*/

#include "a_constants.h"
#include "b_logger.h"
#include "c_interfaces.h"
#include "d_keyboard.h"
#include "e_menu.h"
#include "f_telegram.h"
#include "g_controller.h"
#include "h_temperature.h"
#include "i_wifi_time.h"
#include "j_jpeg.h"
#include "k_display.h"
#include "l_backlight.h"

#include <esp_task_wdt.h>      // Watchdog timer
#define WATCHDOG_TIMEOUT 180   // 3 minute watchdog
#define REPORT_INTERVAL 60000

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


void setup() {
  userEventMessage_t message;
  
  // Initilize serial port
  Serial.begin(115200);

  // Initialize SPIFFS
  delay(300);
  if( !SPIFFS.begin() ) Serial.println("SPIFFS Mount Failed");
  delay(300);
  //listDir(SPIFFS, "/", 0);

  Serial.println("Loading config data");
  controllerData.loadConfig(SPIFFS, CONFIG_FILE);

  Serial.println("Initializing temperature sensors and boiler");
  startTemperature();

  Serial.println("Initializing display");
  startDisplay();

  Serial.println("Initializing backlight");
  startBacklight();

  Serial.println("Initializing controller");
  //startController();

  Serial.println("Initializing WiFi");
  setupWifi(controllerData);

  Serial.println("Initializing Telegram handler");
  beginTelegram(); 

  Serial.println("Initializing menu");
  startMenu();

  Serial.println("Initializing keyboard");
  startKeyboard();

  sendMessage(sndBacklight, cmdMenuHome,      menuQueue);       // Goto home screen
  sendMessage(sndMenu,      cmdUpdateScreen,  controllerQueue); // Control setpoint and display screen

  // Enable watchdog
  Serial.println("Configuring watchdog");
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.println("Setup finished\n\n");
}

void loop() {
  static unsigned long lastTimeDetailedReport=0;      // last time temperature measurement was done
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
        Serial.println("\nEvent 0: Select screen scnMain, cmdMenuOverruleToday");
        selectScreen(scnMain, cmdMenuOverruleToday);
        break;
      
      case 1:
        Serial.println("\nEvent 1: Press select key");
        message = userEventMessage_t(sndKeyboard, cmdKeySelect); 
        xQueueSend( menuQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 2:
        Serial.println("\nEvent 2: Press down key");
        message = userEventMessage_t(sndKeyboard, cmdKeyDown); 
        xQueueSend( menuQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 3:
        Serial.println("\nEvent 3: Press select key");
        message = userEventMessage_t(sndKeyboard, cmdKeySelect); 
        xQueueSend( menuQueue, &message, ( TickType_t ) 10 );  
        break;
      
      case 4:
        Serial.println("\nEvent 4: Next item");
        menu.nextMenuItem();
        menu.dump();    
        break;
      
      case 5:
        Serial.println("\nEvent 5: First item");
        menu.firstMenuItem();
        menu.dump();    
        break;
      
      case 6:
        Serial.println("\nEvent 6: Previous item");
        menu.prevMenuItem();
        menu.dump();    
        break;      
    }
  }
*/

  controllerData.logBusyTime.start(btTotal);
  controllerData.logBusyTime.start(btTemperature);    checkTemperatureIfNeeded();              controllerData.logBusyTime.finish(btTemperature);    
  controllerData.logBusyTime.start(btMenu);           checkMenu();                             controllerData.logBusyTime.finish(btMenu);     
  controllerData.logBusyTime.start(btDisplay);        checkDisplayQueue();                     controllerData.logBusyTime.finish(btDisplay);
  controllerData.logBusyTime.start(btTelegram);       checkTelegramIfNeeded();                 controllerData.logBusyTime.finish(btTelegram);
  controllerData.logBusyTime.start(btController);     processControllerIfNeeded();             controllerData.logBusyTime.finish(btController);
  controllerData.logBusyTime.start(btBacklight);      checkBacklight();                        controllerData.logBusyTime.finish(btBacklight);
  controllerData.logBusyTime.start(btWifi);           reconnectWiFiIfNeeded(controllerData);   controllerData.logBusyTime.finish(btWifi);
  
  controllerData.logBusyTime.start(btTimeSynch);      
  requestTimeZoneIfNeeded(controllerData); 
  syncTimeIfNeeded(controllerData); 
  controllerData.logBusyTime.finish(btTimeSynch);

  controllerData.logBusyTime.finish(btTotal);

  if (millis() - lastTimeDetailedReport > REPORT_INTERVAL) {
    lastTimeDetailedReport = millis();
    Serial.println( String("Wifi connected: ") + String(wifiConnected() ? "Y" : "N") );
    Serial.println( controllerData.logBusyTime.report() );
    int64_t t1 = esp_timer_get_time();
    multi_heap_info_t heapInfo; 
    heap_caps_get_info( &heapInfo, MALLOC_CAP_DEFAULT );
    
    Serial.println( String("Software version: ") + VERSION );
    Serial.println( String("- Time since reboot: ") + secondsToDuration(t1/1e6) ); 
    Serial.println( String("- Temperature measurement successful: ") + String(controllerData.temperatureMeasurementOK ? "Y" : "N") ); 
    Serial.println( String("- Total free bytes in the heap: ") + String(heapInfo.total_free_bytes) ); 
    Serial.println( String("- Total bytes allocated to data in the heap: ") + String(heapInfo.total_allocated_bytes) ); 
    Serial.println( String("- Size of the largest free block in the heap: ") + String(heapInfo.largest_free_block) ); 
    Serial.println( String("- Lifetime minimum free heap size: ") + String(heapInfo.minimum_free_bytes) ); 
    Serial.println( String("- Number of blocks allocated in the heap: ") + String(heapInfo.allocated_blocks) ); 
    Serial.println( String("- Number of free blocks in the heap: ") + String(heapInfo.free_blocks) ); 
    Serial.println( String("- Total number of blocks in the heap: ") + String(heapInfo.total_blocks) ); 
    
  }

  // Reset watchdog timer
  esp_task_wdt_reset();      
}
