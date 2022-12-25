#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
//#include <ESP32Ping.h>

#define TELEGRAM_INTERVAL 1000

using namespace std;

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

// All available Telegram commands
  
struct tgLabelCallback {
  String label;            // label can change depending on the context
  const String callback;   // callback strings must remain the same
};

typedef std::map<command_t, tgLabelCallback> tgCommandList;

const tgCommandList TELEGRAM_COMMANDS = {
  { cmdSetpointLower,                { String(EMOTICON_DOWN_ARROW)  + " Cooler",           "/cmdSetpointLower"                } },
  { cmdComeHome,                     { String(EMOTICON_HOUSE)       + " Come home",        "/cmdComeHome"                     } },
  { cmdSetpointHigher,               { String(EMOTICON_UP_ARROW)    + " Warmer",           "/cmdSetpointHigher"               } },
  { cmdOverruleTodayAway,            { String(EMOTICON_FOOTSTEPS)   + " Go out",           "/cmdOverruleTodayAway"            } },
  { cmdMenuOverruleToday,            { "Overrule today",                                   "/cmdMenuOverruleToday"            } },
  { cmdMenuOverruleTomorrow,         { "Overrule tomorrow",                                "/cmdMenuOverruleTomorrow"         } },
  { cmdMenuOverruleMultipleDays,     { "Overrule multiple days",                           "/cmdMenuOverruleMultipleDays"     } },
  { cmdMenuSettings,                 { String(EMOTICON_GEAR)        + " Settings",         "/cmdMenuSettings"                 } },
  { cmdUpdateStatus,                 { String(EMOTICON_STHETOSCOPE) + " Status",           "/cmdUpdateStatus"                 } },

  { cmdOverruleTodayWorkFromHome,    { String(EMOTICON_HOUSE)       + " Work from home",   "/cmdOverruleTodayWorkFromHome"    } },
  { cmdOverruleTodayWorkAtOffice,    { String(EMOTICON_OFFICE)      + " Work from office", "/cmdOverruleTodayWorkAtOffice"    } },
  { cmdOverruleTodayWeekend,         { String(EMOTICON_CALENDAR)    + " Weekend day",      "/cmdOverruleTodayWeekend"         } },
  { cmdOverruleTodayAway,            { String(EMOTICON_ISLAND)      + " All day away",     "/cmdOverruleTodayAway"            } },
  { cmdOverruleTodayAutomatic,       { String(EMOTICON_MAGIC_STICK) + " Automatic",        "/cmdOverruleTodayAutomatic"       } },
  { cmdMenuMain,                     { "Back to main menu",                                "/cmdMenuMain"                     } },

  { cmdOverruleTomorrowWorkFromHome, { String(EMOTICON_HOUSE)       + " Work from home",   "/cmdOverruleTomorrowWorkFromHome" } },
  { cmdOverruleTomorrowWorkAtOffice, { String(EMOTICON_OFFICE)      + " Work from office", "/cmdOverruleTomorrowWorkAtOffice" } },
  { cmdOverruleTomorrowWeekend,      { String(EMOTICON_CALENDAR)    + " Weekend day",      "/cmdOverruleTomorrowWeekend"      } },
  { cmdOverruleTomorrowAway,         { String(EMOTICON_ISLAND)      + " All day away",     "/cmdOverruleTomorrowAway"         } },
  { cmdOverruleTomorrowAutomatic,    { String(EMOTICON_MAGIC_STICK) + " Automatic",        "/cmdOverruleTomorrowAutomatic"    } },

  { cmdOverruleMultipleWorkFromHome, { String(EMOTICON_HOUSE)       + " Work from home",   "/cmdOverruleMultipleWorkFromHome" } },
  { cmdOverruleMultipleWorkAtOffice, { String(EMOTICON_OFFICE)      + " Work from office", "/cmdOverruleMultipleWorkAtOffice" } },
  { cmdOverruleMultipleWeekend,      { String(EMOTICON_CALENDAR)    + " Weekend day",      "/cmdOverruleMultipleWeekend"      } },
  { cmdOverruleMultipleAway,         { String(EMOTICON_ISLAND)      + " All day away",     "/cmdOverruleMultipleAway"         } },
  { cmdOverruleMultipleAutomatic,    { String(EMOTICON_MAGIC_STICK) + " Automatic",        "/cmdOverruleMultipleAutomatic"    } },
  { cmdOverruleMultipleFewerDays,    { "..days",                                           "/cmdOverruleMultipleFewerDays"    } },
  { cmdOverruleMultipleMoreDays,     { "..days",                                           "/cmdOverruleMultipleMoreDays"     } },
  { cmdOverruleMultipleForever,      { "Forever",                                          "/cmdOverruleMultipleForever"      } },

  { cmdMenuWeekSchedule,             { "Weekly schedule",                                  "/cmdMenuWeekSchedule"             } },
  { cmdMenuHomeTimes,                { String(EMOTICON_HOUSE)       + " Home times",       "/cmdMenuHomeTimes"                } },
  { cmdMenuOfficeTimes,              { String(EMOTICON_OFFICE)      + " Office times",     "/cmdMenuOfficeTimes"              } },
  { cmdMenuWeekendTimes,             { String(EMOTICON_CALENDAR)    + " Weekend times",    "/cmdMenuWeekendTimes"             } },
  { cmdMenuTemperature,              { String(EMOTICON_BULLSEYE)    + " Temperatures",     "/cmdMenuTemperature"              } },
  { cmdMenuSensorOffset,             { String(EMOTICON_THERMOMETER) + " Sensor offset",    "/cmdMenuSensorOffset"             } },
  
  { cmdReportBoiler,                 { String(EMOTICON_FLAME)       + " Boiler report",    "/cmdReportBoiler"                 } },
  { cmdReportLog,                    { String(EMOTICON_GEAR)        + " Log",              "/cmdReportLog"                    } },
  { cmdReportTiming,                 { String(EMOTICON_STOPWATCH)   + " Timing report",    "/cmdReportTiming"                 } },
  { cmdReportDebug,                  { String(EMOTICON_POINTING_FINGER) + " Debug report", "/cmdReportDebug"                  } },
  
  { cmdResetDeviceMenu,              { String(EMOTICON_WARNING) + " Restart the thermostat..", "/cmdResetDeviceMenu"          } },
  { cmdResetDeviceYes,               { String(EMOTICON_WARNING) + " Continue restart",     "/cmdResetDeviceYes"               } },
  { cmdResetDeviceNo,                { "Return without restart",                           "/cmdResetDeviceNo"                } },
  
  { cmdUpdateSoftware,               { "Receive over the air update...",                   "/cmdUpdateSoftware"               } },
    
  { cmdMonday,                       { "Monday",                                           "/cmdMonday"                       } },
  { cmdTuesday,                      { "Tuesday",                                          "/cmdTuesday"                      } },
  { cmdWednesday,                    { "Wednesday",                                        "/cmdWednesday"                    } },
  { cmdThursday,                     { "Thursday",                                         "/cmdThursday"                     } },
  { cmdFriday,                       { "Friday",                                           "/cmdFriday"                       } },
  { cmdSaturday,                     { "Saturday",                                         "/cmdSaturday"                     } },
  { cmdSunday,                       { "Sunday",                                           "/cmdSunday"                       } },

  { cmdWorkFromHome,                 { String(EMOTICON_HOUSE)       + " Work from home",   "/cmdWorkFromHome"                 } },
  { cmdWorkAtOffice,                 { String(EMOTICON_OFFICE)      + " Work from office", "/cmdWorkAtOffice"                 } },
  { cmdWeekend,                      { String(EMOTICON_CALENDAR)    + " Weekend day",      "/cmdWeekend"                      } },
  { cmdAllDayAway,                   { String(EMOTICON_ISLAND)      + " All day away",     "/cmdAllDayAway"                   } },

  { cmdHomeWakeUpEarlier,            { String(EMOTICON_ALARM_CLOCK) + " Wake up -",              "/cmdHomeWakeUpEarlier"            } },
  { cmdHomeWakeUpLater,              { String(EMOTICON_ALARM_CLOCK) + " Wake up +",              "/cmdHomeWakeUpLater"              } },
  { cmdHomeGoToSleepEarlier,         { String(EMOTICON_BED)         + " Sleep -",                "/cmdHomeGoToSleepEarlier"         } },
  { cmdHomeGoToSleepLater,           { String(EMOTICON_BED)         + " Sleep +",                "/cmdHomeGoToSleepLater"           } },
 
  { cmdOfficeWakeUpEarlier,          { String(EMOTICON_ALARM_CLOCK) + " Wake up -",              "/cmdOfficeWakeUpEarlier"          } },
  { cmdOfficeWakeUpLater,            { String(EMOTICON_ALARM_CLOCK) + " Wake up +",              "/cmdOfficeWakeUpLater"            } },
  { cmdOfficeLeaveEarlier,           { String(EMOTICON_FOOTSTEPS)   + " Go out -",               "/cmdOfficeLeaveEarlier"           } },
  { cmdOfficeLeaveLater,             { String(EMOTICON_FOOTSTEPS)   + " Go out +",               "/cmdOfficeLeaveLater"             } },
  { cmdOfficeComeHomeEarlier,        { String(EMOTICON_HOUSE)       + " Come in -",              "/cmdOfficeComeHomeEarlier"        } },
  { cmdOfficeComeHomeLater,          { String(EMOTICON_HOUSE)       + " Come in +",              "/cmdOfficeComeHomeLater"          } },
  { cmdOfficeGoToSleepEarlier,       { String(EMOTICON_BED)         + " Sleep -",                "/cmdOfficeGoToSleepEarlier"       } },
  { cmdOfficeGoToSleepLater,         { String(EMOTICON_BED)         + " Sleep +",                "/cmdOfficeGoToSleepLater"         } },

  { cmdWeekendWakeUpEarlier,         { String(EMOTICON_ALARM_CLOCK) + " Wake up -",              "/cmdWeekendWakeUpEarlier"         } },
  { cmdWeekendWakeUpLater,           { String(EMOTICON_ALARM_CLOCK) + " Wake up +",              "/cmdWeekendWakeUpLater"           } },
  { cmdWeekendGoToSleepEarlier,      { String(EMOTICON_BED)         + " Sleep -",                "/cmdWeekendGoToSleepEarlier"      } },
  { cmdWeekendGoToSleepLater,        { String(EMOTICON_BED)         + " Sleep +",                "/cmdWeekendGoToSleepLater"        } },

  { cmdHighTemperatureDown,          { "High temp -",            "/cmdHighTemperatureDown"          } },
  { cmdHighTemperatureUp,            { "High temp +",            "/cmdHighTemperatureUp"            } },
  { cmdLowTemperatureDown,           { "Low temp -",             "/cmdLowTemperatureDown"           } },
  { cmdLowTemperatureUp,             { "Low temp +",             "/cmdLowTemperatureUp"             } },

  { cmdSensorOffsetDown,             { String(EMOTICON_DOWN_ARROW) + " decrease offset",  "/cmdSensorOffsetDown"             } },
  { cmdSensorOffsetUp,               { String(EMOTICON_UP_ARROW)   + " increase offset",  "/cmdSensorOffsetUp"               } }
};  

