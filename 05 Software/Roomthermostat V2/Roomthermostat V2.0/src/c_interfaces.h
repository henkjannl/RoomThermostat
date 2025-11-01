#pragma once

#include "FS.h"
#include "LittleFS.h"
#include <Preferences.h>

#include <ArduinoJson.h>
#include <list>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

using namespace std;

#define MAX_LENGTH_SUBMENU 140
#define MAX_NUM_SENSORS     4

const int CHAT_ID_LENGTH = 64;
const char* NAMESPACE_NVS = "controller";

typedef void (*keyboardFunction)(); // To create pointers to enableKeyboard and disablekeyboard

class ControllerData_t {
  public:
    // SETTINGS STORED IN CONFIG.JSN
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
    String botChatID="";

    // SETTINGS STORED IN SETTINGS_FILE.JSN
    timeValue_t workFromHomeWakeUp = {  7, 00 };
    timeValue_t workFromHomeSleep  = { 21, 30 };

    timeValue_t workAtOfficeWakeUp = {  6, 30 };
    timeValue_t workAtOfficeGoOut  = {  7, 15 };
    timeValue_t workAtOfficeComeIn = { 17, 15 };
    timeValue_t workAtOfficeSleep  = { 21, 30 };

    timeValue_t weekendWakeUp      = {  9, 00 };
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
    bool multipleForever;

    float sensorOffset=-3.5;        // The offset added to the temperature sensor

    // SETTINGS NOT STORED
    // Settings have been changed and need to be stored to SPIFFS
    bool settingsChanged = false;

    // Backlight
    bool backLightOn  = true;

    // Time status
    bool timeSynched  = false;

    // TEMPERATURE MEASUREMENTS
    uint8_t numSensors;
    DeviceAddress sensorAddress[MAX_NUM_SENSORS];
    bool temperatureMeasurementOK;
    float rawTemperature=20;        // The temperature without offset measured periodically
    float measuredTemperature() { return rawTemperature+sensorOffset; };
    timeValue_t lastTemperatureMeasurement = timeValue_t(0,0);

    // TEMPERATURE CONTROL
    float overrideSetpoint = 20.0;    // Value for temperatureSetpoint if overrideTempNow == true
    bool overrideTempNow = false;
    setpointReason_t reasonForSetpoint;
    dayType_t dayTypes[7] = {dtAuto, dtAuto, dtAuto, dtAuto, dtAuto, dtAuto, dtAuto};
    bool dayTypeOverruled[7] = {false, false, false, false, false, false, false};

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

    // BOILER STATUS
    // Boiler request
    float requestedDomesticHotWaterSetpoint = 55.0;
    float requestedBoilerTemperature;
    float boilerPercentage;
    bool  enableCentralHeating = true;
    bool  enableHotWater = true;
    bool  enableCooling = false;
    timeValue_t lastBoilerUpdate = timeValue_t(0,0);;

    // Boiler response
    boilerResponse_t boilerResponse;
    bool boilerOnline         = false;
    bool centralHeatingActive = false;
    bool hotWaterActive       = false;
    bool flameOn              = false;
    float actualBoilerTemperature;
    float actualDomesticHotWaterTemperature;
    float boilerPressure;

    // Log timing of subprocesses
    LogBusyTime logBusyTime;
    DataLogger_t datalogger;

    void loadConfig(fs::FS &fs, const char * configFile);

    bool saveSettingsNVS();
    bool loadSettingsNVS();
};

