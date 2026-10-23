#include <TFT_eSPI.h>
#include <SPI.h>
#include <JPEGDecoder.h>
#include "SPIFFS.h"
#include "time.h"

#include <esp_task_wdt.h> // Watchdog timer

#define WDT_TIMEOUT 180   // 3 minutes watchdog


//#define USE_ILI9341
#define USE_BUTTONS

// Preceding letters added since Arduino opens libraries in alphabetical order
#include "a_Constants.h"
#include "b_Buttons.h"
#include "c_Eventlog.h"
#include "d_Heaterprogram.h"
#include "e_Controller.h"
#include "f_Wifi.h"
#include "g_Time.h"
#include "h_Datalogging.h"
#include "i_Telegram.h"

#ifdef USE_ILI9341
#include "j_jpeg.h"
#include "k_Menu.h"
#endif


TFT_eSPI tft = TFT_eSPI();       
TFT_eSprite menuItem = TFT_eSprite(&tft);
Data data;

#ifdef USE_ILI9341
Menu menu(&tft, &menuItem, &data);
#endif

/*
ToDo: 

implement low pass filter on temperature measurements

study effectivity of anti-windup during the night

make the status description in Telegram more clear, perhaps also with a hint on next steps

check if Opentherm indeed needs 4 wires (3V3, GND, in, out)
solder actual device
check if buttons work (perhaps lower controller and log frequency)

align normal menu structure with telegram menu structure 

implement saving data in JSON file, including whole schedule

also setting for temperature offset
also setting for hot water temperature
implement switching heater off permanently

additional screen for boiler data

menu to set the time if wifi if not connected
incorporate screen saver and wake-up
check if menuItem sprite can also be declared in the menu class (instead of being a global variable)
export logfile through telegram CSV

over the air updates through telegram

permanently load JPEGs in RAM
perhaps more colors for icons

wifi connection through local hub
*/
 
void setup() {
  // Initilize serial port
  Serial.begin(115200);

  // Initialize SPIFFS
  SPIFFS.begin();
  delay(500);

  // Load config data from SPIFFS
  data.loadConfig(SPIFFS, CONFIG_FILE);

  // Prepare backlight dimming
  /*
  ledcSetup(BCK_LED_CHANNEL, BCK_PWM_FREQ, BCK_RESOLUTION);
  ledcAttachPin(PIN_BACKLIGHT, BCK_LED_CHANNEL);
  ledcWrite(BCK_LED_CHANNEL, 8);
  */
  
  #ifdef USE_ILI9341
  // Initialize screen
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(CLR_LIGHT_TEXT);
  tft.setTextDatum(MC_DATUM);
  tft.loadFont(FONT_TEMP_MEASURED); 
  tft.setTextColor(CLR_DARK_TEXT, CLR_LIGHT_TEXT);
  tft.drawString("Welcome", 160, 120);
  #endif

  // Initialize wifi
  setupWifi(data);

  // Initialize keyboard
  startKeyboard();

  // Initialize temperature controller
  startController();
  
  data.wifiConnected = (WiFi.status() == WL_CONNECTED);

  // Check current daylight saving time for our timezone
  requestTimeZoneIfNeeded(data);

  // Sync with atomic clock
  syncTimeIfNeeded(data);

  setupTelegram(data);

  // Draw menu
  #ifdef USE_ILI9341
  menu.selectMenu(mnHomeScreen);
  #endif

  // Enable watchdog
  Serial.println("Configuring watchdog");
  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  
  logMessage("Thermostat restarted");
}

void loop() {
  
  // Check if a button is pressed
  #ifdef USE_ILI9341
  if     (keyUp)     { menu.buttonUp();     keyUp    =false; }
  else if(keySelect) { menu.buttonSelect(); keySelect=false; }
  else if(keyDown  ) { menu.buttonDown();   keyDown  =false; }
  #endif

  data.wifiConnected = (WiFi.status() == WL_CONNECTED);

  // Determine if setpoint temperature needs to be changed
  data.setSetpoint();

  requestTimeZoneIfNeeded(data);
  syncTimeIfNeeded(data);
  
  #ifdef USE_ILI9341
  menu.periodicScreenUpdate();
  #endif

  reconnectIfNeeded(data);
  handleTelegramIfNeeded(data);
  logIfNeeded(data);
  sendLogFileIfNeeded();
  controlTemperatureIfNeeded(data);

  // Reset watchdog timer
  esp_task_wdt_reset();      
}