// One TelegramChat object for each user known in the system
class TelegramChat {
  public:
    TelegramChat() {}; // Required for the class to be used in a map
    TelegramChat(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue );
    String chatID = "";
    int lastMessageID = 0;
    int lastMessageWithKeyboard = 0;
    String userName = "";

    screen_t screen = scnMain;
    uint8_t dayForSubmenu;
    tgCommandList commands = TELEGRAM_COMMANDS;

    void handleCallback(String & callback);
    String keyboard();
    void respondToUser(UniversalTelegramBot & bot, userEventMessage_t & message);
    
  protected:
    ControllerData_t *controllerData;
    QueueHandle_t controllerQueue;
    QueueHandle_t telegramQueue;
    String btnInline(command_t command);
    String currentTime();
    void sendWeekScheduleToController(uint8_t dayOfWeek, dayType_t typeOfDay);
};

TelegramChat::TelegramChat(ControllerData_t & controllerData, QueueHandle_t controllerQueue, QueueHandle_t telegramQueue ) { 
  this->chatID          =   controllerData.botChatID;
  this->userName        =   controllerData.botUserName;
  this->controllerData  = & controllerData; 
  this->controllerQueue =   controllerQueue; 
  this->telegramQueue   =   telegramQueue;
};

String TelegramChat::currentTime() {
  time_t now;
  time(&now);
  struct tm * local;
  local = localtime(&now);
  char buffer[20];
  snprintf(buffer, 20, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
  return String(buffer);
};

void TelegramChat::sendWeekScheduleToController(uint8_t dayOfWeek, dayType_t typeOfDay) {
  userEventMessage_t message(chatID, screen, cmdSetWeekSchedule);
  message.dayOfWeek = dayOfWeek;
  message.typeOfDay = typeOfDay;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::handleCallback(String & callback) {
  // This is the first response as a message is received.
  // If the command potentially changes controller settings, the command is sent to the controller
  // The controller then modifies the setting and lights up the backlight to display the change to users in the room
  // If the command is not changing any settings (i.e. it flips to a different screen), the command is directly forwarded to the Telegram queue

  // Recognize the response from the inline keyboard
  command_t command = cmdCommandNotRecognized ;
  for (auto const& kv : commands) {
    if(callback==kv.second.callback) {
      command=kv.first;
      break;
    }
  }

  if( command < cmdLastControllerCommand ) {

    // If the command changes the controller settings, it needs to be sent to the controller
    switch(command) {

      case cmdWorkFromHome: sendWeekScheduleToController(dayForSubmenu, dtWorkFromHome); break;
      case cmdWorkAtOffice: sendWeekScheduleToController(dayForSubmenu, dtWorkAtOffice); break;
      case cmdWeekend:      sendWeekScheduleToController(dayForSubmenu, dtWeekend);      break;
      case cmdAllDayAway:   sendWeekScheduleToController(dayForSubmenu, dtAway);         break;

      default: 
        sendMessage(sndTelegram, command, controllerQueue, chatID); 
    }
    
  } 
  else {
    // If controller settings do not need to be changed, send the command directly to the Telegram queue
    Serial.printf("Sending Telegram > Telegram [%s]\n", commandLabels[command].c_str() );
    sendMessage(sndTelegram, command, telegramQueue, chatID);
  }
    
};

String TelegramChat::btnInline(command_t command) {
  return "{ \"text\" : \"" + commands[command].label + "\", \"callback_data\" : \"" + commands[command].callback+ "\" }";
};

String TelegramChat::keyboard() {
  
  switch (screen) {
      
    case scnOverruleToday: 

      return "[["+ btnInline(cmdOverruleTodayWorkFromHome) + "]," +
              "["+ btnInline(cmdOverruleTodayWorkAtOffice) + "]," +
              "["+ btnInline(cmdOverruleTodayWeekend)      + "]," +
              "["+ btnInline(cmdOverruleTodayAway)         + "]," +
              "["+ btnInline(cmdOverruleTodayAutomatic)    + "]," +
              "["+ btnInline(cmdMenuMain)                  + "]]";

      break;
    
    case scnOverruleTomorrow: 
      return "[["+ btnInline(cmdOverruleTomorrowWorkFromHome) + "]," +
              "["+ btnInline(cmdOverruleTomorrowWorkAtOffice) + "]," +
              "["+ btnInline(cmdOverruleTomorrowWeekend)      + "]," +
              "["+ btnInline(cmdOverruleTomorrowAway)         + "]," +
              "["+ btnInline(cmdOverruleTomorrowAutomatic)    + "]," +
              "["+ btnInline(cmdMenuMain)                     + "]]";
      break;

    case scnOverruleMultiple: 

      commands[cmdOverruleMultipleFewerDays].label = String(controllerData->overrideMultipleCount-1) + " days";
      commands[cmdOverruleMultipleMoreDays ].label = String(controllerData->overrideMultipleCount+1) + " days";

      return "[["+ btnInline(cmdOverruleMultipleWorkFromHome) + "]," +
              "["+ btnInline(cmdOverruleMultipleWorkAtOffice) + "]," +
              "["+ btnInline(cmdOverruleMultipleWeekend)      + "]," +
              "["+ btnInline(cmdOverruleMultipleAway)         + "]," +
              "["+ btnInline(cmdOverruleMultipleFewerDays)    + "," + btnInline(cmdOverruleMultipleAutomatic) + "," + btnInline(cmdOverruleMultipleMoreDays) + "]," +
              "["+ btnInline(cmdOverruleMultipleForever)      + "]," +
              "["+ btnInline(cmdMenuMain)                     + "]]";
              
      break;

    case scnSettingsMain: 
  
      return "[[" + btnInline(cmdMenuWeekSchedule) + "," + btnInline(cmdMenuHomeTimes)    + "]," +
              "[" + btnInline(cmdMenuOfficeTimes)  + "," + btnInline(cmdMenuWeekendTimes) + "]," +
              "[" + btnInline(cmdMenuTemperature)  + "," + btnInline(cmdMenuSensorOffset) + "]," +
              "[" + btnInline(cmdReportBoiler)     + "," + btnInline(cmdReportLog)        + "]," +
              "[" + btnInline(cmdReportDebug)      + "," + btnInline(cmdReportTiming)     + "]," +
              "[" + btnInline(cmdResetDeviceMenu)                                         + "]," +
              "[" + btnInline(cmdUpdateSoftware)                                          + "]," +
              "[" + btnInline(cmdMenuMain)                                                + "]]";

      break;
      
    case scnSettingsWeekSchedule: 
      return "[["+ btnInline(cmdMonday)    + "," + btnInline(cmdTuesday)  + "]," +
              "["+ btnInline(cmdWednesday) + "," + btnInline(cmdThursday) + "]," +
              "["+ btnInline(cmdFriday)    + "," + btnInline(cmdSaturday) + "]," +
              "["+ btnInline(cmdSunday)                                   + "]," +
              "["+ btnInline(cmdMenuSettings)                             + "]]";
    
      break;
        
    case scnSettingsDaySchedule: 

      return "[["+ btnInline(cmdWorkFromHome) + "]," +
              "["+ btnInline(cmdWorkAtOffice) + "]," +
              "["+ btnInline(cmdWeekend)      + "]," +
              "["+ btnInline(cmdAllDayAway)   + "]," +
              "["+ btnInline(cmdMenuSettings) + "]]";
             
      break;
    
    case scnSettingsHomeTimes: 
      
      commands[cmdHomeWakeUpEarlier    ].label = "wake "    +String( (controllerData->workFromHomeWakeUp -timeValue_t(0,15) ).str().c_str());
      commands[cmdHomeWakeUpLater      ].label = "wake "    +String( (controllerData->workFromHomeWakeUp +timeValue_t(0,15) ).str().c_str());
      commands[cmdHomeGoToSleepEarlier ].label = "sleep "   +String( (controllerData->workFromHomeSleep  -timeValue_t(0,15) ).str().c_str());
      commands[cmdHomeGoToSleepLater   ].label = "sleep "   +String( (controllerData->workFromHomeSleep  +timeValue_t(0,15) ).str().c_str());
    
      return "[["+ btnInline(cmdHomeWakeUpEarlier)     + "," + btnInline(cmdHomeWakeUpLater)    + "]," +
              "["+ btnInline(cmdHomeGoToSleepEarlier)  + "," + btnInline(cmdHomeGoToSleepLater) + "]," +
              "["+ btnInline(cmdMenuSettings)                                                   + "]]";
      break;
      
    case scnSettingsOfficeTimes: 

      commands[ cmdOfficeWakeUpEarlier    ].label = "wake "    +String( (controllerData->workAtOfficeWakeUp -timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeWakeUpLater      ].label = "wake "    +String( (controllerData->workAtOfficeWakeUp +timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeLeaveEarlier     ].label = "go out "  +String( (controllerData->workAtOfficeGoOut  -timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeLeaveLater       ].label = "go out "  +String( (controllerData->workAtOfficeGoOut  +timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeComeHomeEarlier  ].label = "come in " +String( (controllerData->workAtOfficeComeIn -timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeComeHomeLater    ].label = "come in " +String( (controllerData->workAtOfficeComeIn +timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeGoToSleepEarlier ].label = "sleep "   +String( (controllerData->workAtOfficeSleep  -timeValue_t(0,15) ).str().c_str());
      commands[ cmdOfficeGoToSleepLater   ].label = "sleep "   +String( (controllerData->workAtOfficeSleep  +timeValue_t(0,15) ).str().c_str());
      
      return "[["+ btnInline(cmdOfficeWakeUpEarlier)     + "," + btnInline(cmdOfficeWakeUpLater)    + "]," +
              "["+ btnInline(cmdOfficeLeaveEarlier)      + "," + btnInline(cmdOfficeLeaveLater)     + "]," +
              "["+ btnInline(cmdOfficeComeHomeEarlier)   + "," + btnInline(cmdOfficeComeHomeLater)  + "]," +
              "["+ btnInline(cmdOfficeGoToSleepEarlier)  + "," + btnInline(cmdOfficeGoToSleepLater) + "]," +
              "["+ btnInline(cmdMenuSettings)                                                       + "]]";
      break;
      
    case scnSettingsWeekendTimes: 

      commands[ cmdWeekendWakeUpEarlier    ].label = "wake "    +String( (controllerData->weekendWakeUp -timeValue_t(0,15) ).str().c_str());
      commands[ cmdWeekendWakeUpLater      ].label = "wake "    +String( (controllerData->weekendWakeUp +timeValue_t(0,15) ).str().c_str());
      commands[ cmdWeekendGoToSleepEarlier ].label = "sleep "   +String( (controllerData->weekendSleep  -timeValue_t(0,15) ).str().c_str());
      commands[ cmdWeekendGoToSleepLater   ].label = "sleep "   +String( (controllerData->weekendSleep  +timeValue_t(0,15) ).str().c_str());
      
      return "[["+ btnInline(cmdWeekendWakeUpEarlier)     + "," + btnInline(cmdWeekendWakeUpLater)    + "]," +
              "["+ btnInline(cmdWeekendGoToSleepEarlier)  + "," + btnInline(cmdWeekendGoToSleepLater) + "]," +
              "["+ btnInline(cmdMenuSettings)                                                         + "]]";

      break;
      
    case scnSettingsTemperature: 

      commands[ cmdHighTemperatureDown ].label = "High " + String(controllerData->highTemp-0.5, 1) + "°C";
      commands[ cmdHighTemperatureUp   ].label = "High " + String(controllerData->highTemp+0.5, 1) + "°C";
      commands[ cmdLowTemperatureDown  ].label = "Low "  + String(controllerData->lowTemp -0.5, 1) + "°C";
      commands[ cmdLowTemperatureUp    ].label = "Low "  + String(controllerData->lowTemp +0.5, 1) + "°C";

      return "[["+ btnInline(cmdHighTemperatureDown) + "," + btnInline(cmdHighTemperatureUp) + "]," +
              "["+ btnInline(cmdLowTemperatureDown)  + "," + btnInline(cmdLowTemperatureUp)  + "]," +
              "["+ btnInline(cmdMenuSettings)                                                + "]]";

      break;

    case scnSettingsSensorOffset: 
      return "[["+ btnInline(cmdSensorOffsetDown) + "," + btnInline(cmdSensorOffsetUp) + "]," +
              "["+ btnInline(cmdMenuSettings)                                          + "]]";

      break;

    case scnResetDevice:
      return "[["+ btnInline(cmdResetDeviceYes) + "]," +
              "["+ btnInline(cmdResetDeviceNo ) + "]]";
             
      break;

    default:

      return "[[" + btnInline(cmdSetpointLower)        + "," + btnInline(cmdSetpointHigher) + "]," +
              "[" + btnInline(cmdOverruleTodayAway)    + "," + btnInline(cmdComeHome)       + "]," +
              "[" + btnInline(cmdMenuOverruleToday)                                         + "]," +
              "[" + btnInline(cmdMenuOverruleTomorrow)                                      + "]," +
              "[" + btnInline(cmdMenuOverruleMultipleDays)                                  + "]," +
              "[" + btnInline(cmdMenuSettings)         + "," + btnInline(cmdUpdateStatus)   + "]]";

  } // switch (screen)
}
void TelegramChat::respondToUser(UniversalTelegramBot & bot, userEventMessage_t & message) {
  time_t now;
  struct tm * localTime;
  timeValue_t currTime;
  int currDay;
  uint8_t dayOfWeek;
  const int BUFLEN = 80;
  char buffer[BUFLEN];
  bool showWeekSchedule = false;
  //char buffer1[BUFLEN];
  //char buffer2[BUFLEN];

  String response=""; // Mesage to the user
  
  Serial.printf("TelegramChat::respondToUser() cmd= %s\n", commandLabels[message.command].c_str() );

  // After handling the command by the controller, determine the new message
  // First create default message

  // Current day and time
  time(&now); // Get current time
  localTime = localtime(&now);
  currDay  = localTime->tm_yday;
  currTime = timeValue_t(localTime->tm_hour, localTime->tm_min);
  dayOfWeek = localTime->tm_wday;

  // STEP 1: Change the screen based on the command
  switch (message.command)  {
    case cmdOverruleTodayWorkFromHome:    screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTodayWorkAtOffice:    screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTodayWeekend:         screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTodayAway:            screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTodayAutomatic:       screen = scnMain; showWeekSchedule=true; break;
    case cmdMenuMain:                     screen = scnMain;                        break;
    case cmdOverruleTomorrowWorkFromHome: screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTomorrowWorkAtOffice: screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTomorrowWeekend:      screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTomorrowAway:         screen = scnMain; showWeekSchedule=true; break;
    case cmdOverruleTomorrowAutomatic:    screen = scnMain; showWeekSchedule=true; break;

    case cmdOverruleMultipleWorkFromHome: showWeekSchedule = true; break;
    case cmdOverruleMultipleWorkAtOffice: showWeekSchedule = true; break;
    case cmdOverruleMultipleWeekend:      showWeekSchedule = true; break;
    case cmdOverruleMultipleAway:         showWeekSchedule = true; break;
    case cmdOverruleMultipleAutomatic:    showWeekSchedule = true; break;
    case cmdOverruleMultipleFewerDays:    showWeekSchedule = true; break;
    case cmdOverruleMultipleMoreDays:     showWeekSchedule = true; break;

    case cmdMenuOverruleToday:            screen = scnOverruleToday;        break;
    case cmdMenuOverruleTomorrow:         screen = scnOverruleTomorrow;     break;
    case cmdMenuOverruleMultipleDays:     screen = scnOverruleMultiple;     break;
    case cmdMenuSettings:                 screen = scnSettingsMain;         break;
    
    case cmdMenuWeekSchedule:             screen = scnSettingsWeekSchedule; break;
    case cmdMenuHomeTimes:                screen = scnSettingsHomeTimes;    break;
    case cmdMenuOfficeTimes:              screen = scnSettingsOfficeTimes;  break;
    case cmdMenuWeekendTimes:             screen = scnSettingsWeekendTimes; break;
    case cmdMenuTemperature:              screen = scnSettingsTemperature;  break;
    case cmdMenuSensorOffset:             screen = scnSettingsSensorOffset; break;

    case cmdSunday:                       screen = scnSettingsDaySchedule; dayForSubmenu=0; break;
    case cmdMonday:                       screen = scnSettingsDaySchedule; dayForSubmenu=1; break;
    case cmdTuesday:                      screen = scnSettingsDaySchedule; dayForSubmenu=2; break;
    case cmdWednesday:                    screen = scnSettingsDaySchedule; dayForSubmenu=3; break;
    case cmdThursday:                     screen = scnSettingsDaySchedule; dayForSubmenu=4; break;
    case cmdFriday:                       screen = scnSettingsDaySchedule; dayForSubmenu=5; break;
    case cmdSaturday:                     screen = scnSettingsDaySchedule; dayForSubmenu=6; break;        

    case cmdSetWeekSchedule:              screen = scnSettingsWeekSchedule; break;
    case cmdWorkFromHome:                 screen = scnSettingsWeekSchedule; break;
    case cmdWorkAtOffice:                 screen = scnSettingsWeekSchedule; break;
    case cmdWeekend:                      screen = scnSettingsWeekSchedule; break;
    case cmdAllDayAway:                   screen = scnSettingsWeekSchedule; break;

    case cmdStartTelegram:                screen = scnMain;                 break;    

    case cmdResetDeviceMenu:              screen = scnResetDevice;          break;    
    case cmdResetDeviceYes:               ESP.restart();                    break;    
    case cmdResetDeviceNo:                screen = scnSettingsMain;         break;    
    
  } // switch (message.command) 

  
  // STEP 2: Prepare response and keyboard based on screen
  Serial.printf( "Requesting response for screen [%s]\n", screenTitle[screen].c_str() );

  switch (screen) {
  
    case scnMain: 
    
      switch(controllerData->reasonForSetpoint) {
    
        case spWeekSchedule:
          response= "The setpoint is determined by the normal week schedule.\n"
                    "Today is set to be '" + String(DAY_TYPES[controllerData->regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;
    
        case spMultipleDays:

          if( controllerData->multipleForever) {
            response=  "Multiple days are permanently set to '" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "'\n";
          }
          else {
            localTime = localtime(&now);
            localTime->tm_mday += controllerData->overrideMultipleCount;
            mktime(localTime);
            strftime(buffer, BUFLEN, "%A %e %B", localTime);
            response=  "Multiple days are set to '" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "' for " + 
                        String(controllerData->overrideMultipleCount) + " days\nending " + buffer + "\n";
          }
        break;
    
        case spOverrideToday:
          response= String("Today is overruled to be '") + String(DAY_TYPES[controllerData->overrideToday].c_str()) + "'.\n";
        break;
    
        case spOverrideNow:
          snprintf(buffer, BUFLEN, "%.1f", controllerData->overrideSetpoint);
          response = String("The setpoint is manually overruled at ") + buffer + "°C\n";
        break;
      } // switch(controllerData->reasonForSetpoint)

      break; // screen==scnMain
    
    case scnOverruleToday: 

      if( controllerData->overrideToday == dtAuto )
        response= String("Today is not overruled\n");
      else
        response= String("Today is overruled to be '") + String(DAY_TYPES[controllerData->overrideToday].c_str()) + "'.\n";

      showWeekSchedule=true; 

      break;
    
    case scnOverruleTomorrow: 
      if( controllerData->overrideTomorrow == dtAuto )
        response= String("Tomorrow is not overruled\n");
      else
        response= String("Tomorrow is overruled to be '") + String(DAY_TYPES[controllerData->overrideTomorrow].c_str()) + "'.\n";

      showWeekSchedule=true; 

      break;

    case scnOverruleMultiple: 

      if(controllerData->overrideMultiple==dtAuto) {
        response = "Override multiple not set\n";
      }
      else {
        if( controllerData->multipleForever) {
          response=  "Multiple days are permanently set to '" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "'\n";
        }
        else {
          localTime = localtime(&now);
          localTime->tm_mday += controllerData->overrideMultipleCount;
          mktime(localTime);
          strftime(buffer, BUFLEN, "%A %e %B", localTime);
          response=  "Multiple days are set to '" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "' for " + 
                      String(controllerData->overrideMultipleCount) + " days\nending " + buffer + "\n";
        }
      };

      showWeekSchedule=true; 
      
      break;

    case scnSettingsMain: 
  
      break;
      
    case scnSettingsWeekSchedule: 
      response  = "*Current weekschedule:*\n"                   
                  "Mo" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[1] ] ) + " "
                  "Tu" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[2] ] ) + " "
                  "We" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[3] ] ) + " "
                  "Th" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[4] ] ) + " "
                  "Fr" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[5] ] ) + " "
                  "Sa" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[6] ] ) + " "
                  "Su" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[0] ] ) + "\n";

      break;
        
    case scnSettingsDaySchedule: 

      switch (message.command)  {
        case cmdSunday:    response= String("Sunday is now set to be '"    ) + DAY_TYPES[controllerData->regularWeek[0]].c_str() + "'\n"; break;
        case cmdMonday:    response= String("Monday is now set to be '"    ) + DAY_TYPES[controllerData->regularWeek[1]].c_str() + "'\n"; break;
        case cmdTuesday:   response= String("Tuesday is now set to be '"   ) + DAY_TYPES[controllerData->regularWeek[2]].c_str() + "'\n"; break;
        case cmdWednesday: response= String("Wednesday is now set to be '" ) + DAY_TYPES[controllerData->regularWeek[3]].c_str() + "'\n"; break;
        case cmdThursday:  response= String("Thursday is now set to be '"  ) + DAY_TYPES[controllerData->regularWeek[4]].c_str() + "'\n"; break;
        case cmdFriday:    response= String("Friday is now set to be '"    ) + DAY_TYPES[controllerData->regularWeek[5]].c_str() + "'\n"; break;
        case cmdSaturday:  response= String("Saturday is now set to be '"  ) + DAY_TYPES[controllerData->regularWeek[6]].c_str() + "'\n"; break;
      }  

      response += "Mo" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[1] ] ) + " "
                  "Tu" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[2] ] ) + " "
                  "We" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[3] ] ) + " "
                  "Th" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[4] ] ) + " "
                  "Fr" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[5] ] ) + " "
                  "Sa" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[6] ] ) + " "
                  "Su" + String( DAYTYPE_TO_EMOTICON[ controllerData->regularWeek[0] ] ) + "\n";

      break;
    
    case scnSettingsHomeTimes: 
      response = "*Home times:*\n"
                 "Wake up at " + controllerData->workFromHomeWakeUp.str() + " sleep at " + controllerData->workFromHomeSleep.str() + "\n";
    
      break;
      
    case scnSettingsOfficeTimes: 
      response  = "*Office times:*\n"
                  "Wake up at "   + controllerData->workAtOfficeWakeUp.str() + " go out at " + controllerData->workAtOfficeGoOut.str() + "\n"
                  "come home at " + controllerData->workAtOfficeComeIn.str() + " sleep at "  + controllerData->workAtOfficeSleep.str() + "\n";

      break;
      
    case scnSettingsWeekendTimes: 
      response = "*Weekend times:*\n"
                 "Wake up at " + controllerData->weekendWakeUp.str() + " sleep at " + controllerData->weekendSleep.str() + "\n";

      break;
      
    case scnSettingsTemperature: 
      response = String("*Set temperature values:*\n") +
                 "Low temperature is " + String(controllerData->lowTemp,1) + "°C, high temperature is " + String(controllerData->highTemp, 1) + "°C\n";

      break;

    case scnSettingsSensorOffset: 
      response = "Sensor offset set to " + String(controllerData->sensorOffset, 1) + "°C measured temperature is " + String(controllerData->measuredTemperature(), 1) + "°C\n";

      break;

    case scnResetDevice:
      response = String(EMOTICON_WARNING) + "*DO YOU REALLY WANT TO REBOOT THE THERMOSTAT?*\n";

      break;

  } // switch (screen)

  // STEP 3: OVERRIDE DEFAULT RESPONSE FOR CERTAIN COMMANDS
  switch (message.command)  {

    case cmdStartTelegram:     response= String(EMOTICON_THERMOMETER) + " Welcome to the room thermostat\n"; break;    
    case cmdSetpointLower:     response= "The setpoint temperature is lowered to "   + String(controllerData->temperatureSetpoint, 1)+"°C\n"; break;
    case cmdComeHome:          response= "The setpoint is set to "                   + String(controllerData->temperatureSetpoint, 1)+"°C\n"; break;
    case cmdSetpointHigher:    response= "The setpoint temperature is increased to " + String(controllerData->temperatureSetpoint, 1)+"°C\n"; break;
    case cmdOverruleTodayAway: response= "Status changed to 'Away'\n"; break;

    case cmdUpdateStatus:      

    break; 

    case cmdReportBoiler: 
      switch(controllerData->boilerResponse) {
        case boilerNone: response = "*Boiler report:*\n- No response from boiler\n";         break;
        case boilerSuccess:  
          response  = "*Boiler request / actual:*\n"
                      "- Boiler: "          + String(controllerData->requestedBoilerTemperature, 0)               + "°C / " + String(controllerData->actualBoilerTemperature, 0) + "°C\n"
                      "- Water: "           + String(controllerData->requestedDomesticHotWaterSetpoint, 0)        + "°C / " + String(controllerData->actualDomesticHotWaterTemperature, 0)+ "°C\n"
                      "- Central heating: " + String(controllerData->enableCentralHeating ? "enabled" : "disabled") + " / " + String(controllerData->centralHeatingActive ? "active" : "inactive")+ "\n"
                      "- Hot water "        + String(controllerData->enableHotWater ? "enabled" : "disabled")       + " / " + String(controllerData->hotWaterActive ? "active" : "inactive")+ "\n"
                      "- Percentage: "      + String(100*controllerData->boilerPercentage,0) + "% Update: " + controllerData->lastBoilerUpdate.str() + "\n";
          break;
        case boilerInvalid: response = "*Boiler report:*\n - Invalid response from boiler\n";    break;
        case boilerTimeout: response = "*Boiler report:*\n - Boiler response timeout\n";         break;
      };        
      break;

    case cmdReportLog: response = controllerData->datalogger.str(); break;
        
    case cmdReportTiming: response = "*Timing report*\n" + controllerData->logBusyTime.report(); break;

    case cmdUpdateSoftware: response = "Waiting 5 minutes to receive over the air update...\n"; break;
        
    case cmdReportDebug:
      int64_t t1 = esp_timer_get_time();
      multi_heap_info_t heapInfo;
      heap_caps_get_info( &heapInfo, MALLOC_CAP_DEFAULT );
            
      response=  "*Software version: " + String(VERSION) + "*\n"
                 "- Reboot: "       + secondsToDuration(t1/1e6) + "\n"
                 "- Temperature: "  + String(controllerData->temperatureMeasurementOK      ? "connected" : "disconnected") + "\n"
                 "- Boiler: "       + String(controllerData->boilerResponse==boilerSuccess ? "connected" : "disconnected") + "\n"
                 "- Minimum heap: " + String(heapInfo.minimum_free_bytes) + "\n"; 
      break;        

  } // switch (message.command) end of STEP 3

  // For some of the screens, include the current day schedule starting today
  if( showWeekSchedule ) {
    response += "Day settings starting today:\n";
    for(int i=0; i<7; i++) response += String( DAYTYPE_TO_EMOTICON[ controllerData->dayTypes[ i ] ] ) + " ";
    response += "\n";
  }
  
  response+= "*" + currentTime() + " " +
                   EMOTICON_BULLSEYE    + " " + String(controllerData->temperatureSetpoint,   1) + "°C " +
                   EMOTICON_THERMOMETER + " " + String(controllerData->measuredTemperature(), 1) + "°C " + 
                   EMOTICON_FLAME       + " " + String(100*controllerData->boilerPercentage,  0) + "% "  + 
                   String( controllerData->hotWaterActive ? EMOTICON_SHOWER : "") +
             "*";

  if( ( lastMessageWithKeyboard < 1 ) or ( message.command == cmdStartTelegram ) ) {
    // When sending the first message, the chatID containing the keyboard is not yet known
    bot.sendMessageWithInlineKeyboard(chatID, response, "Markdown", keyboard() );
  } else {
    // Update the last message containing a keyboard
    bot.sendMessageWithInlineKeyboard(chatID, response, "Markdown", keyboard(), lastMessageWithKeyboard);
  }

  if( message.command == cmdUpdateSoftware ) {
    unsigned long updateTimeOut = millis();  
    unsigned long blinkTimeOut;  
    bool ledState;

    disableKeyboard(); // Writing to SPIFFS must not be interrupted by the capacitive touch keys

    // Wait 5 minutes for over the air software update
    while ( millis() - updateTimeOut < 5*60*1000 ){
      ArduinoOTA.handle();

      if ( millis() - blinkTimeOut > 300 ) {
        blinkTimeOut = millis();
        ledState = !ledState;
        digitalWrite(PIN_ESP32_LED, ledState);
      }  
    };
    digitalWrite(PIN_ESP32_LED, false);

    enableKeyboard();

    bot.sendMessageWithInlineKeyboard(chatID, "Timeout for over the air software update", "Markdown", keyboard(), lastMessageWithKeyboard);
  };
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
  
    void handleCallback( String & chatID, String & callback) { userConversation[chatID].handleCallback(callback);            };
    void respondToUser( String & chatID, userEventMessage_t & message);

    void enableTelegram()  { enabled=true;   };
    void disableTelegram() { enabled=false;  };
    bool telegramEnabled() { return enabled; };


  protected:  
    ControllerData_t *controllerData;
    QueueHandle_t controllerQueue;
    QueueHandle_t telegramQueue;
    UniversalTelegramBot *bot;
    WiFiClientSecure *securedClient;

    std::map<String, TelegramChat> userConversation;

    bool enabled = true; 
    
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

  this->botName         =   controllerData.botName;
  this->botUserName     =   controllerData.botUserName;
  this->botToken        =   controllerData.botToken;
  this->botChatID       =   controllerData.botChatID;
};

