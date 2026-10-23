#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

#include "a_Constants.h"
#include "d_Heaterprogram.h"

using namespace std;

const unsigned long TELEGRAM_INTERVAL_MILLIS = 1000;

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

enum tgMenu { tmMain, tmOverruleToday, tmOverruleTomorrow, tmOverruleMultiple, tmSettingsMain, tmSettingsWeekSchedule, tmDaySchedule, tmWorkFromHomeTimes, 
  tmWorkFromOfficeTimes, tmWeekendTimes, tmTemperatureSettings }; 

enum tgCommand { tcCommandNotRecognized, tcSetpointLower, tcSetpointAuto, tcSetpointHigher, tcGoAway, tcOverruleTodayMenu, tcOverruleTomorrowMenu, 
    tcOverruleMultipleMenu, tcSettingsMenu, tcSendUpdate, tcOverruleTodayWorkFromHome, tcOverruleTodayWorkFromOffice, tcOverruleTodayWeekend, 
    tcOverruleTodayAway, tcOverruleTodayAutomatic, tvMainMenu, tcOverruleTomorrowWorkFromHome, tcOverruleTomorrowWorkFromOffice, tcOverruleTomorrowWeekend, 
    tcOverruleTomorrowAway, tcOverruleTomorrowAutomatic, tcOverruleMultipleWorkFromHome, tcOverruleMultipleWorkFromOffice, tcOverruleMultipleWeekend, 
    tcOverruleMultipleAway, tcOverruleMultipleAutomatic, tcOverruleMultipleLessDays, tcOverruleMultipleMoreDays, tcWeeklyScheduleMenu, tcWorkFromHomeTimesMenu, 
    tcWorkFromOfficeTimesMenu, tcWeekendTimesMenu, tcTemperatureMenu, tcMondayScheduleMenu, tcTuesdayScheduleMenu, tcWednesdayScheduleMenu, tcThursdayScheduleMenu, 
    tcFridayScheduleMenu, tcSaturdayScheduleMenu, tcSundayScheduleMenu, tcWorkFromHome, tcWorkFromOffice, tcWeekend, tcAway, tcWorkFromHomeWakeUpEarlier, 
    tcWorkFromHomeWakeUpLater, tcWorkFromHomeSleepEarlier, tcWorkFromHomeSleepLater, tcWorkFromOfficeWakeUpEarlier, tcWorkFromOfficeWakeUpLater, 
    tcWorkFromOfficeGoOutEarlier, tcWorkFromOfficeGoOutLater, tcWorkFromOfficeComeInEarlier, tcWorkFromOfficeComeInLater, tcWorkFromOfficeSleepEarlier, 
    tcWorkFromOfficeSleepLater, tcWeekendWakeUpEarlier, tcWeekendWakeUpLater, tcWeekendSleepEarlier, tcWeekendSleepLater, tcHighTempLower, tcHighTempHigher, 
    tcLowTempLower, tcLowTempHigher
};

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
  { tvMainMenu, { "Back to main menu", "/tvMainMenu" } },

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

  { tcWeeklyScheduleMenu, { "Set weekly schedule", "/tcWeeklyScheduleMenu" } },
  { tcWorkFromHomeTimesMenu, { "Set work from home times", "/tcWorkFromHomeTimesMenu" } },
  { tcWorkFromOfficeTimesMenu, { "Set work from office times", "/tcWorkFromOfficeTimesMenu" } },
  { tcWeekendTimesMenu, { "Set weekend times", "/tcWeekendTimesMenu" } },
  { tcTemperatureMenu, { "Set temperatures", "/tcTemperatureMenu" } },

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
  { tcLowTempHigher, { "Low temp ..°C", "/tcLowTempHigher" } }
};  


struct TelegramChat {
  public:
    String chatID = "";
    String userName = "";
    bool   userWantsUpdates = true;
    
    tgMenu currentMenu = tmMain;
    tgCommandList commands = TELEGRAM_COMMANDS;
  
    void updateLabels(Data & data);
    String keyboard();
    String handleMessage (Data & data, String & message);
    String handleCallback(Data & data, String & callback);
    
  private:
    String btnInline(tgCommand command);
    String temperature(float temp);
    String number(float f);
    String currentTime();
};

