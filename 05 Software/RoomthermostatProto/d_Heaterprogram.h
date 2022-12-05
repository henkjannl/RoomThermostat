#pragma once

#include <list>
#include <map>
#include <string>
#include <cmath>
#include "time.h"
#include <ArduinoJson.h>
#include <OpenTherm.h>

#include "a_Constants.h"
#include "c_Eventlog.h"

using namespace std;

// All data in the thermostat
// most importantly the switching of the temperature setpoint
class Data {
  public:
    Data() { setSetpoint(); };

    // WIFI ACCESS POINT INFORMATION
    list<tAccessPoint> AccessPoints;
    bool locationKnown=false;            
    bool timeSynched=false;
    char timezone[64]="Europe/Amsterdam";
    int  timeZoneOffset=3600;
    int  timeDSToffset=0;
    bool wifiConnected = false;

    // PROGAM TO SWITCH THE TEMPERATURE SETPOINT
    // Regular weekly schedule (Sunday is day 0)
    //                           Sunday     Monday          Tuesday         Wednesday       Thursday        Friday          Saturday  
    dayType_t regularWeek[7] = { dtWeekend, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWeekend }; 

    timeValue_t workFromHomeWakeUp = {  6, 45 };
    timeValue_t workFromHomeSleep  = { 21, 30 };

    timeValue_t workAtOfficeWakeUp = {  6, 45 };
    timeValue_t workAtOfficeGoOut  = {  7, 30 };
    timeValue_t workAtOfficeComeIn = { 17, 00 };
    timeValue_t workAtOfficeSleep  = { 21, 30 };

    timeValue_t weekendWakeUp      = {  9, 00 };
    timeValue_t weekendSleep       = { 22, 30 };

    float highTemp = 19.5;
    float lowTemp  = 10.0;

    // Current settings of temperature
    dayType_t overrideToday     = dtAuto;
    dayType_t overrideTomorrow  = dtAuto;
    dayType_t overrideMultiple  = dtAuto;
    int overrideMultipleCount   = 3;

    bool overrideTempNow = false;
    float overrideSetpoint = 19.5;
    setpointReason_t reasonForSetpoint;

    //bool forceMessageToController=true;

    // TEMPERATURE MEASUREMENT
    // Measuring and controlling the temperature
    float measuredTemperatureOffset = -2.6; // Actual temperature = raw sensor signal + offset

    // TEMPERATURE CONTROL
    float temperatureSetpoint = 21.0;
    float domesticHotWaterSetpoint = 55.0;
    float actualDomesticHotWaterTemperature;
    float measuredTemperature;
    float boilerPercentage;
    float requestedBoilerTemperature;
    float P;
    float I;
    float D;
    float ierr;
    float derivative;
    bool isCentralHeatingActive;
    bool isHotWaterActive;
    bool isFlameOn;
    float actualBoilerTemperature;
    String responseFromController="Not started yet";
    
    bool enableCentralHeating = true;
    bool enableHotWater = true;
    bool enableCooling = false;
    
    // DISPLAY OF THE PHYSICAL DEVICE
    // Screen and messages
    bool forceScreenUpdate=true;

    enum icon_t icons[7] = { iconHome, iconWork, iconWeekend, iconWeekend, iconAway, iconAway, iconAway };

    // TELEGRAM
    // Telegram bot settings
    String botName;
    String botUserName;
    String botToken;
    String botChatID;

    void loadConfig(fs::FS &fs, const char * configFile);
    void setIcons();
    void setSetpoint();

    // Various handlers for the user interface, both device and Telegram
    void setpointLower();
    void setpointAuto();
    void setpointHigher();
    void goAway();
    void overruleTodayWorkFromHome();
    void overruleTodayWorkFromOffice();
    void overruleTodayWeekend();
    void overruleTodayAway();
    void overruleTodayAutomatic();
    void overruleTomorrowWorkFromHome();
    void overruleTomorrowWorkFromOffice();
    void overruleTomorrowWeekend();
    void overruleTomorrowAway();
    void overruleTomorrowAutomatic();
    void overruleMultipleWorkFromHome();
    void overruleMultipleWorkFromOffice();
    void overruleMultipleWeekend();
    void overruleMultipleAway();
    void overruleMultipleAutomatic();
    void overruleMultipleLessDays();
    void overruleMultipleMoreDays();
    void setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay);
    void workFromHomeWakeUpEarlier();
    void workFromHomeWakeUpLater();
    void workFromHomeSleepEarlier();
    void workFromHomeSleepLater();
    void workFromOfficeWakeUpEarlier();
    void workFromOfficeWakeUpLater();
    void workFromOfficeGoOutEarlier();
    void workFromOfficeGoOutLater();
    void workFromOfficeComeInEarlier();
    void workFromOfficeComeInLater();
    void workFromOfficeSleepEarlier();
    void workFromOfficeSleepLater();
    void weekendWakeUpEarlier();
    void weekendWakeUpLater();
    void weekendSleepEarlier();
    void weekendSleepLater();
    void highTempLower();
    void highTempHigher();
    void lowTempLower();
    void lowTempHigher();

  private:

};

