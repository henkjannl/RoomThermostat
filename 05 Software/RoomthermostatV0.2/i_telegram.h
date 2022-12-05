#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

#define TELEGRAM_INTERVAL 1000

using namespace std;

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

const String EMOTICON_BULLSEYE           = "\xF0\x9F\x8E\xAF";
const String EMOTICON_GLASSES            = "\xF0\x9F\x91\x93";
const String EMOTICON_STHETOSCOPE        = "\xF0\x9F\xA9\xBA";
const String EMOTICON_SHOWER             = "\xF0\x9F\x9A\xBF";
const String EMOTICON_BATH               = "\xF0\x9F\x9B\x81";
const String EMOTICON_FLAME              = "\xF0\x9F\x94\xA5";
const String EMOTICON_POINTING_FINGER    = "\xF0\x9F\x91\x89";
const String EMOTICON_THERMOMETER        = "\xF0\x9F\x8C\xA1"; 
const String EMOTICON_UP_ARROW           = "\xE2\xAC\x86\xEF\xB8\x8F";
const String EMOTICON_DOWN_ARROW         = "\xE2\xAC\x87\xEF\xB8\x8F";
const String EMOTICON_TRIANGLE_UP        = "\xF0\x9F\x94\xBC";
const String EMOTICON_TRIANGLE_DOWN      = "\xF0\x9F\x94\xBD";
const String EMOTICON_RED_QUESTION_MARK  = "\xE2\x9D\x93";
const String EMOTICON_PIN                = "\xF0\x9F\x93\x8D";
const String EMOTICON_OFFICE             = "\xF0\x9F\x8F\xA2";
const String EMOTICON_HOUSE              = "\xF0\x9F\x8F\xA0";
const String EMOTICON_ISLAND             = "\xF0\x9F\x8F\x96";
const String EMOTICON_CALENDAR           = "\xF0\x9F\x93\x85";
const String EMOTICON_CHECKMARK          = "\xE2\x9C\x85";
const String EMOTICON_CROSSMARK          = "\xE2\x9D\x8C";
const String EMOTICON_LAMP               = "\xF0\x9F\x92\xA1";
const String EMOTICON_WARNING            = "\xE2\x9A\xA0\xEF\xB8\x8F";
const String EMOTICON_CLOCK              = "\xE2\x9A\xA0\xEF\xB8\x8F";
const String EMOTICON_FOOTSTEPS          = "\xF0\x9F\x91\xA3";
const String EMOTICON_RUNNER             = "\xF0\x9F\x8F\x83";
const String EMOTICON_MAGIC_STICK        = "\xF0\x9F\xAA\x84";
const String EMOTICON_BED                = "\xF0\x9F\x9B\x8F";
const String EMOTICON_SMILEY             = "\xF0\x9F\x98\x80";


// All available Telegram commands
  
struct tgLabelCallback {
  String label;            // label can change depending on the context
  const String callback;   // callback strings must remain the same
};

typedef std::map<command_t, tgLabelCallback> tgCommandList;

