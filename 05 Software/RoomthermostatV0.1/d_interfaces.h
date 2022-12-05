#pragma once

#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <list>

using namespace std;

#define MAX_LENGTH_SUBMENU 140

class DataBus {
  public:
    DataBus(keyboardFunction disableKeyboard, keyboardFunction enableKeyboard);
    void loadConfig(fs::FS &fs, const char * configFile);
    void saveSettings(fs::FS &fs, const char * tempFile, const char * configFile);
    void loadSettings(fs::FS &fs, const char * configFile);
    
    void invalidateAll();
    bool somethingUpdated();

    // Log timing of subprocesses
    LogBusyTime logBusyTime;
    
    // Persistent data of the controller
    timeValue_t workFromHomeWakeUp = {  6, 45 };
    timeValue_t workFromHomeSleep  = { 21, 30 };

    timeValue_t workAtOfficeWakeUp = {  6, 45 };
    timeValue_t workAtOfficeGoOut  = {  8, 00 };
    timeValue_t workAtOfficeComeIn = { 17, 00 };
    timeValue_t workAtOfficeSleep  = { 21, 30 };

    timeValue_t weekendWakeUp      = {  9, 00 };
    timeValue_t weekendSleep       = { 22, 30 };

    // Program to switch temperature setpoint if not overruled
    // Regular weekly schedule (Sunday is day 0)
    //                           Sunday     Monday          Tuesday         Wednesday       Thursday        Friday          Saturday  
    dayType_t regularWeek[7] = { dtWeekend, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWeekend }; 

    float highTemp = 21.0;
    float lowTemp  = 10.0;

    // Current settings of temperature
    dayType_t overrideToday     = dtAuto;
    dayType_t overrideTomorrow  = dtAuto;
    dayType_t overrideMultiple  = dtAuto;
    int overrideMultipleCount   = 3;

    // Temperature measurement
    bool roomTemperatureStatusOK;
    float temperatureSensorOffset;
    float measuredTemperature;

    // Time status
    bool timeSynched          = false;

    // TEMPERATURE CONTROL    
    float overrideSetpoint = 21.0;    // Value for temperatureSetpoint if overrideTempNow == true
    bool overrideTempNow = false;
    setpointReason_t reasonForSetpoint;

    float temperatureSetpoint = 21.0; // Temperature actually sent to the PID controller
    float P;
    float I;
    float D;
    float ierr;
    float derivative;

    // BOILER STATUS
    // Boiler request
    float requestedDomesticHotWaterSetpoint = 55.0;
    float requestedBoilerTemperature;
    float boilerPercentage;
    bool enableCentralHeating = true;
    bool enableHotWater = true;
    bool enableCooling = false;

    // Boiler response
    bool boilerOnline         = false;
    bool centralHeatingActive = false;
    bool hotWaterActive       = false;
    bool flameOn              = false;
    float actualBoilerTemperature;
    float actualDomesticHotWaterTemperature;
    String responseFromBoiler="Not started yet";

    list<tAccessPoint> AccessPoints;
  
    bool locationKnown=false;            
    char timezone[64]="Europe/Amsterdam";
    int  timeZoneOffset=3600;
    int  timeDSToffset=0;
  
    // TELEGRAM
    // Telegram bot settings
    String botName;
    String botUserName;
    String botToken;
    String botChatID;
    bool delayTelegram=true; // slow down Telegram if backlight is on
    
    // Data to display
    DisplayParameter_t<String>  dpDate;
    DisplayParameter_t<String>  dpTime;
    DisplayParameter_t<String>  dpTemperatureSetpoint;
    DisplayParameter_t<bool>    dpOverrideTempNow;
    DisplayParameter_t<String>  dpMeasuredTemperature;
    DisplayParameter_t<icon_t>  dpFlameIcon;
    DisplayParameter_t<icon_t>  dpShowerIcon;
    DisplayParameter_t<icon_t>  dpWifiIcon;
    DisplayParameter_t<icon_t>  dpBoilerConnIcon;
    DisplayParameter_t<icon_t>  dpWeekProgramIcons[7];

    int showSubMenuLines=0;
    char subMenuLine1[MAX_LENGTH_SUBMENU];
    char subMenuLine2[MAX_LENGTH_SUBMENU];

  protected:
    keyboardFunction disableKeyboard;  // pointer to the function that disables the keyboard
    keyboardFunction enableKeyboard;   // pointer to the function that disables the keyboard
};

DataBus::DataBus(keyboardFunction disableKeyboard, keyboardFunction enableKeyboard) { 
  this->disableKeyboard = disableKeyboard; 
  this->enableKeyboard  = enableKeyboard; 

  dpDate.setValue("");  
  dpTime.setValue("--:--");
  dpTemperatureSetpoint.setValue("--");
  dpOverrideTempNow.setValue(false);
  dpMeasuredTemperature.setValue("--");
  dpFlameIcon     .setValue(iconFlame0);
  dpShowerIcon    .setValue(iconShowerOff);
  dpWifiIcon      .setValue(iconWifiLost);
  dpBoilerConnIcon.setValue(iconBoilerConnected);  
  for(int i =0; i<7; i++) dpWeekProgramIcons[i].setValue(iconAway);
}

void DataBus::invalidateAll() {
  // Useful when whole screen needs to be redrawn
  dpDate.invalidate();
  dpTime.invalidate();
  dpTemperatureSetpoint.invalidate();
  dpOverrideTempNow.invalidate();
  dpMeasuredTemperature.invalidate();
  dpFlameIcon.invalidate();
  dpShowerIcon.invalidate();
  dpWifiIcon.invalidate();
  dpBoilerConnIcon.invalidate();
  for(int i=0; i<7; i++) { dpWeekProgramIcons[i].invalidate(); };
}

bool DataBus::somethingUpdated() {
  if(dpDate               .isChanged()) return true;
  if(dpTime               .isChanged()) return true;
  if(dpTemperatureSetpoint.isChanged()) return true;
  if(dpOverrideTempNow    .isChanged()) return true;
  if(dpMeasuredTemperature.isChanged()) return true;
  if(dpFlameIcon          .isChanged()) return true;
  if(dpShowerIcon         .isChanged()) return true;
  if(dpWifiIcon           .isChanged()) return true;
  if(dpBoilerConnIcon     .isChanged()) return true;

  for(int i=0; i<7; i++) 
    if(dpWeekProgramIcons[i].isChanged()) return true;

  return false;
};

void DataBus::loadConfig(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  disableKeyboard();

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

  enableKeyboard();
};

// Loads the settings from a file
void DataBus::loadSettings(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  disableKeyboard();

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

  enableKeyboard();
};

// Saves the settings to a file
void DataBus::saveSettings(fs::FS &fs, const char * tempFile, const char * configFile) {    

  StaticJsonDocument<1024> doc;

  File output = fs.open(tempFile);

  JsonArray jsnAPs = doc.createNestedArray("AccessPoints");
  for(auto & accesspoint : AccessPoints) {
    
    JsonObject jsnAP = jsnAPs.createNestedObject();
    jsnAP["SSID"] = accesspoint.ssid;
    jsnAP["password"] = accesspoint.password;
    jsnAP["timezone"] = accesspoint.timezone;
  }
    
  doc["BotName"]     = botName;
  doc["BotUsername"] = botUserName;
  doc["BotToken"]    = botToken;
  doc["ChatID"]      = botChatID;
    
  serializeJson(doc, output);
}
