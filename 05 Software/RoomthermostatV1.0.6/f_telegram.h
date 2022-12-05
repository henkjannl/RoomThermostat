#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
//#include <ESP32Ping.h>

#define TELEGRAM_INTERVAL 1000

using namespace std;

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

/*
const char dayTypeEmoticons( = {
  { dtWorkFromHome, {0xf0, 0x9f, 0x8f, 0xa0, 0x0} },
  { dtWorkAtOffice, {0xf0, 0x9f, 0x8f, 0xa2, 0x0} },
  { dtWeekend,      {0xf0, 0x9f, 0x93, 0x85, 0x0} },
  { dtAway,         {0xf0, 0x9f, 0x8f, 0x96, 0x0} },
  { dtAuto,         {0xf0, 0x9f, 0xaa, 0x84, 0x0} }
};
*/

#define SECS_PER_YEAR    (365.25 * 24 * 60 * 60)
#define SECS_PER_WEEK    (     7 * 24 * 60 * 60)
#define SECS_PER_DAY     (         24 * 60 * 60)
#define SECS_PER_HOUR    (              60 * 60)
#define SECS_PER_MINUTE  (                   60)

String secondsToDuration(float timeInSecs) {
  String result = "";
  uint32_t n;

  if(timeInSecs<1) return "0 sec";
    
  if( timeInSecs > SECS_PER_YEAR )   { n = int( timeInSecs / SECS_PER_YEAR );   result += String(n)+" yr ";   timeInSecs-= n*SECS_PER_YEAR;   }
  if( timeInSecs > SECS_PER_WEEK )   { n = int( timeInSecs / SECS_PER_WEEK );   result += String(n)+" wk ";   timeInSecs-= n*SECS_PER_WEEK;   }
  if( timeInSecs > SECS_PER_DAY )    { n = int( timeInSecs / SECS_PER_DAY );    result += String(n)+" day ";  timeInSecs-= n*SECS_PER_DAY;    }
  if( timeInSecs > SECS_PER_HOUR )   { n = int( timeInSecs / SECS_PER_HOUR );   result += String(n)+" hr ";   timeInSecs-= n*SECS_PER_HOUR;   }
  if( timeInSecs > SECS_PER_MINUTE ) { n = int( timeInSecs / SECS_PER_MINUTE ); result += String(n)+" min ";  timeInSecs-= n*SECS_PER_MINUTE; }
  if( timeInSecs > 1 )               { n = int( timeInSecs );                   result += String(n)+" sec";                                   }

  return result;
};


// All available Telegram commands
  
struct tgLabelCallback {
  String label;            // label can change depending on the context
  const String callback;   // callback strings must remain the same
};

typedef std::map<command_t, tgLabelCallback> tgCommandList;

