#pragma once

#include "time.h"
#include <map>
#include <string>

// Constants that define the keyboard
#define KEYBOARD_TIMER_ID  0      // Number of the keyboard timer (0..3)
#define KEY_THRESHOLD     45      // Level to trigger capacitive keyboard button
#define PIN_BUT_UP        12      // Capacitive pin for up button
#define PIN_BUT_SELECT     4      // Capacitive pin for select button
#define PIN_BUT_DOWN      15      // Capacitive pin for down button

#define CLR_BACKGROUND   0x0000   // 00, 00, 00 = black
#define CLR_BTN_UNSELECT 0xFFF9   // FF, FC, CD = light yellow
#define CLR_BTN_SELECT   0xFF88   // FF, F2, 47 = dark yellow

#define FONT_MENU_HEADER        "Noway_Medium24"
#define FONT_MENU               "Noway_Light24"
#define FONT_SUBMENU            "Noway_Light18"
#define FONT_DATE               "Noway_Light24"
#define FONT_TIME               "Noway_Medium24"
#define FONT_TEMP_MEASURED      "Noway_Medium60"
#define FONT_TEMP_MEASURED_DEG  "Noway_Light32"
#define FONT_TEMP_SETPOINT      "Noway_Medium32"
#define FONT_TEMP_SETPOINT_DEG  "Noway_Light18"
#define FONT_NORMAL             "Noway_Regular24"
#define FONT_MEDIUM             "Noway_Medium24"

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtAway, dtWeekend, dtAuto};

std::string DAY_TYPES[] = { "Work from home", "Work from office", "All day away", "Weekend day", "Automatic" };

enum icon_t {iconNone, iconTempHigher, iconMenu, iconTempLower,
             iconHome, iconWork, iconWeekend, iconAway, iconAutomatic, iconExit,
             iconFewer, iconMore, iconSettings, iconHighTemperature, iconLowTemperature,
             iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays,
             iconMonday, iconTuesday, iconWednesday, iconThursday, iconFriday, iconSaturday,
             iconSunday, iconShowerOn, iconShowerOff, iconFlame0, iconFlame1, iconFlame2,
             iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8
            };

std::string ICONS[] = {"/iconNone.jpg", "/iconTempHigher.jpg", "/iconMenu.jpg",
                       "/iconTempLower.jpg", "/iconHome.jpg", "/iconWork.jpg", "/iconWeekend.jpg",
                       "/iconAway.jpg", "/iconAutomatic.jpg", "/iconExit.jpg", "/iconFewer.jpg",
                       "/iconMore.jpg", "/iconSettings.jpg", "/iconHighTemperature.jpg",
                       "/iconLowTemperature.jpg", "/iconClock.jpg", "/iconWeekschedule.jpg",
                       "/iconToday.jpg", "/iconTomorrow.jpg", "/iconMultipleDays.jpg", "/iconMonday.jpg",
                       "/iconTuesday.jpg", "/iconWednesday.jpg", "/iconThursday.jpg", "/iconFriday.jpg",
                       "/iconSaturday.jpg", "/iconSunday.jpg", "/iconShowerOn.jpg", "/iconShowerOff.jpg",
                       "/iconFlame0.jpg", "/iconFlame1.jpg", "/iconFlame2.jpg", "/iconFlame3.jpg",
                       "/iconFlame4.jpg", "/iconFlame5.jpg", "/iconFlame6.jpg", "/iconFlame7.jpg",
                       "/iconFlame8.jpg"
                      };
                      
std::map<dayType_t, icon_t> DAYTYPE_TO_ICON = { {dtWorkFromHome, iconHome}, 
  {dtWorkAtOffice, iconWork}, {dtAway, iconAway}, {dtWeekend, iconWeekend}, {dtAuto, iconAutomatic} };

// List of available menus
enum Menus_t {mnHomeScreen, mnMain, mnOverruleDay, mnOverruleMultiple, mnOverruleMultiple2, mnSettings, 
  mnTemperatureSettings, mnSetWeekSchedule, mnSetDaySchedule, mnSetHomeTimes, mnSetOfficeTimes, mnSetWeekendTimes 
};

// List of commands that result from a menu press
enum Commands_t { cmMainMenu, cmOverruleTodayMenu, cmOverruleTomorrowMenu, cmOverruleMultipleMenu, cmSettingsMenu, 
  cmHomeScreen, cmOverruleDayReturn, cmMultipleIncDays, cmMultipleDecDays, cmOverruleMultiple2Menu, cmOverruleMultipleAutomatic, 
  cmOverruleMultipleReturn, cmTemperatureMenu, cmWeekScheduleMenu, cmWorkFromHomeTimesMenu, cmWorkFromOfficeTimesMenu, 
  cmWeekendTimesMenu, cmHighTempInc, cmHighTempDec, cmLowTempInc, cmLowTempDec, cmOverruleDaySettingMenu, 
  cmOverruleDayScheduleHome, cmOverruleDayScheduleOffice, cmOverruleDayScheduleAway, cmOverruleDayScheduleWeekend, 
  cmHomeWakeUpInc, cmHomeWakeUpDec, cmHomeSleepInc, cmHomeSleepDec, cmOfficeWakeUpInc, cmOfficeWakeUpDec, cmOfficeLeaveInc, 
  cmOfficeLeaveDec, cmOfficeComeHomeInc, cmOfficeComeHomeDec, cmOfficeSleepInc, cmOfficeSleepDec, cmWeekendWakeUpInc, 
  cmWeekendWakeUpDec, cmWeekendSleepInc, cmWeekendSleepDec
};