const tgCommandList TELEGRAM_COMMANDS = {
  { cmdSetpointLower,                { EMOTICON_DOWN_ARROW + " temperature" , "/cmdSetpointLower"                } },
  { cmdSetpointAuto,                 { EMOTICON_MAGIC_STICK + " Auto", "/cmdSetpointAuto"                 } },
  { cmdSetpointHigher,               { EMOTICON_UP_ARROW + " temperature", "/cmdSetpointHigher"               } },
  { cmdGoAway,                       { EMOTICON_FOOTSTEPS + " Go out", "/cmdGoAway"                       } },
  { cmdMenuOverruleToday,            { "Overrule today",               "/cmdMenuOverruleToday"            } },
  { cmdMenuOverruleTomorrow,         { "Overrule tomorrow",            "/cmdMenuOverruleTomorrow"         } },
  { cmdMenuOverruleMultipleDays,     { "Overrule multiple days",       "/cmdMenuOverruleMultipleDays"     } },
  { cmdMenuSettings,                 { "Settings",                     "/cmdMenuSettings"                 } },
  { cmdUpdateStatus,                 { "Status",                       "/cmdUpdateStatus"                 } },

  { cmdOverruleTodayWorkFromHome,    { EMOTICON_HOUSE + " Work from home",     "/cmdOverruleTodayWorkFromHome"    } },
  { cmdOverruleTodayWorkAtOffice,    { EMOTICON_OFFICE + " Work from office",  "/cmdOverruleTodayWorkAtOffice"    } },
  { cmdOverruleTodayWeekend,         { EMOTICON_CALENDAR + " Weekend",         "/cmdOverruleTodayWeekend"         } },
  { cmdOverruleTodayAway,            { EMOTICON_ISLAND + " All day away",      "/cmdOverruleTodayAway"            } },
  { cmdOverruleTodayAutomatic,       { EMOTICON_MAGIC_STICK + " Automatic",    "/cmdOverruleTodayAutomatic"       } },
  { cmdMenuMain,                     { "Back to main menu",                    "/cmdMenuMain"                     } },

  { cmdOverruleTomorrowWorkFromHome, { "Work from home",         "/cmdOverruleTomorrowWorkFromHome" } },
  { cmdOverruleTomorrowWorkAtOffice, { "Work from office",       "/cmdOverruleTomorrowWorkAtOffice" } },
  { cmdOverruleTomorrowWeekend,      { "Weekend",                "/cmdOverruleTomorrowWeekend"      } },
  { cmdOverruleTomorrowAway,         { "All day away",           "/cmdOverruleTomorrowAway"         } },
  { cmdOverruleTomorrowAutomatic,    { "Automatic",              "/cmdOverruleTomorrowAutomatic"    } },

  { cmdOverruleMultipleWorkFromHome, { "Work from home",         "/cmdOverruleMultipleWorkFromHome" } },
  { cmdOverruleMultipleWorkAtOffice, { "Work from office",       "/cmdOverruleMultipleWorkAtOffice" } },
  { cmdOverruleMultipleWeekend,      { "Weekend",                "/cmdOverruleMultipleWeekend"      } },
  { cmdOverruleMultipleAway,         { "All day away",           "/cmdOverruleMultipleAway"         } },
  { cmdOverruleMultipleAutomatic,    { "Automatic",              "/cmdOverruleMultipleAutomatic"    } },
  { cmdOverruleMultipleFewerDays,    { "..days",                 "/cmdOverruleMultipleFewerDays"    } },
  { cmdOverruleMultipleMoreDays,     { "..days",                 "/cmdOverruleMultipleMoreDays"     } },

  { cmdMenuWeekSchedule,             { "Weekly schedule",        "/cmdMenuWeekSchedule"             } },
  { cmdMenuHomeTimes,                { "Home times",             "/cmdMenuHomeTimes"                } },
  { cmdMenuOfficeTimes,              { "Office times",           "/cmdMenuOfficeTimes"              } },
  { cmdMenuWeekendTimes,             { "Weekend times",          "/cmdMenuWeekendTimes"             } },
  { cmdMenuTemperature,              { "Temperatures",           "/cmdMenuTemperature"              } },
  { cmdMenuSensorOffset,             { "Sensor offset",          "/cmdMenuSensorOffset"             } },
  { cmdReportBoiler,                 { "Boiler report",          "/cmdReportBoiler"                 } },
  { cmdReportTiming,                 { "Timing report",          "/cmdReportTiming"                 } },
    
  { cmdMonday,                       { "Monday",                 "/cmdMonday"                       } },
  { cmdTuesday,                      { "Tuesday",                "/cmdTuesday"                      } },
  { cmdWednesday,                    { "Wednesday",              "/cmdWednesday"                    } },
  { cmdThursday,                     { "Thursday",               "/cmdThursday"                     } },
  { cmdFriday,                       { "Friday",                 "/cmdFriday"                       } },
  { cmdSaturday,                     { "Saturday",               "/cmdSaturday"                     } },
  { cmdSunday,                       { "Sunday",                 "/cmdSunday"                       } },

  { cmdWorkFromHome,                 { "Work from home",         "/cmdWorkFromHome"                 } },
  { cmdWorkAtOffice,                 { "Work from office",       "/cmdWorkAtOffice"                 } },
  { cmdWeekend,                      { "Weekend day",            "/cmdWeekend"                      } },
  { cmdAllDayAway,                   { "All day away",           "/cmdAllDayAway"                   } },

  { cmdHomeWakeUpEarlier,            { "A: Wake up -",           "/cmdHomeWakeUpEarlier"            } },
  { cmdHomeWakeUpLater,              { "A: Wake up +",           "/cmdHomeWakeUpLater"              } },
  { cmdHomeGoToSleepEarlier,         { "B: Sleep -",             "/cmdHomeGoToSleepEarlier"         } },
  { cmdHomeGoToSleepLater,           { "B: Sleep +",             "/cmdHomeGoToSleepLater"           } },
 
  { cmdOfficeWakeUpEarlier,          { "A: Wake up -",           "/cmdOfficeWakeUpEarlier"          } },
  { cmdOfficeWakeUpLater,            { "A: Wake up +",           "/cmdOfficeWakeUpLater"            } },
  { cmdOfficeLeaveEarlier,           { "B: Go out -",            "/cmdOfficeLeaveEarlier"           } },
  { cmdOfficeLeaveLater,             { "B: Go out +",            "/cmdOfficeLeaveLater"             } },
  { cmdOfficeComeHomeEarlier,        { "C: Come in -",           "/cmdOfficeComeHomeEarlier"        } },
  { cmdOfficeComeHomeLater,          { "C: Come in +",           "/cmdOfficeComeHomeLater"          } },
  { cmdOfficeGoToSleepEarlier,       { "D: Sleep -",             "/cmdOfficeGoToSleepEarlier"       } },
  { cmdOfficeGoToSleepLater,         { "D: Sleep +",             "/cmdOfficeGoToSleepLater"         } },

  { cmdWeekendWakeUpEarlier,         { "A: Wake up -",           "/cmdWeekendWakeUpEarlier"         } },
  { cmdWeekendWakeUpLater,           { "A: Wake up +",           "/cmdWeekendWakeUpLater"           } },
  { cmdWeekendGoToSleepEarlier,      { "B: Sleep -",             "/cmdWeekendGoToSleepEarlier"      } },
  { cmdWeekendGoToSleepLater,        { "B: Sleep +",             "/cmdWeekendGoToSleepLater"        } },

  { cmdHighTemperatureDown,          { "High temp ..°C",         "/cmdHighTemperatureDown"          } },
  { cmdHighTemperatureUp,            { "High temp ..°C",         "/cmdHighTemperatureUp"            } },
  { cmdLowTemperatureDown,           { "Low temp ..°C",          "/cmdLowTemperatureDown"           } },
  { cmdLowTemperatureUp,             { "Low temp ..°C",          "/cmdLowTemperatureUp"             } },

  { cmdSensorOffsetDown,             { EMOTICON_DOWN_ARROW,      "/cmdSensorOffsetDown"             } },
  { cmdSensorOffsetUp,               { EMOTICON_UP_ARROW,        "/cmdSensorOffsetUp"               } }
};  

// One TelegramChat object for each user known in the system
class TelegramChat {
  public:
    TelegramChat() {}; // Required for the class to be used in a map
    TelegramChat(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue );
    String chatID = "";
    String userName = "";

    screen_t screen = scnMain;
    uint8_t dayForSubmenu;
    tgCommandList commands = TELEGRAM_COMMANDS;

    void (*messageToControllerCallback)(userEventMessage_t message);

    void   updateLabels();
    String keyboard();
    String handleMessage(String & message);
    void handleCallback(int messageID, String & callback);
    String displayResponse(userEventMessage_t & message);
    
  protected:
    ControllerData_t *controllerData;
    QueueHandle_t controllerQueue;
    QueueHandle_t telegramQueue;
    String btnInline(command_t command);
    String temperature(float temp);
    String number(float f);
    String currentTime();
    void sendMessageToController(int messageID, command_t command);
    void sendTemperatureOffsetToController(int messageID, float temperatureOffset);
    void sendWeekScheduleToController(int messageID, uint8_t dayOfWeek, dayType_t typeOfDay);
    void sendMessageToTelegram(int messageID, command_t command);
};

TelegramChat::TelegramChat(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue ) { 
  this->chatID          =   controllerData.botChatID;
  this->userName        =   controllerData.botUserName;
  this->controllerData  = & controllerData; 
  this->controllerQueue =   controllerQueue; 
  this->telegramQueue   =   telegramQueue;
};

