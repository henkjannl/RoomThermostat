#pragma once

#include <list>
#include <map>
#include <string>
#include "time.h"
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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
//#define KEYBOARD_TIMER_ID  1      // Number of the timer used for the keys (0..3)
#define KEY_THRESHOLD     50      // Level to trigger capacitive keyboard button
#define PIN_BTN_UP        12      // Capacitive pin for up button
#define PIN_BTN_SELECT     4      // Capacitive pin for select button
#define PIN_BTN_DOWN      15      // Capacitive pin for down button

// Screen colors
#define CLR_BACKGROUND       0xFFFF   // FF, FF, FF = white
#define CLR_CIRCLE_OUTLINE   0xFD2A   // FF, A5, 55 = dark salmon
#define CLR_CIRCLE_FILL      0xFE11   // FF, C0, 88 = light salmon
#define CLR_MEASURED_TEMP    0x0000   // 00, 00, 00 = black
#define CLR_DARK_TEXT        0x8410   // 80, 80, 80 = dark grey
#define CLR_LIGHT_TEXT       0xB596   // B0, B0, B0 = light grey
#define CLR_SETPOINT_TEMP    0xF800   // FF, 00, 00 = red
#define CLR_MENU_TITLE       0x0000   // 00, 00, 00 = black
#define CLR_MENU_SUBTITLE    0x8410   // 80, 80, 80 = dark grey
#define CLR_MENU_TEXT        0x0000   // 00, 00, 00 = black
#define CLR_MENU_LINE        0xF800   // FF, 00, 00 = red

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
  iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays, iconMonday, iconTuesday, 
  iconWednesday, iconThursday, iconFriday, iconSaturday, iconSunday, iconShowerOn, iconShowerOff, iconFlame0, 
  iconFlame1, iconFlame2, iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8, 
  iconWifiConnected, IconWifiLost, iconSetpoint };


std::string ICONS[] = {"/iconNone.jpg", "/iconTempHigher.jpg", "/iconMenu.jpg", "/iconTempLower.jpg", "/iconHome.jpg", 
  "/iconWork.jpg", "/iconWeekend.jpg", "/iconAway.jpg", "/iconAutomatic.jpg", "/iconExit.jpg", "/iconFewer.jpg", 
  "/iconMore.jpg", "/iconSettings.jpg", "/iconHighTemperature.jpg", "/iconLowTemperature.jpg", "/iconClock.jpg", 
  "/iconWeekschedule.jpg", "/iconToday.jpg", "/iconTomorrow.jpg", "/iconMultipleDays.jpg", "/iconMonday.jpg", 
  "/iconTuesday.jpg", "/iconWednesday.jpg", "/iconThursday.jpg", "/iconFriday.jpg", "/iconSaturday.jpg", 
  "/iconSunday.jpg", "/iconShowerOn.jpg", "/iconShowerOff.jpg", "/iconFlame0.jpg", "/iconFlame1.jpg", "/iconFlame2.jpg", 
  "/iconFlame3.jpg", "/iconFlame4.jpg", "/iconFlame5.jpg", "/iconFlame6.jpg", "/iconFlame7.jpg", "/iconFlame8.jpg", 
  "/iconWifiConnected.jpg", "/iconWifiLost.jpg", "/iconSetpoint.jpg" };

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
enum controllerRequest_t { crRequestSetpoint };

// Answers from the main program to the controller
struct answerToController {
  float measuredTemperature;
  float temperatureSetpoint;
};

// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
  float lat, lon;
};

// All data in the thermostat
class Data {
  public:
    Data() { setIcons(); };
    void Load(fs::FS &fs, const char * configFile);

    // List of WiFi access points that can be accessed
    std::list<tAccessPoint> AccessPoints;
    bool locationKnown=false;            
    bool timeSynched=false;
    char timezone[64];
    float lat, lon;
    int timeZoneOffset;
    int timeDSToffset;

    // Dallas temperature sensor
    DallasTemperature *sensor;
    DeviceAddress deviceAddress; 
    void initThermometer(DallasTemperature& sensor);
    void measureTemperature();

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

    // Values which are derived from other data
    float measuredTemperature = 21.4;
    float setpointTemperature = 21.0;
    
    bool showerRunning = false;
    bool wifiConnected = false;
    float boilerPower = 0.6;
    enum icon_t icons[7] = { iconHome, iconWork, iconWeekend, iconWeekend, iconAway, iconAway, iconAway };

    void setIcons();
    void setSetpoint();
  private:

};

void Data::initThermometer(DallasTemperature& s) {
  sensor=&s;
  sensor->begin();
  Serial.printf("%d device(s) found on OneWire bus\n", sensor->getDeviceCount());
  Serial.printf("Parasite power on OneWire bus is: %s\n", sensor->isParasitePowerMode() ? "ON": "OFF");
  if (!sensor->getAddress(deviceAddress, 0)) {
    Serial.println("Unable to find address for device 0");
  }
  else {
    Serial.print("Dallas 18B20 found on address:\t");
    for (uint8_t i = 0; i < 8; i++) Serial.printf("%02X ", deviceAddress[i]);
    Serial.println();
  }

  // set the resolution to 9 bit per device
  sensor->setResolution(deviceAddress, TEMPERATURE_PRECISION);
  Serial.printf("Resolution of sensor set to: %d\n",sensor->getResolution(deviceAddress));
  
  measureTemperature();
};

void Data::measureTemperature() {
  sensor->requestTemperatures();
  measuredTemperature = sensor->getTempC(deviceAddress);
  //Serial.print("Device address:\t");
  //for (uint8_t i = 0; i < 8; i++) Serial.printf("%02X", deviceAddress[i]);
  Serial.printf("Measured temperature:\t%.3f\n", measuredTemperature);
};

void Data::setIcons() {
  time_t now;
  time(&now); // Get current time
  struct tm * lctm = localtime(&now); // Convert time to local time
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

void Data::setSetpoint() {
  time_t now;
  struct tm * timeinfo;
  time(&now);
  timeinfo = localtime(&now);  
  Serial.println(timeinfo->tm_hour);
};

void Data::Load(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  File input = fs.open(configFile);
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
    AccessPoint.lat = elem["lat"];            // Lattitude of location for each access point
    AccessPoint.lon = elem["lon"];            // Longitude of location for each access point
    AccessPoints.push_back(AccessPoint);  
  }
};
