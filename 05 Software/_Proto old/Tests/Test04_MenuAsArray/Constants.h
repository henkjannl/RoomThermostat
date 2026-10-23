#pragma once

#define CLR_BACKGROUND   0x0000   // 00, 00, 00 = black
#define CLR_BTN_UNSELECT 0xFFF9   // FF, FC, CD = light yellow
#define CLR_BTN_SELECT   0xFF88   // FF, F2, 47 = dark yellow

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