// List of contexts
enum Context_t { cxNone, cxToday, cxTomorrow, cxMonday, cxTuesday, cxWednesday, cxThursday, cxFriday, cxSaturday, cxSunday };

class timeValue_t {
  public:
    // Constructors
    timeValue_t() { minutesSinceMidnight = 0; };
    timeValue_t(int hour, int minute) { minutesSinceMidnight=60*hour+minute; };
    timeValue_t(const time_t& source) {
      struct tm * otherTime;
      otherTime = localtime(&source);
      minutesSinceMidnight=60*otherTime->tm_hour+otherTime->tm_min;
    };
    
    int minutesSinceMidnight;
    
    timeValue_t& operator+=(const timeValue_t& other){ minutesSinceMidnight+=other.minutesSinceMidnight; return *this; };
    timeValue_t& operator-=(const timeValue_t& other){ minutesSinceMidnight-=other.minutesSinceMidnight; return *this; };
  
    bool operator> (const time_t&      other);
    bool operator> (const timeValue_t& other) const { return minutesSinceMidnight >  other.minutesSinceMidnight; }
    bool operator< (const timeValue_t& other) const { return minutesSinceMidnight <  other.minutesSinceMidnight; }
    bool operator>=(const timeValue_t& other) const { return minutesSinceMidnight >= other.minutesSinceMidnight; }
    bool operator<=(const timeValue_t& other) const { return minutesSinceMidnight <= other.minutesSinceMidnight; }

    std::string str() {
      char res[16];
      int hour   = minutesSinceMidnight / 60;
      int minute = minutesSinceMidnight % 60;
      snprintf(res, 16, "%d:%02d", hour, minute);
      return std::string(res);
    };
};

timeValue_t operator+(timeValue_t lhs, const timeValue_t & rhs) { lhs += rhs;  return lhs; };    
timeValue_t operator-(timeValue_t lhs, const timeValue_t & rhs) { lhs -= rhs;  return lhs; };    

class Data {
  public:
    Data() { setIcons(); };
    
    // Settings with permanent character (to be saved in JSON)
    bool overrideTempNow = false;
    float overrideTemp = 21.0;

    dayType_t overrideToday     = dtAuto;
    dayType_t overrideTomorrow  = dtAuto;
    dayType_t overrideMultiple  = dtAuto;
    int overrideMultipleCount   = 3;

    //                           Sunday     Monday          Tuesday         Wednesday       Thursday        Friday          Saturday  
    dayType_t regularWeek[7] = { dtWeekend, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWeekend }; // Sunday is day 0

    timeValue_t workFromHomeWakeUp = {  7, 00 };
    timeValue_t workFromHomeSleep  = { 22, 00 };

    timeValue_t workAtOfficeWakeUp = {  7, 00 };
    timeValue_t workAtOfficeGoOut  = {  8, 00 };
    timeValue_t workAtOfficeComeIn = { 17, 00 };
    timeValue_t workAtOfficeSleep  = { 22, 00 };

    timeValue_t weekendWakeUp      = {  9, 00 };
    timeValue_t weekendSleep       = { 23, 00 };

    float highTemp = 21.0;
    float lowTemp  = 10.0;

    // Settings which are derived from other data
    time_t now;
    float measuredTemperature = 21.4;
    float setpointTemperature = 21.0;
    bool showerRunning = true;
    float boilerPower = 0.25;
    enum icon_t icons[7] = { iconHome, iconWork, iconWeekend, iconWeekend, iconAway, iconAway, iconAway };

    void setIcons();
  private:

};

void Data::setIcons() {
  struct tm * lctm = localtime(&now);
  Serial.printf ( "The current date/time is: %s\n", asctime (lctm) );
  uint8_t dayOfWeek = lctm->tm_wday;
  dayType_t daytype;

  // By default, set the icons according to the weekly schedule
  for(int i=0; i<7; i++) {
    daytype=regularWeek[dayOfWeek];  // 0=Sunday
    icons[i]=DAYTYPE_TO_ICON[daytype];
    dayOfWeek++;
    if(dayOfWeek>6) dayOfWeek=0;
  };

  // Override multiple days if needed
  if(overrideMultiple!=dtAuto) {
    for(int i=0; i<overrideMultipleCount; i++) {
      if(i<7) icons[i]=DAYTYPE_TO_ICON[overrideMultiple];
    };  
  };

  // Override today if needed
  if(overrideToday!=dtAuto) {
    icons[0]=DAYTYPE_TO_ICON[overrideToday];
  }

  // Override tomorrow if needed
  if(overrideTomorrow!=dtAuto) {
    icons[1]=DAYTYPE_TO_ICON[overrideTomorrow];
  }

};