void TelegramHandler::begin() {
  // Setup secure connection for Telegram
  securedClient->setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  bot->updateToken(controllerData->botToken);

  const String commands = F("["
                            "{\"command\":\"start\",   \"description\":\"Welcome message\"}" // no comma on last command                            
                            "]");
  bot->setMyCommands(commands);

  // Add first chatID to list
  TelegramChat newTelegramChat=TelegramChat( *controllerData, controllerQueue, telegramQueue);
  userConversation[controllerData->botChatID]=newTelegramChat;

  // Send first message to the user
  bot->sendMessageWithInlineKeyboard( controllerData->botChatID, 
                                      String(EMOTICON_THERMOMETER) + " Thermostat just started", 
                                      "Markdown", 
                                      userConversation[controllerData->botChatID].keyboard() 
                                    );
                                    
};

void TelegramHandler::handleNewMessages(int numNewMessages){
  String response;
  
  for (int i = 0; i < numNewMessages; i++) {
    String chatID = bot->messages[i].chat_id;
    Serial.printf("Message from chatID: %s\n", chatID);

    // If chat is not yet listed, create new item
    if (userConversation.find(chatID) == userConversation.end()) {
      TelegramChat newTelegramChat=TelegramChat(*controllerData, controllerQueue, telegramQueue);
      userConversation[chatID]=newTelegramChat;
      Serial.printf("Chat added to list of chats\n");
      for (auto const& chat : userConversation) {
        Serial.printf("ID: %s [ID: %s Name: %s]\n", chat.first.c_str(), chat.second.chatID.c_str(), chat.second.userName.c_str() );
      }
    }

    // Update data in conversation
    userConversation[chatID].chatID = chatID;
    userConversation[chatID].lastMessageID = bot->messages[i].message_id;
    Serial.printf("Set chat[%s].lastMessageID: %d\n", chatID, userConversation[chatID].lastMessageID );

    userConversation[chatID].userName = bot->messages[i].from_name;
    if (userConversation[chatID].userName == "") userConversation[chatID].userName = "Guest";

    if(bot->messages[i].type=="message") {
      String message = bot->messages[i].text;
      Serial.printf("Handle message %s\n", message.c_str());

      userEventMessage_t command = userEventMessage_t(sndTelegram, cmdStartTelegram);
      respondToUser(chatID, command);

      //userConversation[chatID].responseToMessage( *bot, message);
    }    
    else if (bot->messages[i].type=="callback_query") {
      Serial.printf("Handle callback: chatID: %s  txt:%s\n", chatID.c_str(), bot->messages[i].text.c_str());
      userConversation[chatID].lastMessageWithKeyboard = bot->messages[i].message_id;
      handleCallback(chatID, bot->messages[i].text); // Sends commands to the controller or to the Telegram Queue
    }
  } // for i
};

