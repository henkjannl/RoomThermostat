#include <SPI.h>
#include <TFT_eSPI.h> 

TFT_eSPI tft = TFT_eSPI();

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
const int ledPin = 32;

static const uint8_t BRIGHTNESS[] = { 1,2,4,8,16,32,64,128,255,128,64,32,16,8,4,2 }; 

void setup()   {

  // configure LED PWM functionalitites
  ledcSetup(ledChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledPin, ledChannel);
  
  //Set up the display
  tft.init();
  tft.setRotation(3);
  tft.fillRect(  0,   0, 160, 120, TFT_RED  );
  tft.fillRect(160,   0, 160, 120, TFT_GREEN);
  tft.fillRect(  0, 120, 160, 120, TFT_BLUE );
  tft.fillRect(160, 120, 160, 120, TFT_WHITE);

  tft.fillCircle(160, 120, 60, TFT_DARKGREY );
  tft.fillCircle(160, 120, 40, TFT_LIGHTGREY);
  
  tft.setTextDatum(MC_DATUM);
}

void loop() {
  char buffer [100];

  // increase the LED brightness
  for(auto& brightness : BRIGHTNESS) {
    ledcWrite(ledChannel, brightness);
    snprintf(buffer, 100, "  %03d  ", brightness);
    tft.setTextColor(TFT_BLACK, TFT_RED  );  tft.drawString(buffer, 80, 60, 4);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);  tft.drawString(buffer,240, 60, 4);
    tft.setTextColor(TFT_WHITE, TFT_BLUE );  tft.drawString(buffer, 80,180, 4);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.drawString(buffer,240,180, 4);
    delay(1200);
  }
}
