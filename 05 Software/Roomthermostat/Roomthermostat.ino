
#define ARDUINOJSON_DECODE_UNICODE 1
#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"

#define VERSION "1.1.0"
#define USE_TESTBOT true

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
        Telegram module simplified to be more in line with architectural graph
  1.0.7 Boiler communication interval reduced to 5 sec since otherwise the boiler 
        switches to non-Opentherm mode and stops boiling since the connection is open
  1.0.8 Autosave of settings to SPIFFS
        increased value of Telegram maxMessageLength to 6000 to prevent lockup at cmdReportLog
  1.1.0 Over the air software updates implemented

  TO DO:
    Potential improvements:
    * change main menu to switch off multipleForever
    * introduce permanent 'off' mode, for instance during the summer
    * improve responsiveness
    * send logfile as attachment to Telegram
    * include icons in the menu
    * allow user to modify water temperature of heater and shower
    * save logdata through WiFi connection (Deta Base?)
    * much code can be simplified to remove structures that were used in previous attempts to get the code working
    * try to get FreeRTOS working again to improve performance of buttons and Telegram
    * automatic updates to last message of known clients every 15 minutes
    * implement OpenTherm protocoll in hardware timer
    * optimize actual heater control functionality
        * check if D-action is implemented well in PID controller
        * take into account the weather in the control strategy
        * monitor the time it takes to heat the room in the morning, and compensate that by starting earlier
    * also enable setting the time and date through hardware buttons if WiFi is unavailable
    * display progress upon startup
    * try if SPI frequency in TFT_eSPI can be higher
    
    Code cleanup:
    * find replace action GoOut > Leave
    * cleanup Serial.print
    * check use of const in function calls
    * cleanup the use of messages. Some fields may no longer be needed
    
    Hardware:
    * add a capacitor to prevent brownout of the ESP32 at startup
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

  // Prepare onboard LED for output
  pinMode(PIN_ESP32_LED, OUTPUT);

  // Initialize SPIFFS
  delay(300);
  if( !SPIFFS.begin() ) Serial.println("SPIFFS Mount Failed");
  delay(300);
  //listDir(SPIFFS, "/", 0);

  Serial.printf("Loading config data from '%s'\n", CONFIG_FILE);
  controllerData.loadConfig(SPIFFS, CONFIG_FILE);

  Serial.println("Loading controller settings");
  controllerData.loadSettings(SPIFFS, SETTINGS_FILE);   
  controllerData.settingsChanged = false; 

  Serial.println("Initializing menu");
  startMenu();

  Serial.println("Initializing temperature sensors and boiler");
  startTemperature();

  Serial.println("Initializing controller");
  startController();

  Serial.println("Initializing display");
  startDisplay();

  Serial.println("Initializing backlight");
  startBacklight();

  Serial.println("Initializing WiFi");
  setupWifi(controllerData);

  Serial.println("Initializing Telegram handler");
  startTelegram(); 

  Serial.println("Initializing keyboard");
  startKeyboard();

  // Enable watchdog
  Serial.println("Configuring watchdog");
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.println("Setup finished\n\n");
}

void loop() {
  static unsigned long lastTimeDetailedReport=0;      // last time temperature measurement was done
  static unsigned long lastSettingsSave = millis();  
  static int eventCounter = 0;
  static int counter =0;
  userEventMessage_t message; 

  // If controller settings were changed, autosave the settings every minute
  if(millis() - lastSettingsSave > 60*1000 ){
    lastSettingsSave = millis();
    if( controllerData.settingsChanged ) {
      disableKeyboard(); // Really necessary!
      controllerData.saveSettings(SPIFFS, SETTINGS_TEMP, SETTINGS_FILE);    
      enableKeyboard();
    }
  }

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
    Serial.println( String("- Keyboard polling microseconds: ") + String(keyboardMicroSeconds) ); 
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
