#pragma once

using namespace std;

// Thermometer dallas DS18B20 connected to OneWire bus
#define ONE_WIRE_BUS 17
#define TEMPERATURE_PRECISION 11 // Resolution 0.125°C. Requires 375 ms to fetch temperature

// Configuration file, containing WiFi access point data
#define CONFIG_FILE "/config.jsn"

// Logging
#define EVENT_LOG   "/eventlog.txt"
#define LOG_FILE    "/logfile.log"

// OpenTherm Shield pins configuration
#define OT_IN_PIN         22 // beware: in and out were flipped due to mistake creating a symbol in KiCad
#define OT_OUT_PIN        21

// Screen backlight dimming
#define PIN_BACKLIGHT      15      
#define BCK_PWM_FREQ     5000      
#define BCK_LED_CHANNEL     0      
#define BCK_RESOLUTION      8

// Screen colors converted to RGB565
#define CLR_BACKGROUND       0xFFFF   // FF, FF, FF = white
#define CLR_CIRCLE_OUTLINE   0xCE59   // C8, C8, C8 = dark grey
#define CLR_CIRCLE_FILL      0xEF5D   // E8, E8, E8 = light grey
#define CLR_MEASURED_TEMP    0x0000   // 00, 00, 00 = black
#define CLR_DARK_TEXT        0x8410   // 80, 80, 80 = dark grey
#define CLR_LIGHT_TEXT       0xB596   // B0, B0, B0 = light grey
#define CLR_SETPOINT_TEMP    0xF8C3   // FF, 19, 19 = red
#define CLR_MENU_TITLE       0x0000   // 00, 00, 00 = black
#define CLR_MENU_SUBTITLE    0x8410   // 80, 80, 80 = dark grey
#define CLR_MENU_TEXT        0x0000   // 00, 00, 00 = black
#define CLR_MENU_LINE        0xF8C3   // FF, 19, 19 = red
#define CLR_BLUE_TEXT        0x1A7F   // 19, 4F, FF = blue
#define CLR_RED_TEXT         0xF8C3   // FF, 19, 19 = red

// Screen fonts
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

/* =================================================
   All datatypes for the thermostat
   ================================================= */

string DAY_NAMES[] ={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };  

enum icon_t {iconNone, iconTempHigher, iconMenu, iconTempLower, iconHome, iconWork, iconWeekend, iconAway, 
  iconAutomatic, iconExit, iconFewer, iconMore, iconSettings, iconHighTemperature, iconLowTemperature, 
  iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays, iconMonday, iconTuesday, iconWednesday, 
  iconThursday, iconFriday, iconSaturday, iconSunday, iconShowerOn, iconShowerOff, iconFlame0, iconFlame1, 
  iconFlame2, iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8, iconWifiConnected, 
  iconWifiLost, iconSetpoint, iconSensorOffset
};

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtAway, dtWeekend, dtAuto};

enum setpointReason_t { spWeekSchedule,  spMultipleDays, spOverrideToday, spOverrideNow };

string DAY_TYPES[] = { "Work from home", "Work from office", "All day away", "Weekend day", "Automatic" };

const icon_t DAYTYPE_TO_ICON[] = { iconHome, iconWork, iconWeekend, iconAway, iconAutomatic };

string ICONS[] = {"/iconNone.jpg", "/iconTempHigher.jpg", "/iconMenu.jpg", "/iconTempLower.jpg", 
  "/iconHome.jpg", "/iconWork.jpg", "/iconWeekend.jpg", "/iconAway.jpg", "/iconAutomatic.jpg", "/iconExit.jpg", 
  "/iconFewer.jpg", "/iconMore.jpg", "/iconSettings.jpg", "/iconHighTemperature.jpg", "/iconLowTemperature.jpg", 
  "/iconClock.jpg", "/iconWeekschedule.jpg", "/iconToday.jpg", "/iconTomorrow.jpg", "/iconMultipleDays.jpg", 
  "/iconMonday.jpg", "/iconTuesday.jpg", "/iconWednesday.jpg", "/iconThursday.jpg", "/iconFriday.jpg", 
  "/iconSaturday.jpg", "/iconSunday.jpg", "/iconShowerOn.jpg", "/iconShowerOff.jpg", "/iconFlame0.jpg", 
  "/iconFlame1.jpg", "/iconFlame2.jpg", "/iconFlame3.jpg", "/iconFlame4.jpg", "/iconFlame5.jpg", 
  "/iconFlame6.jpg", "/iconFlame7.jpg", "/iconFlame8.jpg", "/iconWifiConnected.jpg", "/iconWifiLost.jpg", 
  "/iconSetpoint.jpg", "/iconSensorOffset.jpg"
};