void TelegramChat::updateLabels(Data & data) {
  commands[tcSetpointLower ].label = "Down (" + temperature(data.overrideSetpoint-0.5) + ")";
  commands[tcSetpointHigher].label = "Up ("   + temperature(data.overrideSetpoint+0.5) + ")";
  commands[tcOverruleMultipleLessDays].label = String(data.overrideMultipleCount-1) + " days";
  commands[tcOverruleMultipleMoreDays].label = String(data.overrideMultipleCount+1) + " days";
  commands[tcHighTempLower ].label = temperature(data.highTemp-0.5);
  commands[tcHighTempHigher].label = temperature(data.highTemp+0.5);
  commands[tcLowTempLower  ].label = temperature(data.lowTemp -0.5);
  commands[tcLowTempHigher ].label = temperature(data.lowTemp +0.5);

  commands[tcWorkFromHomeWakeUpEarlier  ].label = "wake " +String( (data.workFromHomeWakeUp-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeWakeUpLater    ].label = "wake " +String( (data.workFromHomeWakeUp+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeSleepEarlier   ].label = "sleep "+String( (data.workFromHomeSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromHomeSleepLater     ].label = "sleep "+String( (data.workFromHomeSleep +timeValue_t(0,15)).str().c_str());

  commands[tcWorkFromOfficeWakeUpEarlier].label = "wake "    +String( (data.workAtOfficeWakeUp-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeWakeUpLater  ].label = "wake "    +String( (data.workAtOfficeWakeUp+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeGoOutEarlier ].label = "go out "  +String( (data.workAtOfficeGoOut-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeGoOutLater   ].label = "go out "  +String( (data.workAtOfficeGoOut+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeComeInEarlier].label = "come in " +String( (data.workAtOfficeComeIn-timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeComeInLater  ].label = "come in " +String( (data.workAtOfficeComeIn+timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeSleepEarlier ].label = "sleep "   +String( (data.workAtOfficeSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWorkFromOfficeSleepLater   ].label = "sleep "   +String( (data.workAtOfficeSleep +timeValue_t(0,15)).str().c_str());

  commands[tcWeekendWakeUpEarlier       ].label = "wake "   +String( (data.weekendWakeUp -timeValue_t(0,15)).str().c_str());
  commands[tcWeekendWakeUpLater         ].label = "wake "   +String( (data.weekendWakeUp +timeValue_t(0,15)).str().c_str());
  commands[tcWeekendSleepEarlier        ].label = "sleep "  +String( (data.weekendSleep -timeValue_t(0,15)).str().c_str());
  commands[tcWeekendSleepLater          ].label = "sleep "  +String( (data.weekendSleep +timeValue_t(0,15)).str().c_str());
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
  char buffer1[80];
  char buffer2[80];
  
  switch(currentMenu) {

    // Main menu is chosen as default option and therefore missing here

    case tmOverruleToday:
      result = "[["+ btnInline(tcOverruleTodayWorkFromHome) + "],";
      result+=  "["+ btnInline(tcOverruleTodayWorkFromOffice) + "],";
      result+=  "["+ btnInline(tcOverruleTodayWeekend) + "],";
      result+=  "["+ btnInline(tcOverruleTodayAway) + "],";
      result+=  "["+ btnInline(tcOverruleTodayAutomatic) + "],";
      result+=  "["+ btnInline(tvMainMenu) + "]]";
    break;
    
    case tmOverruleTomorrow:
      result = "[["+ btnInline(tcOverruleTomorrowWorkFromHome) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowWorkFromOffice) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowWeekend) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowAway) + "],";
      result+=  "["+ btnInline(tcOverruleTomorrowAutomatic) + "],";
      result+=  "["+ btnInline(tvMainMenu) + "]]";
    break;
    
    case tmOverruleMultiple:
      result= "[["+ btnInline(tcOverruleMultipleWorkFromHome) + "],";
      result+= "["+ btnInline(tcOverruleMultipleWorkFromOffice) + "],";
      result+= "["+ btnInline(tcOverruleMultipleWeekend) + "],";
      result+= "["+ btnInline(tcOverruleMultipleAway) + "],";
      result+= "["+ btnInline(tcOverruleMultipleLessDays) + ", " + btnInline(tcOverruleMultipleAutomatic) + ", " + 
                   btnInline(tcOverruleMultipleMoreDays) + "],";
      result+= "["+ btnInline(tvMainMenu) + "]]";
    break;
    
    case tmSettingsMain:
      result= "[["+ btnInline(tcWeeklyScheduleMenu) + "],";
      result+= "["+ btnInline(tcWorkFromHomeTimesMenu) + "],";
      result+= "["+ btnInline(tcWorkFromOfficeTimesMenu) + "],";
      result+= "["+ btnInline(tcWeekendTimesMenu) + "],";
      result+= "["+ btnInline(tcTemperatureMenu) + "],";
      result+= "["+ btnInline(tvMainMenu) + "]]";
    break;
    
    case tmSettingsWeekSchedule:
      result= "[["+ btnInline(tcMondayScheduleMenu)    + "," + btnInline(tcTuesdayScheduleMenu) + "],";
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
      result= "[["+ btnInline(tcHighTempLower) + "," + btnInline(tcHighTempHigher) + "],";
      result+= "["+ btnInline(tcLowTempLower)  + "," + btnInline(tcLowTempHigher) + "],";
      result+= "["+ btnInline(tcSettingsMenu) + "]]";
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

String TelegramChat::handleMessage(Data & data, String & message) {
  String response;

  if(message=="/start") {
    currentMenu=tmMain;
    response = "Welcome " + userName + ".\n";
  }
  else if(message=="/log") {
    response = "Current time " + currentTime() + "\n";
    response += "Temperature setpoint: "+number(data.temperatureSetpoint) + "\n";
    response += "Measured temperature: "+number(data.measuredTemperature) + "\n";
    response += "P: "+number(data.P) + "\n";
    response += "I: "+number(data.I) + "\n";
    response += "D: "+number(data.D) + "\n";
    response += "Boiler percentage: "+number(data.boilerPercentage) + "\n";
    response += "Requested boiler temperature: "+number(data.requestedBoilerTemperature) + "\n";
    response += "Actual boiler temperature: "+number(data.actualBoilerTemperature) + "\n";
    response += "Actual hot water temperature: "+number(data.actualDomesticHotWaterTemperature) + "\n";
    response += "Central heating active: "+ String(data.isCentralHeatingActive ? "Y" : "N") + "\n";
    response += "Hot water active: "+ String(data.isHotWaterActive ? "Y" : "N") + "\n";
    response += "Flame on: "+ String(data.isFlameOn ? "Y" : "N") + "\n";
    response += "Response : "+ String(data.responseFromController) + "\n";
  }
  else if(message=="/clearlog") {
    deleteLogFile();
    response = "Log file deleted";
  }
  else {
    response = "Command " + message + " is not recognized";
  }

  return response;
};

String TelegramChat::handleCallback(Data & data, String & callback) {
  time_t now;
  struct tm * lctm;
  uint8_t dayOfWeek;
  static uint8_t dayForSubmenu;
  char buffer[80];

  // Recognize the response from the inline keyboard
  tgCommand command = tcCommandNotRecognized;
  for (auto const& kv : commands) {
    if(callback==kv.second.callback) {
      command=kv.first;
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
      data.setpointLower();
      response= String("The setpoint temperature is lowered to ") +temperature(data.temperatureSetpoint)+"\n";
      break;
      
    case tcSetpointHigher:
      data.setpointHigher();
      response= String("The setpoint temperature is increased to ")+temperature(data.temperatureSetpoint)+"\n";
      break;
      
    case tcSetpointAuto:
      data.setpointAuto();
      response = "The current temperature setpoint is set to 'Auto'.\n";
      break;
      
    case tcGoAway:
      data.overruleTodayAway();
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
      if(data.overrideMultiple==dtAuto) {
        response = String("Multiple days set to ") + String(DAY_TYPES[data.overrideMultiple].c_str()) + "\n";
      }
      else {
        time(&now); // Get current time
        lctm = localtime(&now);
        lctm->tm_mday += data.overrideMultipleCount;
        mktime(lctm);
        strftime(buffer, 80, "%A %e %B", lctm);
        response = String("Multiple days set to ") + String(DAY_TYPES[data.overrideMultiple].c_str()) + " for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      };
      break;
      
    case tcSettingsMenu:
      currentMenu=tmSettingsMain;
      response= "Change settings\n";
      break;

    case tcSendUpdate:
      time(&now); // Get current time
      lctm = localtime(&now); // Convert time to local time
      dayOfWeek = lctm->tm_wday;

      switch(data.reasonForSetpoint) {

        case spWeekSchedule:
          response = String("The setpoint is determined by the normal week schedule.\n");
          response+= "Today is set to be '" + String(DAY_TYPES[data.regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;

        case spMultipleDays:
          response= String("Multiple days are set to '") + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days.\n";
        break;

        case spOverrideToday:
          response= String("Today is overruled to be '") + String(DAY_TYPES[data.overrideToday].c_str()) + "'.\n";
        break;

        case spOverrideNow:
          snprintf(buffer, 80, "%.1f", data.overrideSetpoint);
          response= String("The setpoint is temporarily overruled by the +/- buttons at ")+ buffer + "°C\n";
        break;
      }
      break;

    case tcOverruleTodayWorkFromHome:
      data.overruleTodayWorkFromHome();
      response = "Today is set to 'work from Home'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayWorkFromOffice:
      data.overruleTodayWorkFromOffice();
      response = "Today is set to 'work at the office'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayWeekend:
      data.overruleTodayWeekend();
      response = "Today is set as weekend day\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayAway:
      data.overruleTodayAway();
      response = "Today is set as a day away\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTodayAutomatic:
      data.overruleTodayAutomatic();
      response = String("Today is set to automatic\n");

      switch(data.reasonForSetpoint) {

        case spWeekSchedule:
          response+= "The setpoint is determined by the normal week schedule.\n";
          response+= "Today is set to be '" + String(DAY_TYPES[data.regularWeek[dayOfWeek]].c_str()) + "'.\n";
        break;

        case spMultipleDays:
          response+= "Multiple days are set to '" + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days.\n";
        break;

        case spOverrideToday:
          response+= String("Today is overruled to be '") + String(DAY_TYPES[data.overrideToday].c_str()) + "'.\n";
        break;

        case spOverrideNow:
          snprintf(buffer, 80, "%.1f", data.overrideSetpoint);
          response+= String("The setpoint is temporarily overruled by the +/- buttons at ") + buffer + "°C\n";
        break;
      }

      currentMenu=tmMain;
      break;

    case tvMainMenu:
      currentMenu=tmMain;
      response= "Main menu\n";
      break;

    case tcOverruleTomorrowWorkFromHome:
      data.overruleTomorrowWorkFromHome();
      response = "Tomorrow is set to 'work from home'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowWorkFromOffice:
      data.overruleTomorrowWorkFromOffice();
      response = "Tomorrow is set to 'work at the office'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowWeekend:
      data.overruleTomorrowWeekend();
      response = "Tomorrow is set to 'weekend day'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowAway:
      data.overruleTomorrowAway();
      response = "Tomorrow is set as 'day away'\n";
      currentMenu=tmMain;
      break;

    case tcOverruleTomorrowAutomatic:
      data.overruleTomorrowAutomatic();
      response = "Tomorrow is set to automatic\n";
      currentMenu=tmMain;
      break;

    case tcOverruleMultipleWorkFromHome:
      data.overruleMultipleWorkFromHome();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleWorkFromOffice:
      data.overruleMultipleWorkFromOffice();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleWeekend:
      data.overruleMultipleWeekend();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = String("Multiple days set to '") + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleAway:
      data.overruleMultipleAway();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleAutomatic:
      data.overruleMultipleAutomatic();
      response = "Multiple days set to '" + String(DAY_TYPES[data.overrideMultiple].c_str()) + "\n";
      break;

    case tcOverruleMultipleLessDays:
      data.overruleMultipleLessDays();
      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";
      break;

    case tcOverruleMultipleMoreDays:
      data.overruleMultipleMoreDays();

      time(&now); // Get current time
      lctm = localtime(&now);
      lctm->tm_mday += data.overrideMultipleCount;
      mktime(lctm);
      strftime(buffer, 80, "%A %e %B", lctm);
      response = "Multiple days set to '" + String(DAY_TYPES[data.overrideMultiple].c_str()) + "' for " + String(data.overrideMultipleCount) + " days until " + buffer + "\n";

      break;

    case tcWeeklyScheduleMenu:
      currentMenu=tmSettingsWeekSchedule;
      response= "Choose a day for the weekly schedule\n";
      break;

    case tcWorkFromHomeTimesMenu:
      currentMenu=tmWorkFromHomeTimes;
      response = "When working from home:\n";
      response+= "Wake up: "+String(data.workFromHomeWakeUp.str().c_str())+" sleep: "+String(data.workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeTimesMenu:
      currentMenu=tmWorkFromOfficeTimes;
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWeekendTimesMenu:
      currentMenu=tmWeekendTimes;
      response = "On weekend days:\n";
      response+= "Wake up: "+String(data.weekendWakeUp.str().c_str())+" sleep: "+String(data.weekendSleep.str().c_str())+"\n";
      break;

    case tcTemperatureMenu:
      currentMenu=tmTemperatureSettings;
      response = String("High temp set to ") + temperature(data.highTemp) + "\n";
      response+= String("Low temp set to ")  + temperature(data.lowTemp)  + "\n";
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
      data.setWeekSchedule(dayForSubmenu, dtWorkFromHome);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[data.regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWorkFromOffice:
      data.setWeekSchedule(dayForSubmenu, dtWorkAtOffice);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[data.regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWeekend:
      data.setWeekSchedule(dayForSubmenu, dtWeekend);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[data.regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcAway:
      data.setWeekSchedule(dayForSubmenu, dtAway);
      currentMenu=tmSettingsWeekSchedule;
      response = "Schedule for " + String(DAY_NAMES[dayForSubmenu].c_str()) + "changed to "+ String(DAY_TYPES[data.regularWeek[dayForSubmenu]].c_str()) + "\n";      
      break;

    case tcWorkFromHomeWakeUpEarlier:
      data.workFromHomeWakeUpEarlier();
      response = "When working from home:\n";
      response+= "Wake up: "+String(data.workFromHomeWakeUp.str().c_str())+" sleep: "+String(data.workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeWakeUpLater:
      data.workFromHomeWakeUpLater();
      response = "When working from home:\n";
      response+= "Wake up: "+String(data.workFromHomeWakeUp.str().c_str())+" sleep: "+String(data.workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeSleepEarlier:
      data.workFromHomeSleepEarlier();
      response = "When working from home:\n";
      response+= "Wake up: "+String(data.workFromHomeWakeUp.str().c_str())+" sleep: "+String(data.workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromHomeSleepLater:
      data.workFromHomeSleepLater();
      response = "When working from home:\n";
      response+= "Wake up: "+String(data.workFromHomeWakeUp.str().c_str())+" sleep: "+String(data.workFromHomeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeWakeUpEarlier:
      data.workFromOfficeWakeUpEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeWakeUpLater:
      data.workFromOfficeWakeUpLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeGoOutEarlier:
      data.workFromOfficeGoOutEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeGoOutLater:
      data.workFromOfficeGoOutLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeComeInEarlier:
      data.workFromOfficeComeInEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeComeInLater:
      data.workFromOfficeComeInLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeSleepEarlier:
      data.workFromOfficeSleepEarlier();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWorkFromOfficeSleepLater:
      data.workFromOfficeSleepLater();
      response= "When working at the office:\n";
      response+= "Wake up: "+String(data.workAtOfficeWakeUp.str().c_str())+" go out: "+String(data.workAtOfficeGoOut.str().c_str())+"\n";
      response+= "Come in: "+String(data.workAtOfficeComeIn.str().c_str())+" sleep: "+String(data.workAtOfficeSleep.str().c_str())+"\n";
      break;

    case tcWeekendWakeUpEarlier:
      data.weekendWakeUpEarlier();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(data.weekendWakeUp.str().c_str())+" sleep: "+String(data.weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendWakeUpLater:
      data.weekendWakeUpLater();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(data.weekendWakeUp.str().c_str())+" sleep: "+String(data.weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendSleepEarlier:
      data.weekendSleepEarlier();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(data.weekendWakeUp.str().c_str())+" sleep: "+String(data.weekendSleep.str().c_str())+"\n";
      break;

    case tcWeekendSleepLater:
      data.weekendSleepLater();
      response = "On weekend days:\n";
      response+= "Wake up: "+String(data.weekendWakeUp.str().c_str())+" sleep: "+String(data.weekendSleep.str().c_str())+"\n";
      break;

    case tcHighTempLower:
      data.highTempLower();
      response = "High temp set to " + temperature(data.highTemp) + "\n";
      response+= "Low temp set to " + temperature(data.lowTemp) + "\n";
      break;

    case tcHighTempHigher:
      data.highTempHigher();
      response = "High temp set to " + temperature(data.highTemp) + "\n";
      response+= "Low temp set to " + temperature(data.lowTemp) + "\n";
      break;

    case tcLowTempLower:
      data.lowTempLower();
      response = "High temp set to " + temperature(data.highTemp) + "\n";
      response+= "Low temp set to " + temperature(data.lowTemp) + "\n";
      break;

    case tcLowTempHigher:
      data.lowTempHigher();
      response = "High temp set to " + temperature(data.highTemp) + "\n";
      response+= "Low temp set to " + temperature(data.lowTemp) + "\n";
      break;

    default:
      response="Hmmm, looks like this callback function is not implemented yet";
      break;
  }

  response+= "__" + currentTime() + " room temperature " + temperature(data.measuredTemperature) + 
             " setpoint " + temperature(data.temperatureSetpoint) + "__";

  return response;
};

/* ====================================================== *
 *  THE TELEGRAM HANDLER MANAGES ALL CHATS                *
 * ====================================================== */
class TelegramHandler {
  public:
    std::map<String, TelegramChat> listOfChats;
  
    void updateChatID(String & chatID)                                     { listOfChats[chatID].chatID=chatID;                         };
    void updateUserName(String & chatID, String & userName)                { listOfChats[chatID].userName=userName;                     };
    void updateLabels(Data & data, String & chatID)                        { listOfChats[chatID].updateLabels(data);                    };
    String keyboard(String & chatID)                                       { return listOfChats[chatID].keyboard();                     };
    String handleMessage (Data & data, String & chatID, String & message)  { return listOfChats[chatID].handleMessage (data, message);  };
    String handleCallback(Data & data, String & chatID, String & callback) { return listOfChats[chatID].handleCallback(data, callback); };
};

/* =================================================================== *
 *  GLOBAL VARIABLES                                                   *
 * =================================================================== */

WiFiClientSecure secured_client;
UniversalTelegramBot bot("", secured_client);
TelegramHandler telegramHandler;


void handleNewMessages(Data & data, int numNewMessages)
{
  String message;
  
  for (int i = 0; i < numNewMessages; i++)
  {
    int messageID = bot.messages[i].message_id;
    String chatID = bot.messages[i].chat_id;
    Serial.printf("Message from chatID: %s\n", chatID);
    
    String fromName = bot.messages[i].from_name;
    if (fromName == "") fromName = "Guest";
    telegramHandler.updateUserName(chatID, fromName);
    telegramHandler.updateChatID(chatID);

    if(bot.messages[i].type=="message") {
      message = telegramHandler.handleMessage(data, chatID, bot.messages[i].text);
      telegramHandler.updateLabels(data, chatID);
      bot.sendMessageWithInlineKeyboard(chatID, message, "Markdown", telegramHandler.keyboard(chatID));
    }
    
    else if (bot.messages[i].type=="callback_query") {
      message = telegramHandler.handleCallback(data, chatID, bot.messages[i].text);
      telegramHandler.updateLabels(data, chatID);
      bot.sendMessageWithInlineKeyboard(chatID, message, "Markdown", telegramHandler.keyboard(chatID), messageID);
    }
  } // for i
}

/* =================================================================== *
 *  INITIALIZE TELEGRAM                                                *
 * =================================================================== */

void setupTelegram(Data & data) {
  // Setup secure connection for Telegram
  bot.updateToken(data.botToken);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  /*
  String message = telegramHandler.handleMessage(data, data.botChatID, "/start");
  telegramHandler.updateLabels(data, data.botChatID);
  bot.sendMessageWithInlineKeyboard(data.botChatID, "Thermostat just started", "Markdown", telegramHandler.keyboard(data.botChatID));
  */
};

/* =================================================================== *
 *  PeRIODICALLY HANDLE TELEGRAM                                       *
 * =================================================================== */

void handleTelegramIfNeeded(Data & data) {
  static unsigned long previousAttempt = 0;

  if(millis() - previousAttempt > TELEGRAM_INTERVAL_MILLIS){
    previousAttempt=millis();
    
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(data, numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }
};