void TelegramChat::sendMessageToController(int messageID, command_t command) {
  userEventMessage_t message(chatID, messageID, screen, command);
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::sendTemperatureOffsetToController(int messageID, float temperatureOffset) {
  userEventMessage_t message(chatID, messageID, screen, cmdSetTemperatureOffset);
  message.temperatureOffset = temperatureOffset;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::sendWeekScheduleToController(int messageID, uint8_t dayOfWeek, dayType_t typeOfDay) {
  userEventMessage_t message(chatID, messageID, screen, cmdSetWeekSchedule);
  message.dayOfWeek = dayOfWeek;
  message.typeOfDay = typeOfDay;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::sendMessageToTelegram(int messageID, command_t command) {
  userEventMessage_t message(chatID, messageID, screen, command);
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::updateLabels() {

  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
    //commands[cmdSetpointLower            ].label = EMOTICON_TRIANGLE_DOWN + " " + temperature(controllerData->overrideSetpoint-0.5);
    //commands[cmdSetpointHigher           ].label = EMOTICON_TRIANGLE_UP   + " " + temperature(controllerData->overrideSetpoint+0.5);
    commands[cmdOverruleMultipleFewerDays].label = EMOTICON_TRIANGLE_DOWN + String(controllerData->overrideMultipleCount-1) + " days";
    commands[cmdOverruleMultipleMoreDays ].label = EMOTICON_TRIANGLE_UP   + String(controllerData->overrideMultipleCount+1) + " days";
    commands[cmdHighTemperatureDown      ].label = EMOTICON_TRIANGLE_DOWN + temperature(controllerData->highTemp-0.5);
    commands[cmdHighTemperatureUp        ].label = EMOTICON_TRIANGLE_UP   + temperature(controllerData->highTemp+0.5);
    commands[cmdLowTemperatureDown       ].label = EMOTICON_TRIANGLE_DOWN + temperature(controllerData->lowTemp -0.5);
    commands[cmdLowTemperatureUp         ].label = EMOTICON_TRIANGLE_UP   + temperature(controllerData->lowTemp +0.5);
  
    commands[cmdHomeWakeUpEarlier        ].label = "wake "    +String( (controllerData->workFromHomeWakeUp -timeValue_t(0,15) ).str().c_str());
    commands[cmdHomeWakeUpLater          ].label = "wake "    +String( (controllerData->workFromHomeWakeUp +timeValue_t(0,15) ).str().c_str());
    commands[cmdHomeGoToSleepEarlier     ].label = "sleep "   +String( (controllerData->workFromHomeSleep  -timeValue_t(0,15) ).str().c_str());
    commands[cmdHomeGoToSleepLater       ].label = "sleep "   +String( (controllerData->workFromHomeSleep  +timeValue_t(0,15) ).str().c_str());
  
    commands[cmdOfficeWakeUpEarlier      ].label = "wake "    +String( (controllerData->workAtOfficeWakeUp -timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeWakeUpLater        ].label = "wake "    +String( (controllerData->workAtOfficeWakeUp +timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeLeaveEarlier       ].label = "go out "  +String( (controllerData->workAtOfficeGoOut  -timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeLeaveLater         ].label = "go out "  +String( (controllerData->workAtOfficeGoOut  +timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeComeHomeEarlier    ].label = "come in " +String( (controllerData->workAtOfficeComeIn -timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeComeHomeLater      ].label = "come in " +String( (controllerData->workAtOfficeComeIn +timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeGoToSleepEarlier   ].label = "sleep "   +String( (controllerData->workAtOfficeSleep  -timeValue_t(0,15) ).str().c_str());
    commands[cmdOfficeGoToSleepLater     ].label = "sleep "   +String( (controllerData->workAtOfficeSleep  +timeValue_t(0,15) ).str().c_str());
  
    commands[cmdWeekendWakeUpEarlier     ].label = "wake "    +String( (controllerData->weekendWakeUp      -timeValue_t(0,15) ).str().c_str());
    commands[cmdWeekendWakeUpLater       ].label = "wake "    +String( (controllerData->weekendWakeUp      +timeValue_t(0,15) ).str().c_str());
    commands[cmdWeekendGoToSleepEarlier  ].label = "sleep "   +String( (controllerData->weekendSleep       -timeValue_t(0,15) ).str().c_str());
    commands[cmdWeekendGoToSleepLater    ].label = "sleep "   +String( (controllerData->weekendSleep       +timeValue_t(0,15) ).str().c_str());

    xSemaphoreGive( controllerDataSemaphore );
  } else {
    TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder( controllerDataSemaphore );    
    char *pcTaskName = pcTaskGetTaskName(mutexHolder);
    Serial.printf("TelegramChat::updateLabels(): mutex held by %s\n", pcTaskName);     
  }
};

String TelegramChat::btnInline(command_t command) {
  return "{ \"text\" : \"" + commands[command].label + "\", \"callback_data\" : \"" + commands[command].callback+ "\" }";
};

String TelegramChat::temperature(float temp) {
  char buffer[10];
  snprintf(buffer, 10, "%.1f°C", temp);
  return String(buffer);
};

String TelegramChat::number(float f) {
  char buffer[10];
  snprintf(buffer, 10, "%.3f", f);
  return String(buffer);
};

String TelegramChat::currentTime() {
  time_t now;
  time(&now);
  struct tm * local;
  local = localtime(&now);
  char buffer[20];
  snprintf(buffer, 20, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
  return buffer;
};

String TelegramChat::keyboard() {

  String result = "";
  const int BUFLEN = 80;
  char buffer1[BUFLEN];
  char buffer2[BUFLEN];
  
  switch(screen) {

    // Main menu is chosen as default option and therefore missing here

    case scnOverruleToday:
      result = "[["+ btnInline(cmdOverruleTodayWorkFromHome) + "],";
      result+=  "["+ btnInline(cmdOverruleTodayWorkAtOffice) + "],";
      result+=  "["+ btnInline(cmdOverruleTodayWeekend)      + "],";
      result+=  "["+ btnInline(cmdOverruleTodayAway)         + "],";
      result+=  "["+ btnInline(cmdOverruleTodayAutomatic)    + "],";
      result+=  "["+ btnInline(cmdMenuMain)                  + "]]";
    break;
    
    case scnOverruleTomorrow:
      result = "[["+ btnInline(cmdOverruleTomorrowWorkFromHome) + "],";
      result+=  "["+ btnInline(cmdOverruleTomorrowWorkAtOffice) + "],";
      result+=  "["+ btnInline(cmdOverruleTomorrowWeekend)      + "],";
      result+=  "["+ btnInline(cmdOverruleTomorrowAway)         + "],";
      result+=  "["+ btnInline(cmdOverruleTomorrowAutomatic)    + "],";
      result+=  "["+ btnInline(cmdMenuMain)                     + "]]";
    break;
    
    case scnOverruleMultiple:
      result= "[["+ btnInline(cmdOverruleMultipleWorkFromHome) + "],";
      result+= "["+ btnInline(cmdOverruleMultipleWorkAtOffice) + "],";
      result+= "["+ btnInline(cmdOverruleMultipleWeekend)      + "],";
      result+= "["+ btnInline(cmdOverruleMultipleAway)         + "],";
      result+= "["+ btnInline(cmdOverruleMultipleFewerDays)    + ", " + btnInline(cmdOverruleMultipleAutomatic) + ", " + 
                   btnInline(cmdOverruleMultipleMoreDays)      + "],";
      result+= "["+ btnInline(cmdMenuMain) + "]]";
    break;
    
    case scnSettingsMain:
      result= "[["+ btnInline(cmdMenuWeekSchedule) + "," + btnInline(cmdMenuHomeTimes)    + "],";
      result+= "["+ btnInline(cmdMenuOfficeTimes)  + "," + btnInline(cmdMenuWeekendTimes) + "],";
      result+= "["+ btnInline(cmdMenuTemperature)  + "," + btnInline(cmdMenuSensorOffset) + "],";
      result+= "["+ btnInline(cmdReportBoiler)     + "," + btnInline(cmdReportTiming)     + "],"; 
      result+= "["+ btnInline(cmdMenuMain)                                                + "]]";
    break;
    
    case scnSettingsWeekSchedule:
      result= "[["+ btnInline(cmdMonday)    + "," + btnInline(cmdTuesday)  + "],";
      result+= "["+ btnInline(cmdWednesday) + "," + btnInline(cmdThursday) + "],";
      result+= "["+ btnInline(cmdFriday)    + "," + btnInline(cmdSaturday) + "],";
      result+= "["+ btnInline(cmdSunday)                                   + "],";
      result+= "["+ btnInline(cmdMenuSettings)                             + "]]";
    break;
    
    case scnSettingsDaySchedule:
      result= "[["+ btnInline(cmdWorkFromHome) + "],";
      result+= "["+ btnInline(cmdWorkAtOffice) + "],";
      result+= "["+ btnInline(cmdWeekend)      + "],";
      result+= "["+ btnInline(cmdAllDayAway)   + "],";
      result+= "["+ btnInline(cmdMenuSettings) + "]]";
    break;
    
    case scnSettingsHomeTimes:
      result= "[["+ btnInline(cmdHomeWakeUpEarlier)     + "," + btnInline(cmdHomeWakeUpLater)    + "],";
      result+= "["+ btnInline(cmdHomeGoToSleepEarlier)  + "," + btnInline(cmdHomeGoToSleepLater) + "],";
      result+= "["+ btnInline(cmdMenuSettings)                                                   + "]]";
    break;
    
    case scnSettingsOfficeTimes:
      result= "[["+ btnInline(cmdOfficeWakeUpEarlier)     + "," + btnInline(cmdOfficeWakeUpLater)    + "],";
      result+= "["+ btnInline(cmdOfficeLeaveEarlier)      + "," + btnInline(cmdOfficeLeaveLater)     + "],";
      result+= "["+ btnInline(cmdOfficeComeHomeEarlier)   + "," + btnInline(cmdOfficeComeHomeLater)  + "],";
      result+= "["+ btnInline(cmdOfficeGoToSleepEarlier)  + "," + btnInline(cmdOfficeGoToSleepLater) + "],";
      result+= "["+ btnInline(cmdMenuSettings)                                                       + "]]";
    break;
    
    case scnSettingsWeekendTimes:
      result= "[["+ btnInline(cmdWeekendWakeUpEarlier)     + "," + btnInline(cmdWeekendWakeUpLater)    + "],";
      result+= "["+ btnInline(cmdWeekendGoToSleepEarlier)  + "," + btnInline(cmdWeekendGoToSleepLater) + "],";
      result+= "["+ btnInline(cmdMenuSettings)                                                         + "]]";
    break;
    
    case scnSettingsTemperature:
      result=  "[["+ btnInline(cmdHighTemperatureDown) + "," + btnInline(cmdHighTemperatureUp) + "],";
      result+=  "["+ btnInline(cmdLowTemperatureDown)  + "," + btnInline(cmdLowTemperatureUp)  + "],";
      result+=  "["+ btnInline(cmdMenuSettings)                                                + "]]";
    break;
    
    case scnSettingsSensorOffset:
      result=  "[["+ btnInline(cmdSensorOffsetDown) + "," + btnInline(cmdSensorOffsetUp) + "],";
      result+=  "["+ btnInline(cmdMenuSettings)                                          + "]]";
    break;
    
    default:
      // This is the main menu. If none of the other menus apply, the main menu should be returned by default
      result+= "[[" + btnInline(cmdSetpointLower) + ", " + btnInline(cmdSetpointHigher)  + "],";
      result+=  "[" + btnInline(cmdGoAway)        + ", " + btnInline(cmdSetpointAuto)    + "],";
      result+=  "[" + btnInline(cmdMenuOverruleToday)                                    + "],";
      result+=  "[" + btnInline(cmdMenuOverruleTomorrow)                                 + "],";
      result+=  "[" + btnInline(cmdMenuOverruleMultipleDays)                             + "],";
      result+=  "[" + btnInline(cmdMenuSettings)  + ", " + btnInline(cmdUpdateStatus)    + "]]";
      break;
  };

  return result;
};

String TelegramChat::handleMessage(String & message) {
  String response;

  if(message=="/start") {
    screen=scnMain;
    response = "Welcome " + userName + ".\n";
  }
  else if(message=="/log") {
    if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
      response += "Boiler request\n";
      response += " - Percentage: "+number(controllerData->boilerPercentage) + "\n";
      response += " - Boiler temperature: "+ temperature(controllerData->requestedBoilerTemperature) + "\n";
      response += " - Water temperature: " + temperature(controllerData->requestedDomesticHotWaterSetpoint) + "\n";
      response += " - Central heating: "+ String(controllerData->enableCentralHeating ? "enabled" : "disabled") + "\n";
      response += " - Hot water "+ String(controllerData->enableHotWater ? "enabled" : "disabled") + "\n";
      response += "Boiler response\n";
      response += " - Central heating active: "+ String(controllerData->centralHeatingActive ? "Y" : "N") + "\n";
      response += " - Hot water active: "+ String(controllerData->hotWaterActive ? "Y" : "N") + "\n";
      response += " - Flame on: "+ String(controllerData->flameOn ? "Y" : "N") + "\n";
      response += " - Temperature: "+temperature(controllerData->actualBoilerTemperature) + "\n";
      response += " - Water temperature: "+temperature(controllerData->actualDomesticHotWaterTemperature) + "\n";
    } else {
      TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder( controllerDataSemaphore );    
      char *pcTaskName = pcTaskGetTaskName(mutexHolder);
      Serial.printf("TelegramChat::updateLabels(): mutex held by %s\n", pcTaskName);     
    }
  }
  else if(message=="/clearlog") {
    //deleteLogFile();
    response = "Log file deleted";
  }
  else {
    response = "Command " + message + " is not recognized";
  }

  return response;
};

void TelegramChat::handleCallback(int messageID, String & callback) {
  // Recognize the response from the inline keyboard and send command to controller
  // The controller will send back a message to the Telegram queue which will then be handled by TelegramChat::displayResponse
  command_t command = cmdCommandNotRecognized ;
  for (auto const& kv : commands) {
    if(callback==kv.second.callback) {
      command=kv.first;
      break;
    }
  }

  // First, handle commands that affect controller settings
  switch(command) {
    case cmdSetpointLower:                sendMessageToController(messageID, cmdSetpointLower); break;
    case cmdSetpointHigher:               sendMessageToController(messageID, cmdSetpointHigher); break;
    case cmdSetpointAuto:                 sendMessageToController(messageID, cmdSetpointAuto); break;
    case cmdGoAway:                       sendMessageToController(messageID, cmdOverruleTodayAway); break;
    case cmdOverruleTodayWorkFromHome:    sendMessageToController(messageID, cmdOverruleTodayWorkFromHome); break;
    case cmdOverruleTodayWorkAtOffice:    sendMessageToController(messageID, cmdOverruleTodayWorkAtOffice); break;
    case cmdOverruleTodayWeekend:         sendMessageToController(messageID, cmdOverruleTodayWeekend); break;
    case cmdOverruleTodayAway:            sendMessageToController(messageID, cmdOverruleTodayAway); break;
    case cmdOverruleTodayAutomatic:       sendMessageToController(messageID, cmdOverruleTodayAutomatic); break;
    case cmdOverruleTomorrowWorkFromHome: sendMessageToController(messageID, cmdOverruleTomorrowWorkFromHome); break;
    case cmdOverruleTomorrowWorkAtOffice: sendMessageToController(messageID, cmdOverruleTomorrowWorkAtOffice); break;
    case cmdOverruleTomorrowWeekend:      sendMessageToController(messageID, cmdOverruleTomorrowWeekend); break;
    case cmdOverruleTomorrowAway:         sendMessageToController(messageID, cmdOverruleTomorrowAway); break;
    case cmdOverruleTomorrowAutomatic:    sendMessageToController(messageID, cmdOverruleTomorrowAutomatic); break;
    case cmdOverruleMultipleWorkFromHome: sendMessageToController(messageID, cmdOverruleMultipleAutomatic); break;
    case cmdOverruleMultipleWorkAtOffice: sendMessageToController(messageID, cmdOverruleMultipleWorkAtOffice); break;
    case cmdOverruleMultipleWeekend:      sendMessageToController(messageID, cmdOverruleMultipleWeekend); break;
    case cmdOverruleMultipleAway:         sendMessageToController(messageID, cmdOverruleMultipleAway); break;
    case cmdOverruleMultipleAutomatic:    sendMessageToController(messageID, cmdOverruleMultipleAutomatic); break;
    case cmdOverruleMultipleFewerDays:    sendMessageToController(messageID, cmdOverruleMultipleFewerDays); break;
    case cmdOverruleMultipleMoreDays:     sendMessageToController(messageID, cmdOverruleMultipleMoreDays); break;
    case cmdSensorOffsetDown:             sendMessageToController(messageID, cmdSensorOffsetDown); break;
    case cmdSensorOffsetUp:               sendMessageToController(messageID, cmdSensorOffsetUp); break;
    case cmdWorkFromHome:                 sendWeekScheduleToController(messageID, dayForSubmenu, dtWorkFromHome); break;
    case cmdWorkAtOffice:                 sendWeekScheduleToController(messageID, dayForSubmenu, dtWorkAtOffice); break;
    case cmdWeekend:                      sendWeekScheduleToController(messageID, dayForSubmenu, dtWeekend); break;
    case cmdAllDayAway:                   sendWeekScheduleToController(messageID, dayForSubmenu, dtAway); break;
    case cmdHomeWakeUpEarlier:            sendMessageToController(messageID, cmdHomeWakeUpEarlier); break;
    case cmdHomeWakeUpLater:              sendMessageToController(messageID, cmdHomeWakeUpLater); break;
    case cmdHomeGoToSleepEarlier:         sendMessageToController(messageID, cmdHomeGoToSleepEarlier); break;
    case cmdHomeGoToSleepLater:           sendMessageToController(messageID, cmdHomeGoToSleepLater); break;
    case cmdOfficeWakeUpEarlier:          sendMessageToController(messageID, cmdOfficeGoToSleepEarlier); break;
    case cmdOfficeWakeUpLater:            sendMessageToController(messageID, cmdOfficeWakeUpLater); break;
    case cmdOfficeLeaveEarlier:           sendMessageToController(messageID, cmdOfficeLeaveEarlier); break;
    case cmdOfficeLeaveLater:             sendMessageToController(messageID, cmdOfficeLeaveLater); break;
    case cmdOfficeComeHomeEarlier:        sendMessageToController(messageID, cmdOfficeComeHomeEarlier); break;
    case cmdOfficeComeHomeLater:          sendMessageToController(messageID, cmdOfficeComeHomeLater); break;
    case cmdOfficeGoToSleepEarlier:       sendMessageToController(messageID, cmdOfficeGoToSleepEarlier); break;
    case cmdOfficeGoToSleepLater:         sendMessageToController(messageID, cmdOfficeGoToSleepLater); break;
    case cmdWeekendWakeUpEarlier:         sendMessageToController(messageID, cmdWeekendWakeUpEarlier); break;
    case cmdWeekendWakeUpLater:           sendMessageToController(messageID, cmdWeekendWakeUpLater); break;
    case cmdWeekendGoToSleepEarlier:      sendMessageToController(messageID, cmdWeekendGoToSleepEarlier); break;
    case cmdWeekendGoToSleepLater:        sendMessageToController(messageID, cmdWeekendGoToSleepLater); break;
    case cmdHighTemperatureDown:          sendMessageToController(messageID, cmdHighTemperatureDown); break;
    case cmdHighTemperatureUp:            sendMessageToController(messageID, cmdHighTemperatureUp); break;
    case cmdLowTemperatureDown:           sendMessageToController(messageID, cmdLowTemperatureDown); break;
    case cmdLowTemperatureUp:             sendMessageToController(messageID, cmdLowTemperatureUp); break;

    default: 
      // If controller settings do not need to be changed, send the command directly to the Telegram queue
      sendMessageToTelegram(messageID, command);
  }
};

String TelegramChat::displayResponse(userEventMessage_t & message) {
  time_t now;
  struct tm * lctm;
  uint8_t dayOfWeek;
  const int BUFLEN = 80;
  char buffer[BUFLEN];

  // After handling the command by the controller, determine the new message
  // First create default message
  String response;

  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

    switch(controllerData->reasonForSetpoint) {
  
      case spWeekSchedule:
        response= "The setpoint is determined by the normal week schedule.\n";
        response+= "Today is set to be '" + String(DAY_TYPES[controllerData->regularWeek[dayOfWeek]].c_str()) + "'.\n";
      break;
  
      case spMultipleDays:
        time(&now); // Get current time
        lctm = localtime(&now);
        lctm->tm_mday += controllerData->overrideMultipleCount;
        mktime(lctm);
        strftime(buffer, BUFLEN, "%A %e %B", lctm);
        response=  "Multiple days are set to\n";
        response+= "'" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "' for\n";
        response+= String(controllerData->overrideMultipleCount) + " days ending " + buffer + "\n";
      break;
  
      case spOverrideToday:
        response= String("Today is overruled to be '") + String(DAY_TYPES[controllerData->overrideToday].c_str()) + "'.\n";
      break;
  
      case spOverrideNow:
        snprintf(buffer, BUFLEN, "%.1f", controllerData->overrideSetpoint);
        response = String("The setpoint is temporarily overruled\n");
        response+= String("by the +/- buttons at ") + buffer + "°C\n";
      break;
    } // switch(controllerData->reasonForSetpoint)
  

    // Change menu and/or change message
    switch (message.command)  {
  
      case cmdSetpointLower: response= String("The setpoint temperature is increased to ") +temperature(controllerData->temperatureSetpoint)+"\n"; break;
      case cmdSetpointAuto: response="The setpoint is set back to automatic\n"; break;
      case cmdSetpointHigher: response= String("The setpoint temperature is lowered to ") +temperature(controllerData->temperatureSetpoint)+"\n"; break;
      case cmdGoAway: response=String("Status changed to 'Away'\n"); break;
      case cmdMenuOverruleToday: screen=scnOverruleToday; break;
      case cmdMenuOverruleTomorrow: screen=scnOverruleTomorrow; break;
      case cmdMenuOverruleMultipleDays: screen=scnOverruleMultiple; break;
      case cmdMenuSettings: screen=scnSettingsMain; break;
      case cmdUpdateStatus: 
        response+= "Boiler percentage " + number(100*controllerData->boilerPercentage) + "%\n";
        response+= "Boiler temperature " + temperature(controllerData->actualBoilerTemperature)+"\n";
        break; 

      case cmdOverruleTodayWorkFromHome: screen=scnMain; break;
      case cmdOverruleTodayWorkAtOffice: screen=scnMain; break;
      case cmdOverruleTodayWeekend: screen=scnMain; break;
      case cmdOverruleTodayAway: screen=scnMain; break;
      case cmdOverruleTodayAutomatic: screen=scnMain; break;
      case cmdMenuMain: screen=scnMain; break;

      case cmdOverruleTomorrowWorkFromHome: screen=scnMain; break;
      case cmdOverruleTomorrowWorkAtOffice: screen=scnMain; break;
      case cmdOverruleTomorrowWeekend: screen=scnMain; break;
      case cmdOverruleTomorrowAway: screen=scnMain; break;
      case cmdOverruleTomorrowAutomatic: screen=scnMain; break;

      case cmdMenuWeekSchedule: screen=scnSettingsWeekSchedule; break;
      case cmdMenuHomeTimes: screen=scnSettingsHomeTimes; break;
      case cmdMenuOfficeTimes: screen=scnSettingsOfficeTimes; break;
      case cmdMenuWeekendTimes: screen=scnSettingsWeekendTimes; break;
      case cmdMenuTemperature: screen=scnSettingsTemperature; break;
      case cmdMenuSensorOffset: screen=scnSettingsSensorOffset; break;

      case cmdSunday:    dayForSubmenu=0; screen=scnSettingsDaySchedule; response="Choose settings for Sunday";    break;
      case cmdMonday:    dayForSubmenu=1; screen=scnSettingsDaySchedule; response="Choose settings for Monday";    break;
      case cmdTuesday:   dayForSubmenu=2; screen=scnSettingsDaySchedule; response="Choose settings for Tuesday";   break;
      case cmdWednesday: dayForSubmenu=3; screen=scnSettingsDaySchedule; response="Choose settings for Wednesday"; break;
      case cmdThursday:  dayForSubmenu=4; screen=scnSettingsDaySchedule; response="Choose settings for Thursday";  break;
      case cmdFriday:    dayForSubmenu=5; screen=scnSettingsDaySchedule; response="Choose settings for Friday";    break;
      case cmdSaturday:  dayForSubmenu=6; screen=scnSettingsDaySchedule; response="Choose settings for Saturday";  break;

      // case cmdMenuHome: response=""; break;
      case cmdReportBoiler: 
        screen=scnSettingsMain;
        response =  "Temperature setpoint: "+temperature(controllerData->temperatureSetpoint) + "\n";
        response += "Boiler request:\n";
        response += " - Percentage: "+number(100*controllerData->boilerPercentage) + "%\n";
        response += " - Boiler temperature: "+temperature(controllerData->requestedBoilerTemperature) + "\n";
        response += " - Water temperature: "+temperature(controllerData->requestedDomesticHotWaterSetpoint) + "\n";
        response += " - Heating: "+ String(controllerData->enableCentralHeating ? "enabled" : "disabled") + "\n";
        response += " - Water "+ String(controllerData->enableHotWater ? "enabled" : "disabled") + "\n";
        response += "Boiler response:\n";
        switch(controllerData->boilerResponse) {
          case boilerNone:     response += " - No response from boiler\n";         break;
          case boilerSuccess:  
            response += " - Successful response from boiler\n"; 
            response += " - Heating: "+ String(controllerData->centralHeatingActive ? "on" : "off") + "\n";
            response += " - Water: "+ String(controllerData->hotWaterActive ? "on" : "off") + "\n";
            response += " - Flame: "+ String(controllerData->flameOn ? "on" : "off") + "\n";
            response += " - Boiler temperature: "+temperature(controllerData->actualBoilerTemperature) + "\n";
            response += " - Water temperature: "+temperature(controllerData->actualDomesticHotWaterTemperature) + "\n";
            break;
          case boilerInvalid:  response += " - Invalid response from boiler\n";    break;
          case boilerTimeout:  response += " - Boiler response timeout\n";         break;
        };        
        break;
  
      case cmdReportTiming: 
        response = String("Keyboard interrupt counter 1: ") + String(keyboardInterruptCounter1) + "\n"; 
        response+= String("Keyboard interrupt counter 2: ") + String(keyboardInterruptCounter2) + "\n"; 
        response+= String("Keyboard interrupt counter 3: ") + String(keyboardInterruptCounter3) + "\n"; 

        response+= String("Touch value up pin: ") + String(touchRead(PIN_BUT_UP)) + "\n"; 
        response+= String("Threshold: ") + String(KEY_THRESHOLD) + "\n"; 
        
        break;
        
    } // switch (message.command) 

    // Do the remaining menu's which consist of multiple items
    if( message.command==cmdWorkFromHome or 
        message.command==cmdWorkAtOffice or 
        message.command==cmdWeekend  or 
        message.command==cmdAllDayAway ) {
      screen=scnSettingsWeekSchedule; 
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[controllerData->regularWeek[dayForSubmenu]].c_str()) + "\n"; 
    }
    else if( message.command==cmdHomeWakeUpLater or 
             message.command==cmdHomeWakeUpEarlier or 
             message.command==cmdHomeGoToSleepLater or 
             message.command==cmdHomeGoToSleepEarlier) {
      response  = "When working from home:\n";
      response += "Wake up: "+String(controllerData->workFromHomeWakeUp.str().c_str())+" sleep: "+String(controllerData->workFromHomeSleep.str().c_str())+"\n";
    }      
    else if( message.command==cmdOfficeWakeUpLater   or  message.command==cmdOfficeWakeUpEarlier or 
             message.command==cmdOfficeLeaveLater    or  message.command==cmdOfficeWakeUpLater) {
      response = "When working at the office:\n";
      response+= "Wake up: "+String(controllerData->workAtOfficeWakeUp.str().c_str())+" go out: "+String(controllerData->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(controllerData->workAtOfficeComeIn.str().c_str())+" sleep: "+String(controllerData->workAtOfficeSleep.str().c_str())+"\n"; 
    }  
    else if( message.command==cmdWeekendWakeUpLater      or  message.command==cmdWeekendWakeUpEarlier    or 
             message.command==cmdWeekendGoToSleepLater   or  message.command==cmdWeekendGoToSleepEarlier) {
        response = "On weekend days:\n";
        response+= "Wake up: "+String(controllerData->weekendWakeUp.str().c_str())+" sleep: "+String(controllerData->weekendSleep.str().c_str())+"\n";
    }
    else if ( message.command==cmdSensorOffsetUp or message.command==cmdSensorOffsetDown) {
        response = String("Sensor offset ") + temperature(controllerData->sensorOffset) + "\n"; 
    }
    else if( message.command==cmdHighTemperatureUp or message.command==cmdHighTemperatureDown or 
             message.command==cmdLowTemperatureUp  or message.command==cmdLowTemperatureDown  ) {
        response = "High temperature set to " + temperature(controllerData->highTemp) + "\n";
        response+= "Low temperature set to " + temperature(controllerData->lowTemp) + "\n";
    }
    else if( message.command==cmdOverruleMultipleWorkFromHome or message.command==cmdOverruleMultipleWorkAtOffice or 
             message.command==cmdOverruleMultipleWeekend      or message.command==cmdOverruleMultipleAway         or
             message.command==cmdOverruleMultipleAutomatic    or message.command==cmdOverruleMultipleFewerDays    or
             message.command==cmdOverruleMultipleMoreDays     or message.command==cmdMultipleDaySchedule ) {
        time(&now); // Get current time
        lctm = localtime(&now);
        lctm->tm_mday += controllerData->overrideMultipleCount;
        mktime(lctm);
        strftime(buffer, BUFLEN, "%A %e %B", lctm);
        response= "Multiple days are set to '" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "' for " + String(controllerData->overrideMultipleCount) + " days ending " + buffer + "\n";
    }    
  
    response+= "*" + currentTime() + " " + EMOTICON_THERMOMETER + " " + temperature(controllerData->measuredTemperature() ) + 
               " " + EMOTICON_BULLSEYE + " " + temperature(controllerData->temperatureSetpoint );
    if(controllerData->flameOn)        response+=EMOTICON_FLAME;
    if(controllerData->hotWaterActive) response+=EMOTICON_SHOWER;
    response+= "*";

    xSemaphoreGive( controllerDataSemaphore );
  } else {
    TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder( controllerDataSemaphore );    
    char *pcTaskName = pcTaskGetTaskName(mutexHolder);
    Serial.printf("TelegramChat::displayResponse(): mutex held by %s\n", pcTaskName);     
  }
  
  return response;
};


/**********************************************************
 *  THE TELEGRAM HANDLER MANAGES ALL CHATS                *
 **********************************************************/
class TelegramHandler {
  public:
    TelegramHandler(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue, WiFiClientSecure & securedClient, UniversalTelegramBot & bot);
    void begin( );
    void checkNewMessages();
    void handleNewMessages(int numNewMessages);
    void handleQueueCommand(userEventMessage_t & message);
  
    void updateLabels(String & chatID)                                       { listOfChats[chatID].updateLabels();                      };
    String keyboard(String & chatID)                                         { return listOfChats[chatID].keyboard();                   };
    String handleMessage (String & chatID, String & message)                 { return listOfChats[chatID].handleMessage (message);      };
    void handleCallback(String & chatID, int & messageID, String & callback) { listOfChats[chatID].handleCallback(messageID, callback); };

  protected:  
    ControllerData_t *controllerData;
    QueueHandle_t controllerQueue;
    QueueHandle_t telegramQueue;
    UniversalTelegramBot *bot;
    WiFiClientSecure *securedClient;
    std::map<String, TelegramChat> listOfChats;

    String botName;
    String botUserName;
    String botToken;
    String botChatID;    
};

TelegramHandler::TelegramHandler(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue, WiFiClientSecure & securedClient, UniversalTelegramBot & bot) { 
  this->controllerData  = & controllerData; 
  this->controllerQueue =   controllerQueue; 
  this->telegramQueue   =   telegramQueue;
  this->securedClient   = & securedClient; 
  this->bot             = & bot; 

  this->botName     = controllerData.botName;
  this->botUserName = controllerData.botUserName;
  this->botToken    = controllerData.botToken;
  this->botChatID   = controllerData.botChatID;
};

void TelegramHandler::begin() {
  // Setup secure connection for Telegram
  securedClient->setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  bot->updateToken(controllerData->botToken);

  // Add first chatID to list
  TelegramChat newTelegramChat=TelegramChat( *controllerData, controllerQueue, telegramQueue);

  listOfChats[controllerData->botChatID]=newTelegramChat;

  const String commands = F("["
                            "{\"command\":\"start\",   \"description\":\"Welcome message\"},"
                            "{\"command\":\"log\",     \"description\":\"Status of controller\"},"
                            "{\"command\":\"clearlog\",\"description\":\"Clear logdata\"}" // no comma on last command
                            "]");
  bot->setMyCommands(commands);

  //updateChatID(botChatID);
  String message="/start";
  handleMessage(controllerData->botChatID, message);
  updateLabels(controllerData->botChatID);
  bot->sendMessageWithInlineKeyboard(controllerData->botChatID, String(EMOTICON_THERMOMETER) + " Thermostat just started", "Markdown", keyboard(controllerData->botChatID));
};

void TelegramHandler::handleNewMessages(int numNewMessages){
  String message;
  
  for (int i = 0; i < numNewMessages; i++)
  {
    int messageID = bot->messages[i].message_id;
    String chatID = bot->messages[i].chat_id;
    Serial.printf("Message from chatID: %s\n", chatID);
    String fromName = bot->messages[i].from_name;
    if (fromName == "") fromName = "Guest";

    // If chat is not yet listed, create new item
    if (listOfChats.find(chatID) == listOfChats.end()) {
      TelegramChat newTelegramChat=TelegramChat(*controllerData, controllerQueue, telegramQueue);
      listOfChats[chatID]=newTelegramChat;
      Serial.printf("Chat added to list of chats\n");
    }
    
    if(bot->messages[i].type=="message") {
      Serial.printf("Handle message: %s\n", bot->messages[i].text.c_str());
      message = handleMessage(chatID, bot->messages[i].text);
      updateLabels(chatID);
      bot->sendMessageWithInlineKeyboard(chatID, message, "Markdown", keyboard(chatID));
    }
    
    else if (bot->messages[i].type=="callback_query") {
      Serial.printf("Handle callback: %s\n", bot->messages[i].text.c_str());
      handleCallback(chatID, messageID, bot->messages[i].text); // Sends commands to the controller or to the Telegram Queue
    }
  } // for i
};

void TelegramHandler::checkNewMessages() {
  int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot->getUpdates(bot->last_message_received + 1);
  }
};


void TelegramHandler::handleQueueCommand(userEventMessage_t & message) {
  // Callback queries after pressing a key have been dealt with by handleNewMessages()
  // in that process, the controller was also called. After the controller has finished its task
  // the controller posts a request in this queue to send a message back to the user

  String chatID = message.chatID;
  int & messageID = message.messageID;
  String response = listOfChats[chatID].displayResponse(message);  // Prepare a message for the user
  updateLabels(chatID);                                            // Get the latest info from the controller for the buttons
  bot->sendMessageWithInlineKeyboard(chatID, response, "Markdown", keyboard(chatID), messageID);
}


/***********************************************************************
 *  GlOBAL DECLARATIONS                                                *
 ***********************************************************************/
WiFiClientSecure securedClient;                                                                      // Secure wifi client
UniversalTelegramBot bot("", securedClient);                                                         // Driver for Telegram
TelegramHandler telegramHandler(controllerData, controllerQueue, telegramQueue, securedClient, bot); // Telegram message handler 

void runTelegram( void *pvParameters ) {
  static unsigned long lastTimeTelegramPolled = 0;
  userEventMessage_t message;
  while(true) {

    // Do not poll too often. Telegram would get bored with us
    if ( millis() - lastTimeTelegramPolled > TELEGRAM_INTERVAL ) {
      telegramHandler.checkNewMessages();
      lastTimeTelegramPolled=millis();
    }  
  
    // Check if there are messages in the telegramQueue
    if ( xQueueReceive( telegramQueue, &message, 0) == pdPASS ) {
      Serial.printf("%s > Telegram [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed);
      telegramHandler.handleQueueCommand(message);

      if( message.sender=sndController ) {
        // Apparently Telegram has asked the controller to change something.
        // Request a screen update so any users in the room are notified something was changed
        userEventMessage_t displayMessage = userEventMessage_t(sndTelegram, cmdUpdateScreen);
        xQueueSend( displayQueue, &displayMessage, ( TickType_t ) 10 );  
      }
      
    }

    vTaskDelay( 10 / portTICK_PERIOD_MS );  
  }
};

void beginTelegram() {
  telegramHandler.begin( );
    
  // Setup task
  xTaskCreatePinnedToCore(
    runTelegram,
    "Telegram",   // A name just for humans
    16384,        // This stack size can be checked & adjusted by reading the Stack Highwater
    NULL,
    2,            // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    NULL, 
    ARDUINO_RUNNING_CORE);
}