void Data::loadConfig(fs::FS &fs, const char * configFile) {    
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
         int currDay;
  static int prevDay=0;
         timeValue_t currTime;
  static timeValue_t prevTime = timeValue_t(0,0);
  struct tm * timeinfo;
  dayType_t currDayType;
  uint8_t dayOfWeek;
  
  // Current day and time
  time_t now;
  time(&now);
  timeinfo = localtime(&now);
  currDay = timeinfo->tm_yday;
  currTime=timeValue_t(timeinfo->tm_hour, timeinfo->tm_min);
  dayOfWeek = timeinfo->tm_wday;

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
  }

  // Determine what kind of day we have today
  currDayType=regularWeek[dayOfWeek];
  reasonForSetpoint = spWeekSchedule;

  if ( (overrideMultiple!=dtAuto) and (overrideMultipleCount>0) ) {
    currDayType=overrideMultiple;
    reasonForSetpoint = spMultipleDays;
  }

  if(overrideToday!=dtAuto) {
    currDayType=overrideToday;
    reasonForSetpoint = spOverrideToday;
  }

  // Check if we just passed a switch moment
  switch(currDayType) {

    case dtWorkFromHome:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<workFromHomeWakeUp) and (workFromHomeWakeUp<=currTime) ) { overrideTempNow=false; logMessage("WorkFromHomeWakeUp"); }
      else if( (prevTime<workFromHomeSleep ) and (workFromHomeSleep <=currTime) ) { overrideTempNow=false; logMessage("WorkFromHomeSleep" ); };
      
      if(!overrideTempNow) {
        temperatureSetpoint = lowTemp;
        if( (workFromHomeWakeUp<=currTime) and (currTime<workFromHomeSleep) ) temperatureSetpoint = highTemp;
      }
    break;

    case dtWorkAtOffice:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<workAtOfficeWakeUp) and (workAtOfficeWakeUp<=currTime) ) { overrideTempNow=false; logMessage("WorkFromOfficeWakeUp"); }
      else if( (prevTime<workAtOfficeGoOut ) and (workAtOfficeGoOut <=currTime) ) { overrideTempNow=false; logMessage("WorkFromOfficeGoOut" ); }
      else if( (prevTime<workAtOfficeComeIn) and (workAtOfficeComeIn<=currTime) ) { overrideTempNow=false; logMessage("WorkFromOfficeComeIn"); }
      else if( (prevTime<workAtOfficeSleep ) and (workAtOfficeSleep <=currTime) ) { overrideTempNow=false; logMessage("WorkFromOfficeSleep" ); };
      
      if(!overrideTempNow) {
        temperatureSetpoint = lowTemp;
        if     ( (workAtOfficeWakeUp<=currTime) and (currTime<workAtOfficeGoOut) ) temperatureSetpoint = highTemp;
        else if( (workAtOfficeComeIn<=currTime) and (currTime<workAtOfficeSleep) ) temperatureSetpoint = highTemp;
      }      
    break;

    case dtAway:
      if(currDay != prevDay) { overrideTempNow=false; logMessage("Day away next day" ); };

      if(!overrideTempNow) temperatureSetpoint = lowTemp;
    break;

    case dtWeekend:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<weekendWakeUp) and (weekendWakeUp<=currTime) ) { overrideTempNow=false; logMessage("WeekendWakeUp"); }
      else if( (prevTime<weekendSleep ) and (weekendSleep <=currTime) ) { overrideTempNow=false; logMessage("WeekendSleep" ); };
      
      if(!overrideTempNow) {
        temperatureSetpoint = lowTemp;
        if( (weekendWakeUp<=currTime) and (currTime<weekendSleep) ) temperatureSetpoint = highTemp;
      }
    break;
  }

  //Serial.printf("%s > %s > t=%.1f°C\n", currTime.str().c_str(), DAY_TYPES[currDayType].c_str(), temperatureSetpoint);
  if(overrideTempNow) {
    reasonForSetpoint = spOverrideNow;
    temperatureSetpoint=overrideSetpoint;
  } else {
    overrideSetpoint = int(temperatureSetpoint+0.5);
  }

  // Remember for the next calclation
  prevDay=currDay;
  prevTime=currTime;
};

void Data::setpointLower() { 
  if(!overrideTempNow) {
    overrideSetpoint = 0.5*round(2*measuredTemperature-0.5);
  }
  else {
    if(overrideSetpoint>0) overrideSetpoint-=0.5;
  }
  overrideTempNow=true;
  setSetpoint();
};

void Data::setpointHigher() {
  if(!overrideTempNow) {
    overrideSetpoint= 0.5*round(2*measuredTemperature+0.5);
  }
  else {
    if(overrideSetpoint<30) overrideSetpoint+=0.5;
  }
  overrideTempNow=true;
  setSetpoint();
};

void Data::setpointAuto() { 
  overrideTempNow=false;
  setSetpoint();
};

