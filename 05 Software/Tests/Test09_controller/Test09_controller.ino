#include <TFT_eSPI.h>
#include <SPI.h>
#include <JPEGDecoder.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "time.h"

// Preceding letters added since Arduino opens libraries in alphabetical order
#include "a_Constants.h"
#include "b_Buttons.h"
#include "c_jpeg.h"
#include "d_Menu.h"
#include "e_Controller.h"
#include "f_Wifi.h"
#include "g_Time.h"

// Global objects
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress thermometer;

TFT_eSPI tft = TFT_eSPI();       
TFT_eSprite menuItem = TFT_eSprite(&tft);
Data data;
Menu menu(&tft, &menuItem, &data);


/*
ToDo: 
implement temperature measurement
implement controller
implement logging of relevant signals
connect to internet
sync time with internet
do round robin of checking wifi, re-sync time, poll boiler, measure temperature, etc.
additional screen of boiler data
incorporate screen saver and wake-up
storing data in JSON file
check if menuItem sprite can also be declared in the menu class (instead of being a global variable)
permanently load JPEGs in RAM
perhaps more colors for icons
*/
 
void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  delay(500);
  data.Load(SPIFFS, CONFIG_FILE);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(CLR_LIGHT_TEXT);
  tft.fillScreen(CLR_BACKGROUND);
  data.initThermometer(sensor);

  startKeyboard();
  menu.drawScreen();
  setupWifi(data);
  syncTimeIfNeeded(data);
}

void loop() {
  Keys_t key;

  data.wifiConnected = (WiFi.status() == WL_CONNECTED);
  
  // Check if a button is pressed
  if (xQueueReceive(keyboardQueue, (void *)&key, 0) == pdTRUE) {
    if     (key==kbUp)     { menu.buttonUp();     }
    else if(key==kbSelect) { menu.buttonSelect(); }
    else if(key==kbDown  ) { menu.buttonDown();   }
  }  
  syncTimeIfNeeded(data);
  menu.periodicScreenUpdate();
}
