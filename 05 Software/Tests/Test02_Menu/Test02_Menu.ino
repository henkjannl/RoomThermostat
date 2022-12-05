#include <M5Stack.h>
#include "time.h"
#include "Free_Fonts.h"
#include <list>

#define CLR_BACKGROUND   0x0000   // 00, 00, 00 = black
#define CLR_BTN_UNSELECT 0xFFF9   // FF, FC, CD = light yellow
#define CLR_BTN_SELECT   0xFF83   // FF, F1, 19 = dark yellow

TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd);


static const char* MENU_ITEMS[] = { "Overrule today", "Overrule tomorrow", "Overrule multiple", "Settings" };
int NUM_ITEMS=4;

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtAway, dtWeekend, dtAuto};

class TemperatureSettings {
   public:
      dayType_t overrideToday;
      float overrideTemperatureToday;
      dayType_t overrideTomorrow;
      dayType_t overrideMultiple;
      int overrideMultipleCount;
      dayType_t regularWeek[7]; // Sunday is day 0 

      float workFromHomeWakeUp;
      float workFromHomeSleep;

      float workAtOfficeWakeUp;
      float workAtOfficeGoOut;
      float workAtOfficeComeIn;
      float workAtOfficeSleep;

      float weekendWakeUp;
      float weekendSleep;

      float highTemp = 21.0;
      float lowTemp  = 10.0;

  private:
};

#define FORMAT_SPIFFS_IF_FAILED FALSE

void setup() {
  M5.begin(); 
  M5.Power.begin(); 
  M5.Lcd.fillScreen(WHITE); 
}

void drawButtons(int select) {
  int y=2;
  int dy=32;

  tftSprite.createSprite(224, dy);
  tftSprite.setTextDatum(MC_DATUM);
  tftSprite.setFreeFont(FF18);
  tftSprite.fillSprite(WHITE);    
  
  for(int i=0; i<NUM_ITEMS; i++) {
    
    tftSprite.fillRoundRect(0, 0, 224, dy, 8, i==select ? CLR_BTN_SELECT : CLR_BTN_UNSELECT);
    tftSprite.setTextColor(TFT_BLACK, i==select ? CLR_BTN_SELECT : CLR_BTN_UNSELECT);
    tftSprite.drawString(MENU_ITEMS[i], 112, 15);
    tftSprite.pushSprite(48, y);
    Serial.printf("%s y=%d w=%d h=%d\n", MENU_ITEMS[i], y, tftSprite.textWidth(MENU_ITEMS[i]), tftSprite.fontHeight());
    y+=dy+2;
  }
  tftSprite.deleteSprite();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int sel=0; sel<NUM_ITEMS; sel++) {
    drawButtons(sel);
    delay(500);
  }

}