const tgCommandList TELEGRAM_COMMANDS = {
  { cmdSetpointLower,                { String(EMOTICON_DOWN_ARROW)  + " Cooler",       "/cmdSetpointLower"                } },
  { cmdSetpointAuto,                 { String(EMOTICON_MAGIC_STICK) + " Auto",         "/cmdSetpointAuto"                 } },
  { cmdSetpointHigher,               { String(EMOTICON_UP_ARROW)    + " Warmer",       "/cmdSetpointHigher"               } },
  { cmdGoAway,                       { String(EMOTICON_FOOTSTEPS)   + " Go out",       "/cmdGoAway"                       } },
  { cmdMenuOverruleToday,            { "Overrule today",                               "/cmdMenuOverruleToday"            } },
  { cmdMenuOverruleTomorrow,         { "Overrule tomorrow",                            "/cmdMenuOverruleTomorrow"         } },
  { cmdMenuOverruleMultipleDays,     { "Overrule multiple days",                       "/cmdMenuOverruleMultipleDays"     } },
  { cmdMenuSettings,                 { String(EMOTICON_GEAR) + " Settings",            "/cmdMenuSettings"                 } },
  { cmdUpdateStatus,                 { String(EMOTICON_STHETOSCOPE) + " Status",       "/cmdUpdateStatus"                 } },

  { cmdOverruleTodayWorkFromHome,    { String(EMOTICON_HOUSE) + " Work from home",     "/cmdOverruleTodayWorkFromHome"    } },
  { cmdOverruleTodayWorkAtOffice,    { String(EMOTICON_OFFICE) + " Work from office",  "/cmdOverruleTodayWorkAtOffice"    } },
  { cmdOverruleTodayWeekend,         { String(EMOTICON_CALENDAR) + " Weekend",         "/cmdOverruleTodayWeekend"         } },
  { cmdOverruleTodayAway,            { String(EMOTICON_ISLAND) + " All day away",      "/cmdOverruleTodayAway"            } },
  { cmdOverruleTodayAutomatic,       { String(EMOTICON_MAGIC_STICK) + " Automatic",    "/cmdOverruleTodayAutomatic"       } },
  { cmdMenuMain,                     { "Back to main menu",                            "/cmdMenuMain"                     } },

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
  { cmdReportLog,                    { "Log",                    "/cmdReportLog"                    } },
  { cmdReportTiming,                 { "Timing report",          "/cmdReportTiming"                 } },
  { cmdReportDebug,                  { "Debug report",           "/cmdReportDebug"                  } },
    
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

  { cmdHomeWakeUpEarlier,            { "Wake up -",              "/cmdHomeWakeUpEarlier"            } },
  { cmdHomeWakeUpLater,              { "Wake up +",              "/cmdHomeWakeUpLater"              } },
  { cmdHomeGoToSleepEarlier,         { "Sleep -",                "/cmdHomeGoToSleepEarlier"         } },
  { cmdHomeGoToSleepLater,           { "Sleep +",                "/cmdHomeGoToSleepLater"           } },
 
  { cmdOfficeWakeUpEarlier,          { "Wake up -",              "/cmdOfficeWakeUpEarlier"          } },
  { cmdOfficeWakeUpLater,            { "Wake up +",              "/cmdOfficeWakeUpLater"            } },
  { cmdOfficeLeaveEarlier,           { "Go out -",               "/cmdOfficeLeaveEarlier"           } },
  { cmdOfficeLeaveLater,             { "Go out +",               "/cmdOfficeLeaveLater"             } },
  { cmdOfficeComeHomeEarlier,        { "Come in -",              "/cmdOfficeComeHomeEarlier"        } },
  { cmdOfficeComeHomeLater,          { "Come in +",              "/cmdOfficeComeHomeLater"          } },
  { cmdOfficeGoToSleepEarlier,       { "Sleep -",                "/cmdOfficeGoToSleepEarlier"       } },
  { cmdOfficeGoToSleepLater,         { "Sleep +",                "/cmdOfficeGoToSleepLater"         } },

  { cmdWeekendWakeUpEarlier,         { "Wake up -",              "/cmdWeekendWakeUpEarlier"         } },
  { cmdWeekendWakeUpLater,           { "Wake up +",              "/cmdWeekendWakeUpLater"           } },
  { cmdWeekendGoToSleepEarlier,      { "Sleep -",                "/cmdWeekendGoToSleepEarlier"      } },
  { cmdWeekendGoToSleepLater,        { "Sleep +",                "/cmdWeekendGoToSleepLater"        } },

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
    String userName = "";

    screen_t screen = scnMain;
    uint8_t dayForSubmenu;
    tgCommandList commands = TELEGRAM_COMMANDS;

    void (*messageToControllerCallback)(userEventMessage_t message);

    void   updateLabels();
    String keyboard();
    String responseToMessage(String & message);
    void   handleCallback(String & callback);
    String responseToUser(userEventMessage_t & message);
    
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

void TelegramChat::sendWeekScheduleToController(uint8_t dayOfWeek, dayType_t typeOfDay) {
  userEventMessage_t message(chatID, screen, cmdSetWeekSchedule);
  message.dayOfWeek = dayOfWeek;
  message.typeOfDay = typeOfDay;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void TelegramChat::updateLabels() {

  commands[cmdOverruleMultipleFewerDays].label = String(controllerData->overrideMultipleCount-1) + " days";
  commands[cmdOverruleMultipleMoreDays ].label = String(controllerData->overrideMultipleCount+1) + " days";
  commands[cmdHighTemperatureDown      ].label = String(controllerData->highTemp-0.5, 1) + "°C";
  commands[cmdHighTemperatureUp        ].label = String(controllerData->highTemp+0.5, 1) + "°C";
  commands[cmdLowTemperatureDown       ].label = String(controllerData->lowTemp -0.5, 1) + "°C";
  commands[cmdLowTemperatureUp         ].label = String(controllerData->lowTemp +0.5, 1) + "°C";

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
};

String TelegramChat::btnInline(command_t command) {
  return "{ \"text\" : \"" + commands[command].label + "\", \"callback_data\" : \"" + commands[command].callback+ "\" }";
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
      result= "[["+ btnInline(cmdOverruleMultipleWorkFromHome) + "]," +
               "["+ btnInline(cmdOverruleMultipleWorkAtOffice) + "]," +
               "["+ btnInline(cmdOverruleMultipleWeekend)      + "]," +
               "["+ btnInline(cmdOverruleMultipleAway)         + "]," +
               "["+ btnInline(cmdOverruleMultipleFewerDays)    + "," + btnInline(cmdOverruleMultipleAutomatic) + "," + btnInline(cmdOverruleMultipleMoreDays) + "]," +
               "["+ btnInline(cmdMenuMain) + "]]";
    break;
    
    case scnSettingsMain:
      result= "[[" + btnInline(cmdMenuWeekSchedule) + "," + btnInline(cmdMenuHomeTimes)    + "]," +
               "[" + btnInline(cmdMenuOfficeTimes)  + "," + btnInline(cmdMenuWeekendTimes) + "]," +
               "[" + btnInline(cmdMenuTemperature)  + "," + btnInline(cmdMenuSensorOffset) + "]," +
               "[" + btnInline(cmdReportBoiler)     + "," + btnInline(cmdReportLog)        + "]," +
               "[" + btnInline(cmdReportDebug)      + "," + btnInline(cmdReportTiming)     + "]," +
               "[" + btnInline(cmdMenuMain)                                                + "]]";
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

String TelegramChat::responseToMessage(String & message) {
  String response;

  if(message=="/start") {
    screen=scnMain;
    response = "Welcome " + userName + ".\n";
  }
  else if(message=="/log") {
    response  = "Boiler request / response\n"
                "- Boiler temperature: " + String(controllerData->requestedBoilerTemperature,0)                + "°C / " + String(controllerData->actualBoilerTemperature,0)                    + "°C\n"
                "- Water temperature: "  + String(controllerData->requestedDomesticHotWaterSetpoint,0)         + "°C / " + String(controllerData->actualDomesticHotWaterTemperature,0)          + "°C\n"
                "- Central heating: "    + String(controllerData->enableCentralHeating ? "enabled" : "disabled") + " / " + String(controllerData->centralHeatingActive ? "active" : "inactive") + "\n"
                "- Hot water "           + String(controllerData->enableHotWater ? "enabled" : "disabled")       + " / " + String(controllerData->hotWaterActive       ? "active" : "inactive") + "\n"
                "- Percentage: "         + String(100*controllerData->boilerPercentage, 0) + "\n";
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

void TelegramChat::handleCallback(String & callback) {
  // This is the first response as a message is received.
  // it will either send a command to the controller or it will send it directly to the telegram queue which will send a response
  // the controller will also send a message to the telegram queue to get a response
  // Recognize the response from the inline keyboard and send command to controller
  // The controller will send back a message to the Telegram queue which will then be handled by TelegramChat::responseToUser

  command_t command = cmdCommandNotRecognized ;
  for (auto const& kv : commands) {
    if(callback==kv.second.callback) {
      command=kv.first;
      break;
    }
  }

  // First, handle commands that affect controller settings

  // ToDo: make the switch shorter by the use of cmdLastControllerCommand
  
  switch(command) {
    case cmdSetpointLower:                sendMessage(sndTelegram, cmdSetpointLower,                controllerQueue, chatID); break;
    case cmdSetpointHigher:               sendMessage(sndTelegram, cmdSetpointHigher,               controllerQueue, chatID); break;
    case cmdSetpointAuto:                 sendMessage(sndTelegram, cmdSetpointAuto,                 controllerQueue, chatID); break;
    case cmdGoAway:                       sendMessage(sndTelegram, cmdOverruleTodayAway,            controllerQueue, chatID); break;
    case cmdOverruleTodayWorkFromHome:    sendMessage(sndTelegram, cmdOverruleTodayWorkFromHome,    controllerQueue, chatID); break;
    case cmdOverruleTodayWorkAtOffice:    sendMessage(sndTelegram, cmdOverruleTodayWorkAtOffice,    controllerQueue, chatID); break;
    case cmdOverruleTodayWeekend:         sendMessage(sndTelegram, cmdOverruleTodayWeekend,         controllerQueue, chatID); break;
    case cmdOverruleTodayAway:            sendMessage(sndTelegram, cmdOverruleTodayAway,            controllerQueue, chatID); break;
    case cmdOverruleTodayAutomatic:       sendMessage(sndTelegram, cmdOverruleTodayAutomatic,       controllerQueue, chatID); break;
    case cmdOverruleTomorrowWorkFromHome: sendMessage(sndTelegram, cmdOverruleTomorrowWorkFromHome, controllerQueue, chatID); break;
    case cmdOverruleTomorrowWorkAtOffice: sendMessage(sndTelegram, cmdOverruleTomorrowWorkAtOffice, controllerQueue, chatID); break;
    case cmdOverruleTomorrowWeekend:      sendMessage(sndTelegram, cmdOverruleTomorrowWeekend,      controllerQueue, chatID); break;
    case cmdOverruleTomorrowAway:         sendMessage(sndTelegram, cmdOverruleTomorrowAway,         controllerQueue, chatID); break;
    case cmdOverruleTomorrowAutomatic:    sendMessage(sndTelegram, cmdOverruleTomorrowAutomatic,    controllerQueue, chatID); break;
    case cmdOverruleMultipleWorkFromHome: sendMessage(sndTelegram, cmdOverruleMultipleAutomatic,    controllerQueue, chatID); break;
    case cmdOverruleMultipleWorkAtOffice: sendMessage(sndTelegram, cmdOverruleMultipleWorkAtOffice, controllerQueue, chatID); break;
    case cmdOverruleMultipleWeekend:      sendMessage(sndTelegram, cmdOverruleMultipleWeekend,      controllerQueue, chatID); break;
    case cmdOverruleMultipleAway:         sendMessage(sndTelegram, cmdOverruleMultipleAway,         controllerQueue, chatID); break;
    case cmdOverruleMultipleAutomatic:    sendMessage(sndTelegram, cmdOverruleMultipleAutomatic,    controllerQueue, chatID); break;
    case cmdOverruleMultipleFewerDays:    sendMessage(sndTelegram, cmdOverruleMultipleFewerDays,    controllerQueue, chatID); break;
    case cmdOverruleMultipleMoreDays:     sendMessage(sndTelegram, cmdOverruleMultipleMoreDays,     controllerQueue, chatID); break;
    case cmdSensorOffsetDown:             sendMessage(sndTelegram, cmdSensorOffsetDown,             controllerQueue, chatID); break;
    case cmdSensorOffsetUp:               sendMessage(sndTelegram, cmdSensorOffsetUp,               controllerQueue, chatID); break;
    case cmdWorkFromHome:                 sendWeekScheduleToController(dayForSubmenu, dtWorkFromHome); break;
    case cmdWorkAtOffice:                 sendWeekScheduleToController(dayForSubmenu, dtWorkAtOffice); break;
    case cmdWeekend:                      sendWeekScheduleToController(dayForSubmenu, dtWeekend);      break;
    case cmdAllDayAway:                   sendWeekScheduleToController(dayForSubmenu, dtAway);         break;
    case cmdHomeWakeUpEarlier:            sendMessage(sndTelegram, cmdHomeWakeUpEarlier,            controllerQueue, chatID); break;
    case cmdHomeWakeUpLater:              sendMessage(sndTelegram, cmdHomeWakeUpLater,              controllerQueue, chatID); break;
    case cmdHomeGoToSleepEarlier:         sendMessage(sndTelegram, cmdHomeGoToSleepEarlier,         controllerQueue, chatID); break;
    case cmdHomeGoToSleepLater:           sendMessage(sndTelegram, cmdHomeGoToSleepLater,           controllerQueue, chatID); break;
    case cmdOfficeWakeUpEarlier:          sendMessage(sndTelegram, cmdOfficeGoToSleepEarlier,       controllerQueue, chatID); break;
    case cmdOfficeWakeUpLater:            sendMessage(sndTelegram, cmdOfficeWakeUpLater,            controllerQueue, chatID); break;
    case cmdOfficeLeaveEarlier:           sendMessage(sndTelegram, cmdOfficeLeaveEarlier,           controllerQueue, chatID); break;
    case cmdOfficeLeaveLater:             sendMessage(sndTelegram, cmdOfficeLeaveLater,             controllerQueue, chatID); break;
    case cmdOfficeComeHomeEarlier:        sendMessage(sndTelegram, cmdOfficeComeHomeEarlier,        controllerQueue, chatID); break;
    case cmdOfficeComeHomeLater:          sendMessage(sndTelegram, cmdOfficeComeHomeLater,          controllerQueue, chatID); break;
    case cmdOfficeGoToSleepEarlier:       sendMessage(sndTelegram, cmdOfficeGoToSleepEarlier,       controllerQueue, chatID); break;
    case cmdOfficeGoToSleepLater:         sendMessage(sndTelegram, cmdOfficeGoToSleepLater,         controllerQueue, chatID); break;
    case cmdWeekendWakeUpEarlier:         sendMessage(sndTelegram, cmdWeekendWakeUpEarlier,         controllerQueue, chatID); break;
    case cmdWeekendWakeUpLater:           sendMessage(sndTelegram, cmdWeekendWakeUpLater,           controllerQueue, chatID); break;
    case cmdWeekendGoToSleepEarlier:      sendMessage(sndTelegram, cmdWeekendGoToSleepEarlier,      controllerQueue, chatID); break;
    case cmdWeekendGoToSleepLater:        sendMessage(sndTelegram, cmdWeekendGoToSleepLater,        controllerQueue, chatID); break;
    case cmdHighTemperatureDown:          sendMessage(sndTelegram, cmdHighTemperatureDown,          controllerQueue, chatID); break;
    case cmdHighTemperatureUp:            sendMessage(sndTelegram, cmdHighTemperatureUp,            controllerQueue, chatID); break;
    case cmdLowTemperatureDown:           sendMessage(sndTelegram, cmdLowTemperatureDown,           controllerQueue, chatID); break;
    case cmdLowTemperatureUp:             sendMessage(sndTelegram, cmdLowTemperatureUp,             controllerQueue, chatID); break;

    default: 
      // If controller settings do not need to be changed, send the command directly to the Telegram queue
      Serial.printf("Sending Telegram > Telegram [%s]\n", commandLabels[command].c_str() );
      sendMessage(sndTelegram, command, telegramQueue, chatID);
  }
};

String TelegramChat::responseToUser(userEventMessage_t & message) {
  time_t now;
  struct tm * localTime;
  timeValue_t currTime;
  int currDay;
  uint8_t dayOfWeek;
  const int BUFLEN = 80;
  char buffer[BUFLEN];

  Serial.printf("TelegramChat::responseToUser() cmd= %s\n", commandLabels[message.command].c_str() );

  // After handling the command by the controller, determine the new message
  // First create default message
  String response="";

  // Current day and time
  time(&now); // Get current time
  localTime = localtime(&now);
  currDay  = localTime->tm_yday;
  currTime = timeValue_t(localTime->tm_hour, localTime->tm_min);
  dayOfWeek = localTime->tm_wday;

  // STEP 1: Change the screen based on the command
  switch (message.command)  {
    case cmdMenuOverruleToday:            screen=scnOverruleToday; break;
    case cmdMenuOverruleTomorrow:         screen=scnOverruleTomorrow; break;
    case cmdMenuOverruleMultipleDays:     screen=scnOverruleMultiple; break;
    case cmdMenuSettings:                 screen=scnSettingsMain; break;
    case cmdOverruleTodayWorkFromHome:    screen=scnMain; break;
    case cmdOverruleTodayWorkAtOffice:    screen=scnMain; break;
    case cmdOverruleTodayWeekend:         screen=scnMain; break;
    case cmdOverruleTodayAway:            screen=scnMain; break;
    case cmdOverruleTodayAutomatic:       screen=scnMain; break;
    case cmdMenuMain:                     screen=scnMain; break;
    case cmdOverruleTomorrowWorkFromHome: screen=scnMain; break;
    case cmdOverruleTomorrowWorkAtOffice: screen=scnMain; break;
    case cmdOverruleTomorrowWeekend:      screen=scnMain; break;
    case cmdOverruleTomorrowAway:         screen=scnMain; break;
    case cmdOverruleTomorrowAutomatic:    screen=scnMain; break;
    
    case cmdMenuWeekSchedule:             screen=scnSettingsWeekSchedule; break;
    case cmdMenuHomeTimes:                screen=scnSettingsHomeTimes; break;
    case cmdMenuOfficeTimes:              screen=scnSettingsOfficeTimes; break;
    case cmdMenuWeekendTimes:             screen=scnSettingsWeekendTimes; break;
    case cmdMenuTemperature:              screen=scnSettingsTemperature; break;
    case cmdMenuSensorOffset:             screen=scnSettingsSensorOffset; break;

    case cmdSunday:                       screen=scnSettingsDaySchedule; dayForSubmenu=0; break;
    case cmdMonday:                       screen=scnSettingsDaySchedule; dayForSubmenu=1; break;
    case cmdTuesday:                      screen=scnSettingsDaySchedule; dayForSubmenu=2; break;
    case cmdWednesday:                    screen=scnSettingsDaySchedule; dayForSubmenu=3; break;
    case cmdThursday:                     screen=scnSettingsDaySchedule; dayForSubmenu=4; break;
    case cmdFriday:                       screen=scnSettingsDaySchedule; dayForSubmenu=5; break;
    case cmdSaturday:                     screen=scnSettingsDaySchedule; dayForSubmenu=6; break;        

    case cmdSetWeekSchedule:              screen=scnSettingsWeekSchedule; break;
    case cmdWorkFromHome:                 screen=scnSettingsWeekSchedule; break;
    case cmdWorkAtOffice:                 screen=scnSettingsWeekSchedule; break;
    case cmdWeekend:                      screen=scnSettingsWeekSchedule; break;
    case cmdAllDayAway:                   screen=scnSettingsWeekSchedule; break;
    
  } // switch (message.command) 
  
  // STEP 2: Prepare response based on screen
  Serial.printf( "Requesting response for screen [%s]\n", screenTitle[screen].c_str() );
  
  switch (screen) {
  
    case scnSettingsWeekSchedule: 
      response  = "*Current schedule:*\n" 
                  "Mo" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[1] ] ) + " "
                  "Tu" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[2] ] ) + " "
                  "We" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[3] ] ) + " "
                  "Th" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[4] ] ) + " "
                  "Fr" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[5] ] ) + " "
                  "Sa" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[6] ] ) + " "
                  "Su" + String( DAYTYPE__TO_EMOTICON[ controllerData->regularWeek[0] ] ) + "\n\n";
    break;
    
    case scnMain: 
    
      switch(controllerData->reasonForSetpoint) {
    
        case spWeekSchedule:
          response= "The setpoint is determined by the normal week schedule.\n"
                    "Today is set to be '" + String(DAY_TYPES[controllerData->regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;
    
        case spMultipleDays:
          localTime = localtime(&now);
          localTime->tm_mday += controllerData->overrideMultipleCount;
          mktime(localTime);
          strftime(buffer, BUFLEN, "%A %e %B", localTime);
          response=  "Multiple days are set to\n";
          response+= "'" + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "' for\n";
          response+= String(controllerData->overrideMultipleCount) + " days ending " + buffer + "\n";
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
        response= String("Today is not overruled");
      else
        response= String("Today is overruled to be '") + String(DAY_TYPES[controllerData->overrideToday].c_str()) + "'.\n";
    break;
    
    case scnOverruleTomorrow: 
      if( controllerData->overrideTomorrow == dtAuto )
        response= String("Tomorrow is not overruled");
      else
        response= String("Tomorrow is overruled to be '") + String(DAY_TYPES[controllerData->overrideTomorrow].c_str()) + "'.\n";
    break;

    case scnOverruleMultiple: 
      if(controllerData->overrideMultiple==dtAuto) {
        response = "Override multiple not set\n";
      }
      else {
        localTime = localtime(&now);
        localTime->tm_mday += controllerData->overrideMultipleCount-1; // controllerData->overrideMultipleCount = 1 means today
        mktime(localTime);
        strftime(buffer, BUFLEN, "%A %e %B", localTime);
        response  = String("Multiple days are set to '") + String(DAY_TYPES[controllerData->overrideMultiple].c_str()) + "'for " +
                    String(controllerData->overrideMultipleCount) + "' days\n" + 
                    String("last day is ") + buffer + "\n";
      };
    break;

    //scnOverruleMultipleSchedule: break; // Not used in Telegram
    
    case scnSettingsMain: 
      // response = "Settings menu\n"; 
      // menu is obvious
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
    break;
    
    case scnSettingsHomeTimes: 
      response = "Home times:\n"
                 "Wake up at " + controllerData->workFromHomeWakeUp.str() + " sleep at " + controllerData->workFromHomeSleep.str() + "\n";
      break;
      
    case scnSettingsOfficeTimes: 
      response  = "Office times:\n"
                  "Wake up at "   + controllerData->workAtOfficeWakeUp.str() + " go out at " + controllerData->workAtOfficeGoOut.str() + "\n"
                  "come home at " + controllerData->workAtOfficeComeIn.str() + " sleep at "  + controllerData->workAtOfficeSleep.str() + "\n";
      break;
      
    case scnSettingsWeekendTimes: 
      response = "Weekend times:\n"
                 "Wake up at " + controllerData->weekendWakeUp.str() + " sleep at " + controllerData->weekendSleep.str() + "\n";
      break;
      
    case scnSettingsTemperature: 
      response = "Low temperature is " + String(controllerData->lowTemp,1) + "°C, high temperature is " + String(controllerData->highTemp, 1) + "°C\n";
      break;
      
    case scnSettingsSensorOffset: 
      response = "Sensor offset set to " + String(controllerData->sensorOffset, 1) + "°C measured temperature is " + String(controllerData->measuredTemperature(), 1) + "°C\n";
    break;
  } // switch (screen)

  // STEP 3: OVERRULE RESPONSE FOR INDIVIDUAL COMMANDS
  switch (message.command)  {
    case cmdSetpointLower:  response= "The setpoint temperature is lowered to "    + String(controllerData->temperatureSetpoint, 1)+"°C\n"; break;
    case cmdSetpointAuto:   response= "The setpoint is set back to automatic\n"; break;
    case cmdSetpointHigher: response= "The setpoint temperature is increased to " + String(controllerData->temperatureSetpoint, 1)+"°C\n"; break;
    case cmdGoAway:         response= "Status changed to 'Away'\n"; break;
    case cmdUpdateStatus:   break; // The standard is good enough

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
  
  response+= "*" + currentTime() + " " +
                   EMOTICON_BULLSEYE    + " " + String(controllerData->temperatureSetpoint,   1) + "°C " +
                   EMOTICON_THERMOMETER + " " + String(controllerData->measuredTemperature(), 1) + "°C " + 
                   EMOTICON_FLAME       + " " + String(100*controllerData->boilerPercentage,  0) + "% "  + 
                   String( controllerData->hotWaterActive ? EMOTICON_SHOWER : "") +
             "*";

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
  
    void updateLabels(String & chatID)                                       { userConversation[chatID].updateLabels();                      };
    String keyboard(String & chatID)                                         { return userConversation[chatID].keyboard();                   };
    String responseToMessage (String & chatID, String & message)             { return userConversation[chatID].responseToMessage(message);   };
    void handleCallback(String & chatID, String & callback)                  { userConversation[chatID].handleCallback(callback);            };

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

  // Add first chatID to list
  TelegramChat newTelegramChat=TelegramChat( *controllerData, controllerQueue, telegramQueue);

  userConversation[controllerData->botChatID]=newTelegramChat;

  const String commands = F("["
                            "{\"command\":\"start\",   \"description\":\"Welcome message\"},"
                            "{\"command\":\"log\",     \"description\":\"Status of controller\"},"
                            "{\"command\":\"clearlog\",\"description\":\"Clear logdata\"}" // no comma on last command
                            "]");
  bot->setMyCommands(commands);

  //updateChatID(botChatID);
  String message="/start";
  responseToMessage(controllerData->botChatID, message);
  updateLabels(controllerData->botChatID);
  bot->sendMessageWithInlineKeyboard(controllerData->botChatID, String(EMOTICON_THERMOMETER) + " Thermostat just started", "Markdown", keyboard(controllerData->botChatID));
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
    userConversation[chatID].userName = bot->messages[i].from_name;
    if (userConversation[chatID].userName == "") userConversation[chatID].userName = "Guest";

    if(bot->messages[i].type=="message") {
      Serial.printf("handleNewMessages() %s\n", bot->messages[i].text.c_str());
      response = responseToMessage(chatID, bot->messages[i].text);
      updateLabels(chatID);
      bot->sendMessageWithInlineKeyboard(chatID, response, "Markdown", keyboard(chatID));
    }    
    else if (bot->messages[i].type=="callback_query") {
      Serial.printf("Handle callback: chatID: %s  txt:%s\n", chatID.c_str(), bot->messages[i].text.c_str());
      handleCallback(chatID, bot->messages[i].text); // Sends commands to the controller or to the Telegram Queue
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

  Serial.printf("TelegramHandler::handleQueueCommand() for chatID: %s\n", chatID);

  if( chatID.length()>1 ) {
    String response = userConversation[chatID].responseToUser(message);  // Prepare a message for the user
    updateLabels(chatID);                                                // Get the latest info from the controller for the buttons
  
    Serial.printf("Message length: %d\n", response.length() );
    Serial.printf("Response:\n%s\n", response.c_str() );
    
    bool sent = bot->sendMessageWithInlineKeyboard(chatID, response, "Markdown", keyboard(chatID), userConversation[chatID].lastMessageID);
  
    Serial.printf("sendMessageWithInlineKeyboard success: %s\n", sent ? "Y" : "N");  
  }

};


/***********************************************************************
 *  GlOBAL DECLARATIONS                                                *
 ***********************************************************************/
WiFiClientSecure securedClient;                                                                      // Secure wifi client
UniversalTelegramBot bot("", securedClient);                                                         // Driver for Telegram
TelegramHandler telegramHandler(controllerData, controllerQueue, telegramQueue, securedClient, bot); // Telegram message handler 

void beginTelegram() {
  bot.maxMessageLength = 3000;
  telegramHandler.begin( );
}

void checkTelegramIfNeeded() {
  static unsigned long lastTimeTelegramPolled = 0;
  userEventMessage_t message;

  if( telegramHandler.telegramEnabled() ) {
      
    // Do not poll too often. Telegram would get bored with us
    if ( millis() - lastTimeTelegramPolled > TELEGRAM_INTERVAL ) {
      Serial.println("telegramHandler.checkNewMessages()");
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
        telegramHandler.handleQueueCommand(message);
        
      }; // switch( message.command )
    
  }; // xQueueReceive

};
