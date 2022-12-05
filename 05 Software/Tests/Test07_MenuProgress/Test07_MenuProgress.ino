#include <M5Stack.h>
#include "time.h"
#include "Menu.h"
#include "Constants.h"

Data data;

Menu menu(&data);

/*
  ToDo: 
  incorporate screen saver and wake-up
  permanently load JPEGs in RAM
  perhaps more colors for icons
*/
 
void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(LIGHTGREY);
  delay(500);
  M5.Lcd.fillScreen(WHITE);
  SPIFFS.begin();
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
