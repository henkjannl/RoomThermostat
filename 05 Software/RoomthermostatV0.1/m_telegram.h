#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Ping.h>

using namespace std;

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

enum tgMenu { tmMain, tmOverruleToday, tmOverruleTomorrow, tmOverruleMultiple, tmSettingsMain, tmSensorOffset, tmSettingsWeekSchedule, 
  tmDaySchedule, tmWorkFromHomeTimes, tmWorkFromOfficeTimes, tmWeekendTimes, tmTemperatureSettings}; 

// All available Telegram commands
enum tgCommand { tcCommandNotRecognized, tcSetpointLower, tcSetpointAuto, tcSetpointHigher, tcGoAway, tcOverruleTodayMenu, tcOverruleTomorrowMenu, 
  tcOverruleMultipleMenu, tcSettingsMenu, tcSendUpdate, tcOverruleTodayWorkFromHome, tcOverruleTodayWorkFromOffice, tcOverruleTodayWeekend, 
  tcOverruleTodayAway, tcOverruleTodayAutomatic, tcMainMenu, tcOverruleTomorrowWorkFromHome, tcOverruleTomorrowWorkFromOffice, tcOverruleTomorrowWeekend, 
  tcOverruleTomorrowAway, tcOverruleTomorrowAutomatic, tcOverruleMultipleWorkFromHome, tcOverruleMultipleWorkFromOffice, tcOverruleMultipleWeekend, 
  tcOverruleMultipleAway, tcOverruleMultipleAutomatic, tcOverruleMultipleLessDays, tcOverruleMultipleMoreDays, tcWeeklyScheduleMenu, tcWorkFromHomeTimesMenu, 
  tcWorkFromOfficeTimesMenu, tcWeekendTimesMenu, tcTemperatureMenu, tcSensorOffsetMenu, tcSettingsLogdata, tcTimingReport, tcOffsetLower, tcOffsetHigher, 
  tcMondayScheduleMenu, tcTuesdayScheduleMenu, tcWednesdayScheduleMenu, tcThursdayScheduleMenu, tcFridayScheduleMenu, tcSaturdayScheduleMenu, tcSundayScheduleMenu, 
  tcWorkFromHome, tcWorkFromOffice, tcWeekend, tcAway, tcWorkFromHomeWakeUpEarlier, tcWorkFromHomeWakeUpLater, tcWorkFromHomeSleepEarlier, tcWorkFromHomeSleepLater, 
  tcWorkFromOfficeWakeUpEarlier, tcWorkFromOfficeWakeUpLater, tcWorkFromOfficeGoOutEarlier, tcWorkFromOfficeGoOutLater, tcWorkFromOfficeComeInEarlier, 
  tcWorkFromOfficeComeInLater, tcWorkFromOfficeSleepEarlier, tcWorkFromOfficeSleepLater, tcWeekendWakeUpEarlier, tcWeekendWakeUpLater, tcWeekendSleepEarlier, 
  tcWeekendSleepLater, tcHighTempLower, tcHighTempHigher, tcLowTempLower, tcLowTempHigher};
  
struct tgLabelCallback {
  String label;            // label can change depending on the context
  const String callback;   // callback strings must remain the same
};

typedef std::map<tgCommand, tgLabelCallback> tgCommandList;

