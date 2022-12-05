#include <TFT_eSPI.h>
#include <SPI.h>

/* Test program to ensure screen and capacitive keys work together
   tested successfully on 13 March 2022
   Also tests disabling and enabling of keyboard
*/

#include "b_Buttons.h"

TFT_eSPI tft = TFT_eSPI();       
 
void setup() {
  // Initilize serial port
  Serial.begin(115200);

  // Initialize screen
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);

  Serial.println("Initializing keyboard");

  // Initialize keyboard
  startKeyboard();
  enableKeyboard();
  Serial.println("Starting main loop");
}

void loop() {
  static unsigned long counter=0;
  
  // Check if a button is pressed
  if(keyUp)     { 
    tft.fillCircle(20,20,10,TFT_RED); 
    Serial.println("UP");
    keyUp=false; 
    } 
  else tft.fillCircle(20,20,10,TFT_DARKGREY); 
  
  if(keySelect) { 
    tft.fillCircle(40,20,10,TFT_GREEN); 
    Serial.println("SELECT");
    keySelect=false;
    disableKeyboard();
    Serial.println("Keyboard disabled");    
    counter=0;
    } 
  else tft.fillCircle(40,20,10,TFT_DARKGREY); 
  
  if(keyDown) { 
    tft.fillCircle(60,20,10,TFT_BLUE); 
    Serial.println("DOWN");
    keyDown  =false;
    } 
  else tft.fillCircle(60,20,10,TFT_DARKGREY); 

  counter++;
  if(counter==20) {
    enableKeyboard();
    Serial.println("Keyboard enabled");
    Serial.printf ("Last keyboard poll took %d seconds\n", tbtns);
  }
  
  delay(200);
  
}
