#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>
#include "SPIFFS.h"
#include <FS.h>
//#include "esp_system.h"
#include <list>
#include <map>
#include <string>
#include <cmath>

//using namespace std;

// Use core 1, so WiFi can use core 0
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Thermometer dallas DS18B20 connected to OneWire bus
#define ONE_WIRE_BUS 17
#define TEMPERATURE_PRECISION 11 // Resolution 0.125°C. Requires 375 ms to fetch temperature

// Configuration file, containing WiFi access point data
#define CONFIG_FILE       "/config.jsn"

// Constants that define the keyboard
#define KEY_THRESHOLD     50      // Level to trigger capacitive keyboard button
#define PIN_BTN_UP        12      // Capacitive pin for up button
#define PIN_BTN_SELECT     4      // Capacitive pin for select button
#define PIN_BTN_DOWN      15      // Capacitive pin for down button

// OpenTherm Shield pins configuration
#define OT_IN_PIN         21
#define OT_OUT_PIN        22

//#define OT_IN_PIN         22
//#define OT_OUT_PIN        21

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
enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtAway, dtWeekend, dtAuto};

std::string DAY_TYPES[] = { "Work from home", "Work from office", "All day away", "Weekend day", "Automatic" };

enum icon_t {iconNone, iconTempHigher, iconMenu, iconTempLower, iconHome, iconWork, iconWeekend, iconAway, 
  iconAutomatic, iconExit, iconFewer, iconMore, iconSettings, iconHighTemperature, iconLowTemperature, 
  iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays, iconMonday, iconTuesday, iconWednesday, 
  iconThursday, iconFriday, iconSaturday, iconSunday, iconShowerOn, iconShowerOff, iconFlame0, iconFlame1, 
  iconFlame2, iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8, iconWifiConnected, 
  iconWifiLost, iconSetpoint, iconSensorOffset
};

std::string ICONS[] = {"/iconNone.jpg", "/iconTempHigher.jpg", "/iconMenu.jpg", "/iconTempLower.jpg", 
  "/iconHome.jpg", "/iconWork.jpg", "/iconWeekend.jpg", "/iconAway.jpg", "/iconAutomatic.jpg", "/iconExit.jpg", 
  "/iconFewer.jpg", "/iconMore.jpg", "/iconSettings.jpg", "/iconHighTemperature.jpg", "/iconLowTemperature.jpg", 
  "/iconClock.jpg", "/iconWeekschedule.jpg", "/iconToday.jpg", "/iconTomorrow.jpg", "/iconMultipleDays.jpg", 
  "/iconMonday.jpg", "/iconTuesday.jpg", "/iconWednesday.jpg", "/iconThursday.jpg", "/iconFriday.jpg", 
  "/iconSaturday.jpg", "/iconSunday.jpg", "/iconShowerOn.jpg", "/iconShowerOff.jpg", "/iconFlame0.jpg", 
  "/iconFlame1.jpg", "/iconFlame2.jpg", "/iconFlame3.jpg", "/iconFlame4.jpg", "/iconFlame5.jpg", 
  "/iconFlame6.jpg", "/iconFlame7.jpg", "/iconFlame8.jpg", "/iconWifiConnected.jpg", "/iconWifiLost.jpg", 
  "/iconSetpoint.jpg", "/iconSensorOffset.jpg"
};

std::map<dayType_t, icon_t> DAYTYPE_TO_ICON = { {dtWorkFromHome, iconHome}, 
  {dtWorkAtOffice, iconWork}, {dtAway, iconAway}, {dtWeekend, iconWeekend}, {dtAuto, iconAutomatic} };

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

// Reqests from the controller to the main program
struct MessageControllerToMain {
  float measuredTemperature        = 12.3;  
  float boilerPercentage           =  0.6;
  float requestedBoilerTemperature = 60.0;
  
  float P,I,D;
  
  bool isCentralHeatingActive   = false;
  bool isHotWaterActive         = false;
  bool isFlameOn                = false;
  float actualBoilerTemperature = 40.0;
  char responseStatus[12]       = "NONE";
};

// Answers from the main program to the controller
struct MessageMainToController {
  float setpointTemperature;
  float measuredTemperatureOffset;
};

// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
};

// All data in the thermostat
class Data {
  public:
    Data() { setSetpoint(); };

    // List of WiFi access points that can be accessed
    std::list<tAccessPoint> AccessPoints;
    bool locationKnown=false;            
    bool timeSynched=false;
    char timezone[64]="Europe/Amsterdam";
    int timeZoneOffset=3600;
    int timeDSToffset=0;

    // Regular weekly schedule
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

    // Screen and messages
    bool forceScreenUpdate=true;
    bool forceMessageToController=true;

    // Current settings of temperature
    bool overrideTempNow = false;
    float overrideTemp = 21.0;

    dayType_t overrideToday     = dtAuto;
    dayType_t overrideTomorrow  = dtAuto;
    dayType_t overrideMultiple  = dtAuto;
    int overrideMultipleCount   = 3;

    // Measuring and controlling the temperature
    float measuredTemperatureOffset = -2.6; // Actual temperature = raw sensor signal + offset
    float setpointTemperature = 21.0;
    time_t prevSetpointCalculationTime;

    MessageControllerToMain messageFromController;
    
    bool wifiConnected = false;
    enum icon_t icons[7] = { iconHome, iconWork, iconWeekend, iconWeekend, iconAway, iconAway, iconAway };

