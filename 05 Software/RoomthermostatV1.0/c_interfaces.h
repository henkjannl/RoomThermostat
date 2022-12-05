#pragma once

#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <list>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

using namespace std;

#define MAX_LENGTH_SUBMENU 140
#define MAX_NUM_SENSORS     4

const int CHAT_ID_LENGTH = 64;

typedef void (*keyboardFunction)(); // To create pointers to enableKeyboard and disablekeyboard

class ControllerData_t {
  public:
    timeValue_t workFromHomeWakeUp = {  7, 00 };
    timeValue_t workFromHomeSleep  = { 21, 30 };

    timeValue_t workAtOfficeWakeUp = {  7, 00 };
    timeValue_t workAtOfficeGoOut  = {  7, 45 };
    timeValue_t workAtOfficeComeIn = { 17, 00 };
    timeValue_t workAtOfficeSleep  = { 21, 30 };

    timeValue_t weekendWakeUp      = {  9, 30 };
    timeValue_t weekendSleep       = { 22, 00 };

    // Program to switch temperature setpoint if not overruled
    // Regular weekly schedule (Sunday is day 0)
    //                           Sunday     Monday          Tuesday         Wednesday       Thursday        Friday          Saturday  
    dayType_t regularWeek[7] = { dtWeekend, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWorkAtOffice, dtWeekend }; 

    float highTemp = 19.5;
    float lowTemp  = 10.0;

    // Current settings of temperature
    dayType_t overrideToday     = dtAuto;
    dayType_t overrideTomorrow  = dtAuto;
    dayType_t overrideMultiple  = dtAuto;
    int overrideMultipleCount   = 3;
    
    // Backlight
    bool backLightOn  = true; 

    // Time status
    bool timeSynched  = false;

    // TEMPERATRE MEASUREMENTS
    uint8_t numSensors;
    DeviceAddress sensorAddress[MAX_NUM_SENSORS];
    bool temperatureMeasurementOK;
    float rawTemperature;        // The temperature without offset measured periodically
    float sensorOffset=-3.5;     // The offset added
    float measuredTemperature() { return rawTemperature+sensorOffset; };
    
    // TEMPERATURE CONTROL    
    float overrideSetpoint = 20.0;    // Value for temperatureSetpoint if overrideTempNow == true
    bool overrideTempNow = false;
    setpointReason_t reasonForSetpoint;

    float temperatureSetpoint = 20.0; // Temperature actually sent to the PID controller
    float P;
    float I;
    float D;
    float ierr;
    float derivative;

    // Text to be displayed on the display or in Telegram
    icon_t dspFlameIcon   = iconFlame0;  
    icon_t dspShowerIcon  = iconShowerOff;
    icon_t dspWifiIcon    = iconWifiLost;
    icon_t dspBoilerIcon  = iconBoilerLost;
    icon_t dayIcons[7]    = { iconHome, iconHome, iconHome, iconHome, iconHome, iconHome, iconHome };

    // BOILER STATUS
    // Boiler request
    float requestedDomesticHotWaterSetpoint = 55.0;
    float requestedBoilerTemperature;
    float boilerPercentage;
    bool  enableCentralHeating = true;
    bool  enableHotWater = true;
    bool  enableCooling = false;

    // Boiler response
    boilerResponse_t boilerResponse;
    bool boilerOnline         = false;
    bool centralHeatingActive = false;
    bool hotWaterActive       = false;
    bool flameOn              = false;
    float actualBoilerTemperature;
    float actualDomesticHotWaterTemperature;
    float boilerPressure;

    // WiFi
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

    // Log timing of subprocesses
    LogBusyTime logBusyTime;

    void loadConfig(fs::FS &fs, const char * configFile);
    void saveSettings(fs::FS &fs, const char * tempFile, const char * configFile);
    void loadSettings(fs::FS &fs, const char * configFile);
};

void ControllerData_t::loadConfig(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  //disableKeyboard();

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

  //enableKeyboard();
};

// Loads the settings from a file
void ControllerData_t::loadSettings(fs::FS &fs, const char * configFile) {    
  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  //disableKeyboard();

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

  //enableKeyboard();
};

// Saves the settings to a file
void ControllerData_t::saveSettings(fs::FS &fs, const char * tempFile, const char * configFile) {    

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

class userEventMessage_t {
  public:
    userEventMessage_t() {};
    userEventMessage_t(sender_t sender, command_t command);                               // Constructor with just a sender and a command
    userEventMessage_t(screen_t screen, command_t selectedMenuItem, command_t command);   // Constructor for basic message from menu
    userEventMessage_t(String chatID, int messageID, screen_t screen, command_t command); // Constructor for basic Telegram message
  
    sender_t sender;
    command_t command;
    char chatID[CHAT_ID_LENGTH];
    int messageID;
    screen_t screen;
    command_t selectedMenuItem;
    float temperatureOffset;
    uint8_t dayOfWeek;
    dayType_t typeOfDay;
};

// Constructor with just a sender and a command
userEventMessage_t::userEventMessage_t(sender_t sender, command_t command) {
  this->sender = sender;
  this->command = command;
}


// Constructor for basic message from menu
userEventMessage_t::userEventMessage_t(screen_t screen, command_t selectedMenuItem, command_t command) {
  this->sender = sndMenu;
  this->screen = screen;
  this->selectedMenuItem = selectedMenuItem;
  this->command = command;
}

// Constructor for basic Telegram message
userEventMessage_t::userEventMessage_t(String chatID, int messageID, screen_t screen, command_t command) {
  this->sender = sndTelegram;
  snprintf ( this->chatID, CHAT_ID_LENGTH, "%s", chatID);
  this->messageID = messageID;
  this->screen = screen;
  this->command = command;
}


/******************************************
 * GLOBAL VARIABLES                       * 
 ******************************************/

// Queue of commands from keyboard to menu
QueueHandle_t backlightQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );  // Queue of pressed keys sent to menu

// Queue of commands from keyboard to menu
QueueHandle_t menuQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );  // Queue of pressed keys sent to menu

// Queue of commands handled by the controller
QueueHandle_t controllerQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );       
  
// Central bus that handles most communication
ControllerData_t controllerData;                                       

// Mutex that allows talking to the controllerData
SemaphoreHandle_t controllerDataSemaphore = xSemaphoreCreateMutex();

// Queue of commands handled by the display
QueueHandle_t displayQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );       

// Queue of commands handled by the Telegram client
QueueHandle_t telegramQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );       

/*
  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

    xSemaphoreGive( controllerDataSemaphore );
  }
*/


/*
  // Update data for display and Telegram
  if(controllerData->timeSynched) {
    strftime(buffer, BUFLEN, "%A %e %B", localTime);
    controllerData->dspDate = buffer;

    strftime(buffer, BUFLEN, "%R", localTime);
    controllerData->dspTime = buffer;
  }
*/
