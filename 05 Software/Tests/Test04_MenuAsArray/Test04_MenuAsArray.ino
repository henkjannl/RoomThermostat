#include <M5Stack.h>
#include "time.h"
#include "Free_Fonts.h"
#include "Menu.h"
#include "Constants.h"

Menu menu;

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(LIGHTGREY);
  delay(500);
  M5.Lcd.fillScreen(WHITE);

  menu.drawMenu();
  
/*
  Serial.println("\nMain menu:");
  for(const menuItem_t &menuItem : menuStructure[mnMain].menuItems) Serial.printf("  - %s\n",menuItem.label);

  Serial.println("\nSet week schedule:");
  for(const menuItem_t &menuItem : menuStructure[mnSetWeekSchedule].menuItems) Serial.printf("  - %s\n",menuItem.label);
*/

}

void loop() {

  M5.update(); 
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(1000, 200)) {
    menu.buttonUp();
  } else if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(1000, 200)) {
    menu.buttonSelect();
  } else if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(1000, 200)) {
    menu.buttonDown();
  };

}