void Data::goAway() { 
  overrideToday=dtAway;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTodayWorkFromHome() { 
  overrideToday=dtWorkFromHome;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTodayWorkFromOffice() { 
  overrideToday=dtWorkAtOffice;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTodayWeekend() {
  overrideToday=dtWeekend;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTodayAway() { 
  overrideToday=dtAway;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTodayAutomatic() { 
  overrideToday=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleTomorrowWorkFromHome() { 
  overrideTomorrow=dtWorkFromHome;
  setSetpoint();
};

void Data::overruleTomorrowWorkFromOffice() { 
  overrideTomorrow=dtWorkAtOffice;
  setSetpoint();
};

void Data::overruleTomorrowWeekend() { 
  overrideTomorrow=dtWeekend;
  setSetpoint();
};

void Data::overruleTomorrowAway() { 
  overrideTomorrow=dtAway;
  setSetpoint();
};

void Data::overruleTomorrowAutomatic() { 
  overrideTomorrow= dtAuto;
  setSetpoint();
};

void Data::overruleMultipleWorkFromHome() { 
  overrideMultiple=dtWorkFromHome;
  overrideToday=dtAuto;
  overrideTomorrow=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleMultipleWorkFromOffice() { 
  overrideMultiple=dtWorkAtOffice;
  overrideTomorrow=dtAuto;
  overrideToday=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleMultipleWeekend() { 
  overrideMultiple=dtWeekend;
  overrideTomorrow=dtAuto;
  overrideToday=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleMultipleAway() { 
  overrideMultiple=dtAway;
  overrideTomorrow=dtAuto;
  overrideToday=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleMultipleAutomatic() { 
  overrideToday=dtAuto;
  overrideTempNow=false;
  setSetpoint();
};

void Data::overruleMultipleLessDays() { 
  if(overrideMultipleCount>2) overrideMultipleCount--;
  setSetpoint();
};

void Data::overruleMultipleMoreDays() { 
  overrideMultipleCount++;
  setSetpoint();
};

void Data::setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay) {
  regularWeek[dayOfWeek]=typeOfDay;
  setSetpoint();
};

void Data::workFromHomeWakeUpEarlier() { 
  if(workFromHomeWakeUp >= timeValue_t(0,15)) workFromHomeWakeUp-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromHomeWakeUpLater() { 
  if(workFromHomeWakeUp <= timeValue_t(23,15)) workFromHomeWakeUp+=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromHomeSleepEarlier() { 
  if(workFromHomeSleep >= timeValue_t(0,15)) workFromHomeSleep-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromHomeSleepLater() { 
  if(workFromHomeSleep <= timeValue_t(23,15)) workFromHomeSleep+=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeWakeUpEarlier() { 
  if(workAtOfficeWakeUp >= timeValue_t(0,15)) workAtOfficeWakeUp-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeWakeUpLater() { 
  if(workAtOfficeWakeUp <= timeValue_t(23,15)) workAtOfficeWakeUp+=timeValue_t(0,15);
  setSetpoint();
};
 
void Data::workFromOfficeGoOutEarlier() { 
  if(workAtOfficeGoOut >= timeValue_t(0,15)) workAtOfficeGoOut-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeGoOutLater() { 
  if(workAtOfficeGoOut <= timeValue_t(23,15)) workAtOfficeGoOut+=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeComeInEarlier() { 
  if(workAtOfficeComeIn >= timeValue_t(0,15)) workAtOfficeComeIn-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeComeInLater() { 
  if(workAtOfficeComeIn <= timeValue_t(23,15)) workAtOfficeComeIn+=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeSleepEarlier() { 
  if(workAtOfficeSleep >= timeValue_t(0,15)) workAtOfficeSleep-=timeValue_t(0,15);
  setSetpoint();
};

void Data::workFromOfficeSleepLater() { 
  if(workAtOfficeSleep <= timeValue_t(23,15)) workAtOfficeSleep+=timeValue_t(0,15);
  setSetpoint();
};

void Data::weekendWakeUpEarlier() { 
  if(weekendWakeUp >= timeValue_t(0,15)) weekendWakeUp-=timeValue_t(0,15);
  setSetpoint();
};

void Data::weekendWakeUpLater() { 
  if(weekendWakeUp <= timeValue_t(23,15)) weekendWakeUp+=timeValue_t(0,15);
  setSetpoint();
};

void Data::weekendSleepEarlier() { 
  if(weekendSleep >= timeValue_t(0,15)) weekendSleep-=timeValue_t(0,15);
  setSetpoint();
};

void Data::weekendSleepLater() { 
  if(weekendSleep <= timeValue_t(23,15)) weekendSleep+=timeValue_t(0,15);
  setSetpoint();
};

void Data::highTempLower() { 
  if(highTemp>lowTemp+0.5) highTemp-=0.5;
  setSetpoint();
};

void Data::highTempHigher() { 
  if(highTemp<30) highTemp+=0.5;
  setSetpoint();
};

void Data::lowTempLower() { 
  if(lowTemp>4.0) lowTemp-=0.5;
  setSetpoint();
};

void Data::lowTempHigher() { 
  if(lowTemp<highTemp-0.5) lowTemp+=0.5;
  setSetpoint();
};