void TelegramHandler::checkNewMessages() {
  int numNewMessages = bot->getUpdates(bot->last_message_received + 1);

  Serial.println("telegramHandler.checkNewMessages()");
  while (numNewMessages) {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot->getUpdates(bot->last_message_received + 1);
  }
};

void TelegramHandler::respondToUser( String & chatID, userEventMessage_t & message) { 
  Serial.printf("respondToUser ChatID: %s [%s]\n", chatID, commandLabels[message.command].c_str() );
  userConversation[chatID].respondToUser( *bot, message); 
};


/***********************************************************************
 *  GlOBAL DECLARATIONS                                                *
 ***********************************************************************/
WiFiClientSecure securedClient;                                                                      // Secure wifi client
UniversalTelegramBot bot("", securedClient);                                                         // Driver for Telegram, bot token not yet retrieved by ControllerData since SPIFFS not yet up
TelegramHandler telegramHandler(controllerData, controllerQueue, telegramQueue, securedClient, bot); // Telegram message handler 

void setupOTA() {
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();    
}

void startTelegram() {
  bot.maxMessageLength = 6000;
  telegramHandler.begin( );

  Serial.println("Setting up over the air update");
  setupOTA();
}

void checkTelegramIfNeeded() {
  static unsigned long lastTimeTelegramPolled = 0;
  userEventMessage_t message;

  if( telegramHandler.telegramEnabled() ) {
      
    // Do not poll too often. Telegram would get bored with us
    if ( millis() - lastTimeTelegramPolled > TELEGRAM_INTERVAL ) {
      telegramHandler.checkNewMessages();
      lastTimeTelegramPolled=millis();
    }  
      
  }  // telegramEnabled() 

  // Check if there are messages in the telegramQueue
  // This must still go un if Telegram is disabled, otherwise it cannot be enabled again
  if ( xQueueReceive( telegramQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Telegram [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed, telegramHandler.telegramEnabled() ? "Y" : "N");

    switch( message.command ) {
      case cmdDisableTelegram:
        telegramHandler.disableTelegram();
      break;

      case cmdEnableTelegram:
        telegramHandler.enableTelegram();
      break;

      default:
        String chatID = message.chatID;
            
        if( chatID.length()>1 ) {
          Serial.printf("Handle queue command for chatID: %s\n", chatID);
          telegramHandler.respondToUser(chatID, message);
        }
        
      }; // switch( message.command )
    
  }; // xQueueReceive

};
