#include <M5Stack.h>
#include "time.h"
#include "Menu.h"
#include "Constants.h"

Data data;

Menu menu(&data);

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(LIGHTGREY);
  delay(500);
  M5.Lcd.fillScreen(WHITE);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS available!");

  menu.drawScreen();

}

void loop() {

  M5.update(); 
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
    menu.buttonDown();
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    menu.buttonSelect();
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
    menu.buttonUp();
  };

}