    // Telegram bot settings
    String botName;
    String botUserName;
    String botToken;
    String botChatID;

    void loadConfig(fs::FS &fs, const char * configFile);
    void setIcons();
    void setSetpoint();
  private:

};

void Data::loadConfig(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  //File file = SD.open(filename);
  //File input = fs.open(configFile);
  fs::File input = fs.open(configFile);
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    Serial.print(F("deserializeJson() of configfile failed: "));
    Serial.println(error.f_str());
    return;
  }
      
  for (JsonObject elem : doc["AccessPoints"].as<JsonArray>()) {
    tAccessPoint AccessPoint;
    strlcpy(AccessPoint.ssid     ,elem["SSID"],    sizeof(AccessPoint.ssid    ));
    strlcpy(AccessPoint.password ,elem["password"],sizeof(AccessPoint.password));
    strlcpy(AccessPoint.timezone ,elem["timezone"],sizeof(AccessPoint.timezone));          
    AccessPoints.push_back(AccessPoint);  
  }

  botName=    doc["BotName"    ].as<String>();
  botUserName=doc["BotUsername"].as<String>();
  botToken=   doc["BotToken"   ].as<String>();
  botChatID=  doc["ChatID"     ].as<String>();

  Serial.println("Config loaded");  
};

void Data::setIcons() {
  time_t now;
  time(&now); // Get current time
  struct tm * lctm = localtime(&now); // Convert time to local time

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

void Data::setSetpoint() {
  struct tm * timeinfo;
  dayType_t currDayType;
  uint8_t dayOfWeek;
  float previousSetpointTemperature = setpointTemperature;
  
  // Day and time of previous setpoint calculation
  timeinfo = localtime(&prevSetpointCalculationTime);
  int prevDay = timeinfo->tm_yday;
  timeValue_t prevTime(timeinfo->tm_hour, timeinfo->tm_min);

  // Current day and time
  time_t now;
  time(&now);
  timeinfo = localtime(&now);
  int currDay = timeinfo->tm_yday;
  timeValue_t currTime(timeinfo->tm_hour, timeinfo->tm_min);

  // Shift days at midnight
  if(currDay != prevDay) {
    overrideToday = overrideTomorrow;
    overrideTomorrow=dtAuto;
    
    if (overrideMultiple!=dtAuto) {
      if(overrideMultipleCount>0)
        overrideMultipleCount--;
      else
        overrideMultiple=dtAuto;
    }
    forceMessageToController=true;
  }

  // Determine what kind of day we have today
  dayOfWeek = timeinfo->tm_wday;
  currDayType=regularWeek[dayOfWeek];
  if ( (overrideMultiple!=dtAuto) and (overrideMultipleCount>0) ) currDayType=overrideMultiple;
  if(overrideToday!=dtAuto) currDayType=overrideToday;

  // Check if we just passed a switch moment
  switch(currDayType) {
    case dtWorkFromHome:
      if( (prevTime<workFromHomeWakeUp) and (workFromHomeWakeUp<currTime) ) overrideTempNow=false;
      if( (prevTime<workFromHomeSleep ) and (workFromHomeSleep <currTime) ) overrideTempNow=false;
      if(!overrideTempNow) {
        setpointTemperature = lowTemp;
        if( (workFromHomeWakeUp<=currTime) and (currTime<workFromHomeSleep) ) setpointTemperature = highTemp;
      }
    break;

    case dtWorkAtOffice:
      if( (prevTime<workAtOfficeWakeUp) and (workAtOfficeWakeUp<currTime) ) overrideTempNow=false;
      if( (prevTime<workAtOfficeGoOut ) and (workAtOfficeGoOut<currTime ) ) overrideTempNow=false;
      if( (prevTime<workAtOfficeComeIn) and (workAtOfficeComeIn<currTime) ) overrideTempNow=false;
      if( (prevTime<workAtOfficeSleep ) and (workAtOfficeSleep<currTime ) ) overrideTempNow=false;
      if(!overrideTempNow) {
        setpointTemperature = lowTemp;
        if( (workAtOfficeWakeUp<=currTime) and (currTime<workAtOfficeGoOut) ) setpointTemperature = highTemp;
        if( (workAtOfficeComeIn<=currTime) and (currTime<workAtOfficeSleep) ) setpointTemperature = highTemp;
      }
      
    break;

    case dtAway:
      if(currDay != prevDay) overrideTempNow=false;
      if(!overrideTempNow) setpointTemperature = lowTemp;
    break;

    case dtWeekend:
      if( (prevTime<weekendWakeUp) and (weekendWakeUp<currTime) ) overrideTempNow=false;
      if( (prevTime<weekendSleep ) and (weekendSleep<currTime ) ) overrideTempNow=false;
      if(!overrideTempNow) {
        setpointTemperature = lowTemp;
        if( (weekendWakeUp<=currTime) and (currTime<weekendSleep) ) setpointTemperature = highTemp;
      }
    break;
  }

  //Serial.printf("%s > %s > t=%.1f°C\n", currTime.str().c_str(), DAY_TYPES[currDayType].c_str(), setpointTemperature);
  if(!overrideTempNow) overrideTemp = 0.5*round(2*messageFromController.measuredTemperature);    
  if(previousSetpointTemperature != setpointTemperature) forceMessageToController=true;

  prevSetpointCalculationTime=now;
};