// List of available menus
enum Menus_t {mnHomeScreen, mnMain, mnOverruleDay, mnOverruleMultiple, mnOverruleMultiple2, mnSettings, 
  mnTemperatureSettings, mnSetWeekSchedule, mnSetDaySchedule, mnSetHomeTimes, mnSetOfficeTimes, mnSetWeekendTimes, 
  mnAdjustSensorOffset
};

// List of commands that result from a menu press
enum Commands_t { cmMainMenu, cmOverruleTodayMenu, cmOverruleTomorrowMenu, cmOverruleMultipleMenu, cmSettingsMenu, 
  cmHomeScreen, cmOverruleDayReturn, cmMultipleIncDays, cmMultipleDecDays, cmOverruleMultiple2Menu, 
  cmOverruleMultipleAutomatic, cmOverruleMultipleReturn, cmTemperatureMenu, cmWeekScheduleMenu, cmWorkFromHomeTimesMenu, 
  cmWorkFromOfficeTimesMenu, cmWeekendTimesMenu, cmSensorOffsetMenu, cmHighTempInc, cmHighTempDec, cmLowTempInc, cmLowTempDec, 
  cmOverruleDaySettingMenu, cmOverruleDayScheduleHome, cmOverruleDayScheduleOffice, cmOverruleDayScheduleAway, 
  cmOverruleDayScheduleWeekend, cmHomeWakeUpInc, cmHomeWakeUpDec, cmHomeSleepInc, cmHomeSleepDec, cmOfficeWakeUpInc, 
  cmOfficeWakeUpDec, cmOfficeLeaveInc, cmOfficeLeaveDec, cmOfficeComeHomeInc, cmOfficeComeHomeDec, cmOfficeSleepInc, 
  cmOfficeSleepDec, cmWeekendWakeUpInc, cmWeekendWakeUpDec, cmWeekendSleepInc, cmWeekendSleepDec, cmSensorOffsetInc, 
  cmSensorOffsetDec
};

// List of contexts
enum Context_t { cxNone, cxToday, cxTomorrow, cxMonday, cxTuesday, cxWednesday, cxThursday, cxFriday, cxSaturday, cxSunday };

// Keys on the keyboard
enum Keys_t { kbUp, kbSelect, kbDown };

// Stores hour and minute of changes in the program
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
  
    bool operator> (const time_t& other);
    bool operator> (const timeValue_t& other) const { return minutesSinceMidnight >  other.minutesSinceMidnight; }
    bool operator< (const timeValue_t& other) const { return minutesSinceMidnight <  other.minutesSinceMidnight; }
    bool operator>=(const timeValue_t& other) const { return minutesSinceMidnight >= other.minutesSinceMidnight; }
    bool operator<=(const timeValue_t& other) const { return minutesSinceMidnight <= other.minutesSinceMidnight; }

    string str() {
      char res[16];
      int hour   = minutesSinceMidnight / 60;
      int minute = minutesSinceMidnight % 60;
      snprintf(res, 16, "%d:%02d", hour, minute);
      return string(res);
    };
};

timeValue_t operator+(timeValue_t lhs, const timeValue_t & rhs) { lhs += rhs;  return lhs; };    
timeValue_t operator-(timeValue_t lhs, const timeValue_t & rhs) { lhs -= rhs;  return lhs; };    


// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
};

// TEMPERATURE MEASUREMENT