void ControllerData_t::loadConfig(fs::FS &fs, const char * configFile) {
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

bool ControllerData_t::saveSettingsNVS() {
  Preferences pref;

  if (!pref.begin(NAMESPACE_NVS, false)) {
    Serial.println("Failed to open NVS for writing");
    return false;
  }

  pref.putInt("workFromHmeWkUp",  (int)workFromHomeWakeUp.minutesSinceMidnight);
  pref.putInt("workFromHmeSlep",  (int)workFromHomeSleep.minutesSinceMidnight);
  pref.putInt("workAtOffWakeUp",  (int)workAtOfficeWakeUp.minutesSinceMidnight);
  pref.putInt("workAtOffGoOut",   (int)workAtOfficeGoOut.minutesSinceMidnight);
  pref.putInt("workAtOffComeIn",  (int)workAtOfficeComeIn.minutesSinceMidnight);
  pref.putInt("workAtOffSleep",   (int)workAtOfficeSleep.minutesSinceMidnight);
  pref.putInt("weekendWakeUp",    (int)weekendWakeUp.minutesSinceMidnight);
  pref.putInt("weekendSleep",     (int)weekendSleep.minutesSinceMidnight);

  pref.putInt("regularWeek0",     (int)regularWeek[0]);
  pref.putInt("regularWeek1",     (int)regularWeek[1]);
  pref.putInt("regularWeek2",     (int)regularWeek[2]);
  pref.putInt("regularWeek3",     (int)regularWeek[3]);
  pref.putInt("regularWeek4",     (int)regularWeek[4]);
  pref.putInt("regularWeek5",     (int)regularWeek[5]);
  pref.putInt("regularWeek6",     (int)regularWeek[6]);

  pref.putFloat("highTemp",       highTemp);
  pref.putFloat("lowTemp",        lowTemp);
  pref.putInt("overrideToday",    (int)overrideToday);
  pref.putInt("overrideTomrrw",   (int)overrideTomorrow);
  pref.putInt("overrideMltpl",    (int)overrideMultiple);
  pref.putInt("overrideMultCnt",  (int)overrideMultipleCount);
  pref.putBool("multipleForever", multipleForever);
  pref.putFloat("sensorOffset",   sensorOffset);
  pref.end();
  settingsChanged = false;
  return true;
}

bool ControllerData_t::loadSettingsNVS() {
  Preferences pref;

  if (!pref.begin(NAMESPACE_NVS, true)) {   // true = read-only
    Serial.println("Failed to open NVS for reading");
    return false;
  }

  workFromHomeWakeUp.minutesSinceMidnight = pref.getInt("workFromHmeWkUp",  7*60+ 0);
  workFromHomeSleep.minutesSinceMidnight  = pref.getInt("workFromHmeSlep", 21*60+30);
  workAtOfficeWakeUp.minutesSinceMidnight = pref.getInt("workAtOffWakeUp",  6*60+30);
  workAtOfficeGoOut.minutesSinceMidnight  = pref.getInt("workAtOffGoOut",   7*60+15);
  workAtOfficeComeIn.minutesSinceMidnight = pref.getInt("workAtOffComeIn", 17*60+15);
  workAtOfficeSleep.minutesSinceMidnight  = pref.getInt("workAtOffSleep",  21*60+30);
  weekendWakeUp.minutesSinceMidnight      = pref.getInt("weekendWakeUp",    9*60+30);
  weekendSleep.minutesSinceMidnight       = pref.getInt("weekendSleep",    22*60+ 0);

  regularWeek[0] = (dayType_t)pref.getInt("regularWeek0", 2);
  regularWeek[1] = (dayType_t)pref.getInt("regularWeek1", 1);
  regularWeek[2] = (dayType_t)pref.getInt("regularWeek2", 0);
  regularWeek[3] = (dayType_t)pref.getInt("regularWeek3", 1);
  regularWeek[4] = (dayType_t)pref.getInt("regularWeek4", 1);
  regularWeek[5] = (dayType_t)pref.getInt("regularWeek5", 0);
  regularWeek[6] = (dayType_t)pref.getInt("regularWeek6", 2);

  highTemp              = pref.getFloat("highTemp", 19.5);
  lowTemp               = pref.getFloat("lowTemp", 17);
  overrideToday         = (dayType_t)pref.getInt("overrideToday", 4);
  overrideTomorrow      = (dayType_t)pref.getInt("overrideTomrrw", 4);
  overrideMultiple      = (dayType_t)pref.getInt("overrideMltpl", 4);
  overrideMultipleCount = pref.getInt("overrideMultCnt", 0);
  multipleForever       = pref.getBool("multipleForever", false);
  sensorOffset          = pref.getFloat("sensorOffset", -3.5);

  pref.end();
  settingsChanged = true; // in case defaults were used
  return true;
}

class userEventMessage_t {
  public:
    userEventMessage_t() {};
    userEventMessage_t(sender_t sender, command_t command);                               // Constructor with sender and command
    userEventMessage_t(screen_t screen, command_t selectedMenuItem, command_t command);   // Constructor for basic message from menu
    userEventMessage_t(String chatID, screen_t screen, command_t command);                // Constructor for basic Telegram message

    sender_t sender = sndInvalid;
    command_t command = cmdCommandNotRecognized;
    char chatID[CHAT_ID_LENGTH] = "";
    screen_t screen = scnInvalid;
    command_t selectedMenuItem = cmdCommandNotRecognized;
    float temperatureOffset = 0;
    uint8_t dayOfWeek = 0;
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
userEventMessage_t::userEventMessage_t(String chatID, screen_t screen, command_t command) {
  this->sender = sndTelegram;
  snprintf ( this->chatID, CHAT_ID_LENGTH, "%s", chatID);
  this->screen = screen;
  this->command = command;
}

// Global function to send simple messages
void sendMessage(sender_t sender, command_t command, QueueHandle_t &queue) {
  userEventMessage_t message;
  message.sender = sender;
  message.command = command;
  xQueueSend( queue, &message, ( TickType_t ) 10 );
}

void sendMessage(sender_t sender, command_t command, QueueHandle_t &queue, String &chatID) {
  userEventMessage_t message;
  message.sender = sender;
  message.command = command;
  snprintf(message.chatID, CHAT_ID_LENGTH, "%s", chatID);
  xQueueSend( queue, &message, ( TickType_t ) 10 );
}


/******************************************
 * GLOBAL VARIABLES                       *
 ******************************************/

// Central bus that handles most communication
ControllerData_t controllerData;

// A queue for each process
QueueHandle_t menuQueue        = xQueueCreate( 20, sizeof(userEventMessage_t) );
QueueHandle_t telegramQueue    = xQueueCreate( 20, sizeof(userEventMessage_t) );
QueueHandle_t controllerQueue  = xQueueCreate( 20, sizeof(userEventMessage_t) );
QueueHandle_t temperatureQueue = xQueueCreate( 20, sizeof(userEventMessage_t) );
QueueHandle_t displayQueue     = xQueueCreate( 20, sizeof(userEventMessage_t) );
QueueHandle_t backlightQueue   = xQueueCreate( 20, sizeof(userEventMessage_t) );