const tgCommandList TELEGRAM_COMMANDS = {
  { tcSetpointLower, { "xxx°C", "/tcSetpointLower" } },
  { tcSetpointAuto, { "Auto", "/tcSetpointAuto" } },
  { tcSetpointHigher, { "xxx°C", "/tcSetpointHigher" } },
  { tcGoAway, { "Go out", "/tcGoAway" } },
  { tcOverruleTodayMenu, { "Overrule today", "/tcOverruleTodayMenu" } },
  { tcOverruleTomorrowMenu, { "Overrule tomorrow", "/tcOverruleTomorrowMenu" } },
  { tcOverruleMultipleMenu, { "Overrule multiple days", "/tcOverruleMultipleMenu" } },
  { tcSettingsMenu, { "Settings", "/tcSettingsMenu" } },
  { tcSendUpdate, { "Status", "/tcSendUpdate" } },
  { tcOverruleTodayWorkFromHome, { "Work from home", "/tcOverruleTodayWorkFromHome" } },
  { tcOverruleTodayWorkFromOffice, { "Work from office", "/tcOverruleTodayWorkFromOffice" } },
  { tcOverruleTodayWeekend, { "Weekend", "/tcOverruleTodayWeekend" } },
  { tcOverruleTodayAway, { "All day away", "/tcOverruleTodayAway" } },
  { tcOverruleTodayAutomatic, { "Automatic", "/tcOverruleTodayAutomatic" } },
  { tcMainMenu, { "Back to main menu", "/tcMainMenu" } },

  { tcOverruleTomorrowWorkFromHome, { "Work from home", "/tcOverruleTomorrowWorkFromHome" } },
  { tcOverruleTomorrowWorkFromOffice, { "Work from office", "/tcOverruleTomorrowWorkFromOffice" } },
  { tcOverruleTomorrowWeekend, { "Weekend", "/tcOverruleTomorrowWeekend" } },
  { tcOverruleTomorrowAway, { "All day away", "/tcOverruleTomorrowAway" } },
  { tcOverruleTomorrowAutomatic, { "Automatic", "/tcOverruleTomorrowAutomatic" } },

  { tcOverruleMultipleWorkFromHome, { "Work from home", "/tcOverruleMultipleWorkFromHome" } },
  { tcOverruleMultipleWorkFromOffice, { "Work from office", "/tcOverruleMultipleWorkFromOffice" } },
  { tcOverruleMultipleWeekend, { "Weekend", "/tcOverruleMultipleWeekend" } },
  { tcOverruleMultipleAway, { "All day away", "/tcOverruleMultipleAway" } },
  { tcOverruleMultipleAutomatic, { "Automatic", "/tcOverruleMultipleAutomatic" } },
  { tcOverruleMultipleLessDays, { "..days", "/tcOverruleMultipleLessDays" } },
  { tcOverruleMultipleMoreDays, { "..days", "/tcOverruleMultipleMoreDays" } },

  { tcWeeklyScheduleMenu, { "Weekly schedule", "/tcWeeklyScheduleMenu" } },
  { tcWorkFromHomeTimesMenu, { "Home times", "/tcWorkFromHomeTimesMenu" } },
  { tcWorkFromOfficeTimesMenu, { "Office times", "/tcWorkFromOfficeTimesMenu" } },
  { tcWeekendTimesMenu, { "Weekend times", "/tcWeekendTimesMenu" } },
  { tcTemperatureMenu, { "Temperatures", "/tcTemperatureMenu" } },
  { tcSensorOffsetMenu, { "Sensor offset", "/tcSensorOffsetMenu" } },
  { tcSettingsLogdata, { "Logdata", "/tcSettingsLogdata" } },
  { tcTimingReport, { "Timing report", "/tcTimingReport" } },
    
  { tcMondayScheduleMenu, { "Monday", "/tcMondayScheduleMenu" } },
  { tcTuesdayScheduleMenu, { "Tuesday", "/tcTuesdayScheduleMenu" } },
  { tcWednesdayScheduleMenu, { "Wednesday", "/tcWednesdayScheduleMenu" } },
  { tcThursdayScheduleMenu, { "Thursday", "/tcThursdayScheduleMenu" } },
  { tcFridayScheduleMenu, { "Friday", "/tcFridayScheduleMenu" } },
  { tcSaturdayScheduleMenu, { "Saturday", "/tcSaturdayScheduleMenu" } },
  { tcSundayScheduleMenu, { "Sunday", "/tcSundayScheduleMenu" } },

  { tcWorkFromHome, { "Work from home", "/tcWorkFromHome" } },
  { tcWorkFromOffice, { "Work from office", "/tcWorkFromOffice" } },
  { tcWeekend, { "Weekend day", "/tcWeekend" } },
  { tcAway, { "All day away", "/tcAway" } },

  { tcWorkFromHomeWakeUpEarlier, { "A: Wake up -", "/tcWorkFromHomeWakeUpEarlier" } },
  { tcWorkFromHomeWakeUpLater, { "A: Wake up +", "/tcWorkFromHomeWakeUpLater" } },
  { tcWorkFromHomeSleepEarlier, { "B: Sleep -", "/tcWorkFromHomeSleepEarlier" } },
  { tcWorkFromHomeSleepLater, { "B: Sleep +", "/tcWorkFromHomeSleepLater" } },

  { tcWorkFromOfficeWakeUpEarlier, { "A: Wake up -", "/tcWorkFromOfficeWakeUpEarlier" } },
  { tcWorkFromOfficeWakeUpLater, { "A: Wake up +", "/tcWorkFromOfficeWakeUpLater" } },
  { tcWorkFromOfficeGoOutEarlier, { "B: Go out -", "/tcWorkFromOfficeGoOutEarlier" } },
  { tcWorkFromOfficeGoOutLater, { "B: Go out +", "/tcWorkFromOfficeGoOutLater" } },
  { tcWorkFromOfficeComeInEarlier, { "C: Come in -", "/tcWorkFromOfficeComeInEarlier" } },
  { tcWorkFromOfficeComeInLater, { "C: Come in +", "/tcWorkFromOfficeComeInLater" } },
  { tcWorkFromOfficeSleepEarlier, { "D: Sleep -", "/tcWorkFromOfficeSleepEarlier" } },
  { tcWorkFromOfficeSleepLater, { "D: Sleep +", "/tcWorkFromOfficeSleepLater" } },

  { tcWeekendWakeUpEarlier, { "A: Wake up -", "/tcWeekendWakeUpEarlier" } },
  { tcWeekendWakeUpLater, { "A: Wake up +", "/tcWeekendWakeUpLater" } },
  { tcWeekendSleepEarlier, { "B: Sleep -", "/tcWeekendSleepEarlier" } },
  { tcWeekendSleepLater, { "B: Sleep +", "/tcWeekendSleepLater" } },

  { tcHighTempLower, { "High temp ..°C", "/tcHighTempLower" } },
  { tcHighTempHigher, { "High temp ..°C", "/tcHighTempHigher" } },
  { tcLowTempLower, { "Low temp ..°C", "/tcLowTempLower" } },
  { tcLowTempHigher, { "Low temp ..°C", "/tcLowTempHigher" } },

  { tcOffsetLower, { "Offset down", "/tcOffsetLower" } },
  { tcOffsetHigher, { "Offset up", "/tcOffsetHigher" } }
};  


