#include <TFT_eSPI.h>
#include <SPI.h>

/* Test program to ensure screen and capacitive keys work together
   tested successfully on 13 March 2022
   Also tests disabling and enabling of keyboard
*/

#include "b_Buttons.h"

TFT_eSPI tft = TFT_eSPI();       

#include <OpenTherm.h>
#define OT_IN_PIN         22 // beware: in and out were flipped due to mistake creating a symbol in KiCad
#define OT_OUT_PIN        21
OpenTherm ot(OT_IN_PIN, OT_OUT_PIN);

void ICACHE_RAM_ATTR handleInterrupt() {
  ot.handleInterrupt();
}
 
void setup() {
  // Initilize serial port
  Serial.begin(115200);

  // Initialize screen
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  Serial.println("Initializing opentherm interface");
  // Driver for opentherm shield
  ot.begin(handleInterrupt);

  Serial.println("Initializing keyboard");

  // Initialize keyboard
  startKeyboard();
  enableKeyboard();
  Serial.println("Starting main loop");
}

void loop() {
  static unsigned long counter=0;
  static String responseFromController="Not started yet";
  
  // Check if a button is pressed
  tft.fillCircle(20,20,10,keyUp ? TFT_RED : TFT_DARKGREY); 
  if(keyUp)     { 
    Serial.println("UP");
    keyUp=false; 
    } 
  
  tft.fillCircle(40,20,10,keySelect ? TFT_GREEN : TFT_DARKGREY); 
  if(keySelect) { 
    Serial.println("SELECT");
    keySelect=false;
    counter=0;
    } 
  
  tft.fillCircle(60,20,10,keyDown ? TFT_BLUE : TFT_DARKGREY); 
  if(keyDown) { 
    Serial.println("DOWN");
    keyDown  =false;
    } 

  counter++;

  tft.fillCircle(40,40,10, openthermEnabled ? TFT_YELLOW : TFT_DARKGREY);

  if(counter % 10 == 0) {
    if (openthermEnabled) {
      unsigned long openThermStart = micros();
      unsigned long response = ot.setBoilerStatus(true, true, false);
      OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
      responseFromController = String(ot.statusToString(responseStatus));
      unsigned long openThermDuration = micros()-openThermStart;
      Serial.println("Status: " + responseFromController);
      Serial.printf("OpenTherm blocks the prcess for %d microseconds\n", openThermDuration);
    }
    Serial.printf("Buttons block the prcess for %d microseconds\n", btnduration);
    Serial.printf("Opentherm disable counter %d\n", disableOpenthermTicks);
  }
    
  delay(200);
  
}