class TelegramChat {
  public:
    TelegramChat() {}; // Required for the class to be used in a map
    TelegramChat(DataBus & databus, Controller & controller) { this->databus = & databus; this->controller = & controller; };
    String chatID = "";
    String userName = "";
    bool   userWantsUpdates = true;
    
    tgMenu currentMenu = tmMain;
    tgCommandList commands = TELEGRAM_COMMANDS;
  
    void updateLabels();
    String keyboard();
    String handleMessage (String & message);
    String handleCallback(String & callback);
    
  protected:
    DataBus *databus;
    Controller *controller;
    String btnInline(tgCommand command);
    String temperature(float temp);
    String number(float f);
    String currentTime();
};

void TelegramChat::updateLabels() {
  commands[tcSetpointLower ].label = "Down (" + temperature(databus->overrideSetpoint-0.5) + ")";
  commands[tcSetpointHigher].label = "Up ("   + temperature(databus->overrideSetpoint+0.5) + ")";
  commands[tcOverruleMultipleLessDays].label = String(databus->overrideMultipleCount-1) + " days";
  commands[tcOverruleMultipleMoreDays].label = String(databus->overrideMultipleCount+1) + " days";
  commands[tcHighTempLower ].label = temperature(databus->highTemp-0.5);
  commands[tcHighTempHigher].label = temperature(databus->highTemp+0.5);
  commands[tcLowTempLower  ].label = temperature(databus->lowTemp -0.5);
  commands[tcLowTempHigher ].label = temperature(databus->lowTemp +0.5);

  commands[tcWorkFromHomeWakeUpEarlier  ].label = "wake " +String( (databus->workFromHomeWakeUp-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeWakeUpLater    ].label = "wake " +String( (databus->workFromHomeWakeUp+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeSleepEarlier   ].label = "sleep "+String( (databus->workFromHomeSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeSleepLater     ].label = "sleep "+String( (databus->workFromHomeSleep +timeValue_t(0,15)).str().c_str());

  commands[tcWorkFromOfficeWakeUpEarlier].label = "wake "    +String( (databus->workAtOfficeWakeUp-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeWakeUpLater  ].label = "wake "    +String( (databus->workAtOfficeWakeUp+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeGoOutEarlier ].label = "go out "  +String( (databus->workAtOfficeGoOut-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeGoOutLater   ].label = "go out "  +String( (databus->workAtOfficeGoOut+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeComeInEarlier].label = "come in " +String( (databus->workAtOfficeComeIn-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeComeInLater  ].label = "come in " +String( (databus->workAtOfficeComeIn+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeSleepEarlier ].label = "sleep "   +String( (databus->workAtOfficeSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeSleepLater   ].label = "sleep "   +String( (databus->workAtOfficeSleep +timeValue_t(0,15)).str().c_str());

  commands[tcWeekendWakeUpEarlier       ].label = "wake "   +String( (databus->weekendWakeUp -timeValue_t(0,15)).str().c_str());
  commands[tcWeekendWakeUpLater         ].label = "wake "   +String( (databus->weekendWakeUp +timeValue_t(0,15)).str().c_str());
  commands[tcWeekendSleepEarlier        ].label = "sleep "  +String( (databus->weekendSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWeekendSleepLater          ].label = "sleep "  +String( (databus->weekendSleep +timeValue_t(0,15)).str().c_str());
};

String TelegramChat::btnInline(tgCommand command) {
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
  
  switch(currentMenu) {

    // Main menu is chosen as default option and therefore missing here

    case tmOverruleToday:
      result = "[["+ btnInline(tcOverruleTodayWorkFromHome) + "],";
      result+=  "["+ btnInline(tcOverruleTodayWorkFromOffice) + "],";
      result+=  "["+ btnInline(tcOverruleTodayWeekend) + "],";
      result+=  "["+ btnInline(tcOverruleTodayAway) + "],";
      result+=  "["+ btnInline(tcOverruleTodayAutomatic) + "],";
      result+=  "["+ btnInline(tcMainMenu) + "]]";
    break;
    
    case tmOverruleTomorrow:
      result = "[["+ btnInline(tcOverruleTomorrowWorkFromHome) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowWorkFromOffice) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowWeekend) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowAway) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowAutomatic) + "],";
      result+=  "["+ btnInline(tcMainMenu) + "]]";
    break;
    
    case tmOverruleMultiple:
      result= "[["+ btnInline(tcOverruleMultipleWorkFromHome) + "],";
      result+= "["+ btnInline(tcOverruleMultipleWorkFromOffice) + "],";
      result+= "["+ btnInline(tcOverruleMultipleWeekend) + "],";
      result+= "["+ btnInline(tcOverruleMultipleAway) + "],";
      result+= "["+ btnInline(tcOverruleMultipleLessDays) + ", " + btnInline(tcOverruleMultipleAutomatic) + ", " + 
                   btnInline(tcOverruleMultipleMoreDays) + "],";
      result+= "["+ btnInline(tcMainMenu) + "]]";
    break;
    
    case tmSettingsMain:
      result= "[["+ btnInline(tcWeeklyScheduleMenu)      + "," + btnInline(tcWorkFromHomeTimesMenu) + "],";
      result+= "["+ btnInline(tcWorkFromOfficeTimesMenu) + "," + btnInline(tcWeekendTimesMenu)      + "],";
      result+= "["+ btnInline(tcTemperatureMenu)         + "," + btnInline(tcSensorOffsetMenu)      + "],";
      result+= "["+ btnInline(tcSettingsLogdata)         + "," + btnInline(tcTimingReport)          + "],"; 
      result+= "["+ btnInline(tcMainMenu)              + "]]";
    break;
    
    case tmSettingsWeekSchedule:
      result= "[["+ btnInline(tcMondayScheduleMenu)    + "," + btnInline(tcTuesdayScheduleMenu)  + "],";
      result+= "["+ btnInline(tcWednesdayScheduleMenu) + "," + btnInline(tcThursdayScheduleMenu) + "],";
      result+= "["+ btnInline(tcFridayScheduleMenu)    + "," + btnInline(tcSaturdayScheduleMenu) + "],";
      result+= "["+ btnInline(tcSundayScheduleMenu) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmDaySchedule:
      result= "[["+ btnInline(tcWorkFromHome) + "],";
      result+= "["+ btnInline(tcWorkFromOffice) + "],";
      result+= "["+ btnInline(tcWeekend) + "],";
      result+= "["+ btnInline(tcAway) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmWorkFromHomeTimes:
      result= "[["+ btnInline(tcWorkFromHomeWakeUpEarlier) + "," + btnInline(tcWorkFromHomeWakeUpLater) + "],";
      result+= "["+ btnInline(tcWorkFromHomeSleepEarlier)  + "," + btnInline(tcWorkFromHomeSleepLater) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmWorkFromOfficeTimes:
      result= "[["+ btnInline(tcWorkFromOfficeWakeUpEarlier) + "," + btnInline(tcWorkFromOfficeWakeUpLater) + "],";
      result+= "["+ btnInline(tcWorkFromOfficeGoOutEarlier)  + "," + btnInline(tcWorkFromOfficeGoOutLater) + "],";
      result+= "["+ btnInline(tcWorkFromOfficeComeInEarlier) + "," + btnInline(tcWorkFromOfficeComeInLater) + "],";
      result+= "["+ btnInline(tcWorkFromOfficeSleepEarlier)  + "," + btnInline(tcWorkFromOfficeSleepLater) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmWeekendTimes:
      result= "[["+ btnInline(tcWeekendWakeUpEarlier) + "," + btnInline(tcWeekendWakeUpLater) + "],";
      result+= "["+ btnInline(tcWeekendSleepEarlier)  + "," + btnInline(tcWeekendSleepLater) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmTemperatureSettings:
      result=  "[["+ btnInline(tcHighTempLower) + "," + btnInline(tcHighTempHigher) + "],";
      result+=  "["+ btnInline(tcLowTempLower)  + "," + btnInline(tcLowTempHigher) + "],";
      result+=  "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    case tmSensorOffset:
      result=  "[["+ btnInline(tcOffsetLower) + "," + btnInline(tcOffsetHigher) + "],";
      result+=  "["+ btnInline(tcSettingsMenu) + "]]";
    break;
    
    default:
      // This is the main menu. If none of the other menus apply, the main menu should be returned by default
      result+= "[[" + btnInline(tcSetpointLower) + ", " +  btnInline(tcSetpointHigher) + "],";
      result+=  "[" + btnInline(tcGoAway) + ", " + btnInline(tcSetpointAuto) + "],";
      result+=  "[" + btnInline(tcOverruleTodayMenu) + "],";
      result+=  "[" + btnInline(tcOverruleTomorrowMenu) + "],";
      result+=  "[" + btnInline(tcOverruleMultipleMenu) + "],";
      result+=  "[" + btnInline(tcSettingsMenu) + ", " + btnInline(tcSendUpdate) + "]]";
      break;
  };

  return result;
};

String TelegramChat::handleMessage(String & message) {
  String response;

  if(message=="/start") {
    currentMenu=tmMain;
    response = "Welcome " + userName + ".\n";
  }
  else if(message=="/log") {
    response += "Temperature setpoint: "+databus->dpTemperatureSetpoint.getValue() + "\n";
    response += "Boiler request\n";
    response += " - Percentage: "+number(databus->boilerPercentage) + "\n";
    response += " - Boiler temperature: "+ temperature(databus->requestedBoilerTemperature) + "\n";
    response += " - Water temperature: " + temperature(databus->requestedDomesticHotWaterSetpoint) + "\n";
    response += " - Central heating: "+ String(databus->enableCentralHeating ? "enabled" : "disabled") + "\n";
    response += " - Hot water "+ String(databus->enableHotWater ? "enabled" : "disabled") + "\n";
    response += "Boiler response\n";
    response += " - Response: "+ String(databus->responseFromBoiler) + "\n";
    response += " - Central heating active: "+ String(databus->centralHeatingActive ? "Y" : "N") + "\n";
    response += " - Hot water active: "+ String(databus->hotWaterActive ? "Y" : "N") + "\n";
    response += " - Flame on: "+ String(databus->flameOn ? "Y" : "N") + "\n";
    response += " - Temperature: "+temperature(databus->actualBoilerTemperature) + "\n";
    response += " - Water temperature: "+temperature(databus->actualDomesticHotWaterTemperature) + "\n";
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

String TelegramChat::handleCallback(String & callback) {
  time_t now;
  struct tm * lctm;
  uint8_t dayOfWeek;
  static uint8_t dayForSubmenu;
  const int BUFLEN = 80;
  char buffer[BUFLEN];

  // Recognize the response from the inline keyboard
  tgCommand command = tcCommandNotRecognized;
  for (auto const& kv : commands) {
    if(callback==kv.second.callback) {
      command=kv.first;
      Serial.printf("Callback %s recognized\n", kv.second.callback.c_str() );
      break;
    }
  }

  // Do what needs to be done and determine the new message
  String response;

  switch(command) {
    case tcCommandNotRecognized:
      response="Command from callback function not recognized\n";
      break;
      
    case tcSetpointLower:
      controller->setpointLower();
      response= String("The setpoint temperature is lowered to ") +temperature(databus->temperatureSetpoint)+"\n";
      break;
      
    case tcSetpointHigher:
      controller->setpointHigher();
      response= String("The setpoint temperature is increased to ")+temperature(databus->temperatureSetpoint)+"\n";
      break;
      
    case tcSetpointAuto:
      controller->setpointAuto();
      response = "The current temperature setpoint is set to 'Auto'.\n";
      break;
      
    case tcGoAway:
      controller->overruleTodayAway();
      response = "Today is set to 'Away'.\n";
      break;
      
    case tcOverruleTodayMenu:
      currentMenu=tmOverruleToday;
      response= "Select the type of day for today\n";
      break;
      
    case tcOverruleTomorrowMenu:
      currentMenu=tmOverruleTomorrow;
      response= "Select the type of day for tomorrow\n";
      break;
      
    case tcOverruleMultipleMenu:
      currentMenu=tmOverruleMultiple;
      if(databus->overrideMultiple==dtAuto) {
        response = String("Multiple days set to ") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "\n";
      }
      else {
        time(&now); // Get current time
        lctm = localtime(&now);
        lctm->tm_mday += databus->overrideMultipleCount;
        mktime(lctm);
        strftime(buffer, BUFLEN, "%A %e %B", lctm);
        response = String("Multiple days set to ") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + " for " + String(databus->overrideMultipleCount) + " days until " + buffer + "\n";
      };
      break;
      
    case tcSettingsMenu:
      currentMenu=tmSettingsMain;
      response= "Change settings\n";
      break;

    case tcSendUpdate:
      currentMenu=tmMain;

      time(&now); // Get current time
      lctm = localtime(&now); // Convert time to local time
      dayOfWeek = lctm->tm_wday;

      switch(databus->reasonForSetpoint) {

        case spWeekSchedule:
          response = String("The setpoint is determined by the normal week schedule.\n");
          response+= "Today is set to be '" + String(DAY_TYPES[databus->regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;

        case spMultipleDays:
          response= String("Multiple days are set to '") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days.\n";
        break;

        case spOverrideToday:
          response= String("Today is overruled to be '") + String(DAY_TYPES[databus->overrideToday].c_str()) + "'.\n";
        break;

        case spOverrideNow:
          snprintf(buffer, BUFLEN, "%.1f", databus->overrideSetpoint);
          response= String("The setpoint is temporarily overruled by the +/- buttons at ")+ buffer + "°C\n";
        break;
      }

      snprintf(buffer, BUFLEN, "Boiler percentage %d%%, flame %s\n", int(100*databus->boilerPercentage), databus->flameOn ? "on" : "off");
      response+= String(buffer);
      break;

    case tcOverruleTodayWorkFromHome:
      controller->overruleTodayWorkFromHome();
      response = "Today is set to 'work from Home'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayWorkFromOffice:
      controller->overruleTodayWorkFromOffice();
      response = "Today is set to 'work at the office'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayWeekend:
      controller->overruleTodayWeekend();
      response = "Today is set as weekend day\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayAway:
      controller->overruleTodayAway();
      response = "Today is set as a day away\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayAutomatic:
      controller->overruleTodayAutomatic();
      response = String("Today is set to automatic\n");

      switch(databus->reasonForSetpoint) {

        case spWeekSchedule:
          response+= "The setpoint is determined by the normal week schedule.\n";
          response+= "Today is set to be '" + String(DAY_TYPES[databus->regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;

        case spMultipleDays:
          response+= "Multiple days are set to '" + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days.\n";
        break;

        case spOverrideToday:
          response+= String("Today is overruled to be '") + String(DAY_TYPES[databus->overrideToday].c_str()) + "'.\n";
        break;

        case spOverrideNow:
          snprintf(buffer, BUFLEN, "%.1f", databus->overrideSetpoint);
          response+= String("The setpoint is temporarily overruled by the +/- buttons at ") + buffer + "°C\n";
        break;
      }

      currentMenu=tmMain;
      break;

    case tcMainMenu:
      currentMenu=tmMain;
      response= "Main menu\n";
      break;

    case tcOverruleTomorrowWorkFromHome:
      controller->overruleTomorrowWorkFromHome();
      response = "Tomorrow is set to 'work from home'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowWorkFromOffice:
      controller->overruleTomorrowWorkFromOffice();
      response = "Tomorrow is set to 'work at the office'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowWeekend:
      controller->overruleTomorrowWeekend();
      response = "Tomorrow is set to 'weekend day'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowAway:
      controller->overruleTomorrowAway();
      response = "Tomorrow is set as 'day away'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowAutomatic:
      controller->overruleTomorrowAutomatic();
      response = "Tomorrow is set to automatic\n";
      currentMenu=tmMain;
      break;

    case tcOverruleMultipleWorkFromHome:
      controller->overruleMultipleWorkFromHome();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleWorkFromOffice:
      controller->overruleMultipleWorkFromOffice();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleWeekend:
      controller->overruleMultipleWeekend();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleAway:
      controller->overruleMultipleAway();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleAutomatic:
      controller->overruleMultipleAutomatic();
      response = "Multiple days set to '" + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "\n";
      break;

    case tcOverruleMultipleLessDays:
      controller->overruleMultipleLessDays();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days ending " + buffer + "\n";
      break;

    case tcOverruleMultipleMoreDays:
      controller->overruleMultipleMoreDays();

      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += databus->overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, BUFLEN, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[databus->overrideMultiple].c_str()) + "' for " + String(databus->overrideMultipleCount) + " days ending " + buffer + "\n";
      break;

    case tcWeeklyScheduleMenu:
      currentMenu=tmSettingsWeekSchedule;
      response= "Choose a day for the weekly schedule\n";
      break;

    case tcWorkFromHomeTimesMenu:
      currentMenu=tmWorkFromHomeTimes;
      response = "When working from home:\n";
      response+= "Wake up: "+String(databus->workFromHomeWakeUp.str().c_str())+" sleep: "+String(databus->workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeTimesMenu:
      currentMenu=tmWorkFromOfficeTimes;
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWeekendTimesMenu:
      currentMenu=tmWeekendTimes;
      response = "On weekend days:\n";
      response+= "Wake up: "+String(databus->weekendWakeUp.str().c_str())+" sleep: "+String(databus->weekendSleep.str().c_str())+"\n";
      break;

    case tcTemperatureMenu:
      currentMenu=tmTemperatureSettings;
      response = String("High temp set to ") + temperature(databus->highTemp) + "\n";
      response+= String("Low temp set to ")  + temperature(databus->lowTemp)  + "\n";
      break;

    case tcSensorOffsetMenu:
      currentMenu=tmSensorOffset;
      response = String("Sensor offset ") + temperature(databus->temperatureSensorOffset) + "\n";
      break;

    case tcOffsetLower:
      currentMenu=tmSensorOffset;
      controller->decreaseSensorOffset();
      response = String("Sensor offset ") + temperature(databus->temperatureSensorOffset) + "\n";
      break;

    case tcOffsetHigher:
      currentMenu=tmSensorOffset;
      controller->increaseSensorOffset();
      response = String("Sensor offset set to ") + temperature(databus->temperatureSensorOffset) + "\n";
      break;

    case tcSettingsLogdata:
      currentMenu=tmSettingsMain;
      response =  "Temperature setpoint: "+temperature(databus->temperatureSetpoint) + "\n";
      response += "Boiler request:\n";
      response += " - Percentage: "+number(100*databus->boilerPercentage) + "%\n";
      response += " - Boiler temperature: "+temperature(databus->requestedBoilerTemperature) + "\n";
      response += " - Water temperature: "+temperature(databus->requestedDomesticHotWaterSetpoint) + "\n";
      response += " - Heating: "+ String(databus->enableCentralHeating ? "enabled" : "disabled") + "\n";
      response += " - Water "+ String(databus->enableHotWater ? "enabled" : "disabled") + "\n";
      response += "Boiler response:\n";
      response += " - "+ String(databus->responseFromBoiler) + "\n";
      response += " - Heating: "+ String(databus->centralHeatingActive ? "on" : "off") + "\n";
      response += " - Water: "+ String(databus->hotWaterActive ? "on" : "off") + "\n";
      response += " - Flame: "+ String(databus->flameOn ? "on" : "off") + "\n";
      response += " - Boiler temperature: "+temperature(databus->actualBoilerTemperature) + "\n";
      response += " - Water temperature: "+temperature(databus->actualDomesticHotWaterTemperature) + "\n";
      break;

    case tcTimingReport:
      currentMenu=tmMain;
      response += databus->logBusyTime.report();
      break;

    case tcMondayScheduleMenu:
      dayForSubmenu=1;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcTuesdayScheduleMenu:
      dayForSubmenu=2;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcWednesdayScheduleMenu:
      dayForSubmenu=3;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcThursdayScheduleMenu:
      dayForSubmenu=4;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcFridayScheduleMenu:
      dayForSubmenu=5;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcSaturdayScheduleMenu:
      dayForSubmenu=6;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";      
      break;

    case tcSundayScheduleMenu:
      dayForSubmenu=0;
      currentMenu=tmDaySchedule;
      response = "Set schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "\n";
      break;

    case tcWorkFromHome:
      controller->setWeekSchedule(dayForSubmenu, dtWorkFromHome);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[databus->regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWorkFromOffice:
      controller->setWeekSchedule(dayForSubmenu, dtWorkAtOffice);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[databus->regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWeekend:
      controller->setWeekSchedule(dayForSubmenu, dtWeekend);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[databus->regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcAway:
      controller->setWeekSchedule(dayForSubmenu, dtAway);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[databus->regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWorkFromHomeWakeUpEarlier:
      controller->workFromHomeWakeUpEarlier();
      response = "When working from home:\n";
      response+= "Wake up: "+String(databus->workFromHomeWakeUp.str().c_str())+" sleep: "+String(databus->workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeWakeUpLater:
      controller->workFromHomeWakeUpLater();
      response = "When working from home:\n";
      response+= "Wake up: "+String(databus->workFromHomeWakeUp.str().c_str())+" sleep: "+String(databus->workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeSleepEarlier:
      controller->workFromHomeSleepEarlier();
      response = "When working from home:\n";
      response+= "Wake up: "+String(databus->workFromHomeWakeUp.str().c_str())+" sleep: "+String(databus->workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeSleepLater:
      controller->workFromHomeSleepLater();
      response = "When working from home:\n";
      response+= "Wake up: "+String(databus->workFromHomeWakeUp.str().c_str())+" sleep: "+String(databus->workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeWakeUpEarlier:
      controller->workFromOfficeWakeUpEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeWakeUpLater:
      controller->workFromOfficeWakeUpLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeGoOutEarlier:
      controller->workFromOfficeGoOutEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeGoOutLater:
      controller->workFromOfficeGoOutLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeComeInEarlier:
      controller->workFromOfficeComeInEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeComeInLater:
      controller->workFromOfficeComeInLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeSleepEarlier:
      controller->workFromOfficeSleepEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeSleepLater:
      controller->workFromOfficeSleepLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(databus->workAtOfficeWakeUp.str().c_str())+" go out: "+String(databus->workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(databus->workAtOfficeComeIn.str().c_str())+" sleep: "+String(databus->workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWeekendWakeUpEarlier:
      controller->weekendWakeUpEarlier();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(databus->weekendWakeUp.str().c_str())+" sleep: "+String(databus->weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendWakeUpLater:
      controller->weekendWakeUpLater();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(databus->weekendWakeUp.str().c_str())+" sleep: "+String(databus->weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendSleepEarlier:
      controller->weekendSleepEarlier();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(databus->weekendWakeUp.str().c_str())+" sleep: "+String(databus->weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendSleepLater:
      controller->weekendSleepLater();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(databus->weekendWakeUp.str().c_str())+" sleep: "+String(databus->weekendSleep.str().c_str())+"\n";
      break;

    case tcHighTempLower:
      controller->highTempLower();
      response = "High temp set to " + temperature(databus->highTemp) + "\n";
      response+= "Low temp set to " + temperature(databus->lowTemp) + "\n";
      break;

    case tcHighTempHigher:
      controller->highTempHigher();
      response = "High temp set to " + temperature(databus->highTemp) + "\n";
      response+= "Low temp set to " + temperature(databus->lowTemp) + "\n";
      break;

    case tcLowTempLower:
      controller->lowTempLower();
      response = "High temp set to " + temperature(databus->highTemp) + "\n";
      response+= "Low temp set to " + temperature(databus->lowTemp) + "\n";
      break;

    case tcLowTempHigher:
      controller->lowTempHigher();
      response = "High temp set to " + temperature(databus->highTemp) + "\n";
      response+= "Low temp set to " + temperature(databus->lowTemp) + "\n";
      break;

    default:
      response="Hmmm, looks like this callback function is not implemented yet";
      break;
  }

  response+= "__" + currentTime() + " room temperature " + temperature(databus->measuredTemperature) + 
             " setpoint " + databus->dpTemperatureSetpoint.getValue() + "°C__";

  return response;
};

/**********************************************************
 *  THE TELEGRAM HANDLER MANAGES ALL CHATS                *
 **********************************************************/
class TelegramHandler {
  public:
    TelegramHandler(DataBus & databus, WiFiClientSecure &securedClient, UniversalTelegramBot & bot, Controller & controller);
    void begin();
    void loop();
    void handleNewMessages(int numNewMessages);
  
    void updateChatID(String & chatID)                        { listOfChats[chatID].chatID=chatID;                   };
    void updateUserName(String & chatID, String & userName)   { listOfChats[chatID].userName=userName;               };
    void updateLabels(String & chatID)                        { listOfChats[chatID].updateLabels();                  };
    String keyboard(String & chatID)                          { return listOfChats[chatID].keyboard();               };
    String handleMessage (String & chatID, String & message)  { return listOfChats[chatID].handleMessage (message);  };
    String handleCallback(String & chatID, String & callback) { return listOfChats[chatID].handleCallback(callback); };

  protected:  
    DataBus *databus;
    UniversalTelegramBot *bot;
    WiFiClientSecure *securedClient;
    Controller *controller;
    
    std::map<String, TelegramChat> listOfChats;
};

TelegramHandler::TelegramHandler(DataBus & databus, WiFiClientSecure &securedClient, UniversalTelegramBot & bot, Controller & controller) { 
  this->databus = & databus; 
  this->securedClient = & securedClient; 
  this->bot = & bot; 
  this->controller = & controller; 
};

void TelegramHandler::begin() {
  // Setup secure connection for Telegram
  securedClient->setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  bot->updateToken(databus->botToken);

  // Add first chatID to list
  TelegramChat newTelegramChat=TelegramChat(*databus, *controller);
  listOfChats[databus->botChatID]=newTelegramChat;

  const String commands = F("["
                            "{\"command\":\"start\",   \"description\":\"Welcome message\"},"
                            "{\"command\":\"log\",     \"description\":\"Status of controller\"},"
                            "{\"command\":\"clearlog\",\"description\":\"Clear logdata\"}" // no comma on last command
                            "]");
  bot->setMyCommands(commands);

  updateChatID(databus->botChatID);
  updateLabels(databus->botChatID);
  String message="/start";
  handleMessage(databus->botChatID, message);

  bot->sendMessageWithInlineKeyboard(databus->botChatID, "Thermostat just started", "Markdown", keyboard(databus->botChatID));
};

void TelegramHandler::loop() {
  static unsigned long previousAttempt = 0;
  static unsigned long telegramIntervalMillis = 500;
  //const char* remote_host = "www.google.com";

  // Check Telegram twice a second, but much less frequently if backlight is on
  telegramIntervalMillis = databus->delayTelegram ? 5000 : 2000;

  if( millis() - previousAttempt > telegramIntervalMillis ){
    Serial.println("TelegramHandler::loop()");
    databus->logBusyTime.start(btTelegram);  
    
    //Serial.println(telegramIntervalMillis);
    //Serial.println(Ping.ping(remote_host) ? "Ping success" : "Ping no response");
    
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
  
    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    }

    databus->logBusyTime.finish(btTelegram);  

    previousAttempt=millis();
  }
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
      TelegramChat newTelegramChat=TelegramChat(*databus, *controller);
      listOfChats[chatID]=newTelegramChat;
      Serial.printf("Chat added to list of chats\n");
    }
    
    updateUserName(chatID, fromName);
    updateChatID(chatID);

    if(bot->messages[i].type=="message") {
      Serial.printf("Handle message: %s\n", bot->messages[i].text.c_str());
      message = handleMessage(chatID, bot->messages[i].text);
      updateLabels(chatID);
      bot->sendMessageWithInlineKeyboard(chatID, message, "Markdown", keyboard(chatID));
    }
    
    else if (bot->messages[i].type=="callback_query") {
      Serial.printf("Handle callback: %s\n", bot->messages[i].text.c_str());
      message = handleCallback(chatID, bot->messages[i].text);
      updateLabels(chatID);
      bot->sendMessageWithInlineKeyboard(chatID, message, "Markdown", keyboard(chatID), messageID);
    }
  } // for i
};
