#pragma once

#include <map>
#include <list>

#define BACKLIGHT_TIME_OUT 60*1000

class Screen {
  public:
    Screen() { }; // Never used, but required when class is used in a map
    Screen(std::list<command_t> menu);

    void firstItem() {
      selection = std::begin(menu);
    };
    
    void prevItem()  {
      if ( selection != std::begin(menu) ) --selection;
    };
    
    void nextItem()  {
      if ( selection != --std::end(menu) ) ++selection;
    };

    void selectItem(command_t menuItem);
    command_t selectedItem() {
      return *selection;
    };

    std::list<command_t> menu;
    std::list<command_t>::iterator selection;
};

Screen::Screen(std::list<command_t> menu) {
  this->menu = menu;
  firstItem();
}

void Screen::selectItem(command_t menuItem) {
  bool itemfound = false;
  for (std::list<command_t>::iterator item = std::begin(menu); item != std::end(menu); item++) {
    if (*item == menuItem) {
      selection = item;
      itemfound = true;
      break;
    }
  }

  // Always produce a valid result, even when item is not found
  if ( !itemfound ) std::begin(menu);
};


typedef std::map<screen_t, Screen > umScreen_t;

class MenuStructure {
  public:
    MenuStructure(screenInitializer_t structure);
    void selectScreen(screen_t screen) { currentScreen = screen; };
    void selectScreen(screen_t screen, command_t menuItem) { currentScreen = screen; screens[screen].selectItem(menuItem); };
    void selectMenuItem(screen_t screen, command_t menuItem) { screens[screen].selectItem(menuItem); };
    void prevMenuItem()  { screens[currentScreen].prevItem();  };
    void nextMenuItem()  { screens[currentScreen].nextItem();  };
    void firstMenuItem() { screens[currentScreen].firstItem(); };

    screen_t selectedScreen() { return currentScreen; };
    command_t selectedMenuItem() { return screens[currentScreen].selectedItem(); };
    std::list<command_t> menuItems() { return screens[currentScreen].menu; };
    void dump();

    screen_t  previousScreen;
    command_t dayToBeOverruled = cmdMenuOverruleToday;
    command_t daySettingComingFrom;
    dayType_t  daySettingCurrent;

  protected:
    screen_t currentScreen;
    umScreen_t screens;
};

MenuStructure::MenuStructure(screenInitializer_t structure) {
  Screen newScreen;
  for (auto & screen : structure) {
    newScreen = Screen(screen.second);
    //newScreen.firstItem();
    screens[screen.first] = newScreen;
    screens[screen.first].firstItem();
  }

  //for(auto & screen : screens) screen.second.firstItem();

  currentScreen = (*std::begin(screens)).first; // By default, select the first screen
};

void MenuStructure::dump() {
  Serial.printf("Selected screen %s\n", screenTitle[ selectedScreen() ].c_str() );
  for (auto & screen : screens) {
    Serial.printf("Screen title %s selected item: %s\n", screenTitle[screen.first].c_str(), commandLabels[screen.second.selectedItem()].c_str() );
    for ( auto & command : screen.second.menu ) {
      Serial.print( screen.second.selectedItem() == command ? ">" : " ");
      Serial.println(String(" - ") + commandLabels[command].c_str() );
    }
  }

};

/*******************************************************
     GLOBAL VARIABLES
 *******************************************************/

MenuStructure menu = MenuStructure(SCREENS); // Screens and menus of hardware unit

// Functions that display can point to to learn what the current screen and selected menu are
screen_t selectedScreen() {
  return menu.selectedScreen();
};
command_t selectedMenuItem() {
  return menu.selectedMenuItem();
};

// Forward declarations
void HandleSelectButton();

// Setup the keyboard handler
void startMenu() {
  menu.selectMenuItem(scnHome, cmdMenuMain);
}

void sendMessageToController(command_t command) {
  userEventMessage_t message(menu.selectedScreen(), menu.selectedMenuItem(), command);
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void sendTemperatureOffsetToController(float temperatureOffset) {
  userEventMessage_t message(menu.selectedScreen(), menu.selectedMenuItem(), cmdSetTemperatureOffset);
  message.temperatureOffset = temperatureOffset;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void sendWeekScheduleToController(uint8_t dayOfWeek, dayType_t typeOfDay) {
  userEventMessage_t message(menu.selectedScreen(), menu.selectedMenuItem(), cmdSetWeekSchedule);
  message.dayOfWeek = dayOfWeek;
  message.typeOfDay = typeOfDay;
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void selectScreen(screen_t screen, command_t menuItem) {
  menu.selectScreen(screen, menuItem);
  userEventMessage_t message(sndMenu, menuItem);
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void selectScreen(screen_t screen) {
  menu.selectScreen(screen);
  userEventMessage_t message = userEventMessage_t(sndMenu, cmdUpdateScreen);
  xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );
};

void HandleSelectButton() {
  userEventMessage_t message;

  switch( menu.selectedScreen() ) {
     
    // SCREEN scnHome
    case scnHome:
      selectScreen(scnMain);
      break;
  
    // SCREEN scnMain
    case scnMain :
      if        ( menu.selectedMenuItem() == cmdMenuOverruleToday ) {
        if      ( controllerData.overrideToday == dtWorkFromHome) selectScreen(scnOverruleToday, cmdWorkFromHome);
        else if ( controllerData.overrideToday == dtWorkAtOffice) selectScreen(scnOverruleToday, cmdWorkAtOffice);
        else if ( controllerData.overrideToday == dtAway        ) selectScreen(scnOverruleToday, cmdOverruleTodayAway);
        else if ( controllerData.overrideToday == dtWeekend     ) selectScreen(scnOverruleToday, cmdWeekend);
        else if ( controllerData.overrideToday == dtAuto        ) selectScreen(scnOverruleToday, cmdOverruleTodayAutomatic);
        else                                                      selectScreen(scnOverruleToday);
      }
      else if   ( menu.selectedMenuItem() == cmdMenuOverruleTomorrow ) {
        if      ( controllerData.overrideTomorrow == dtWorkFromHome) selectScreen(scnOverruleTomorrow, cmdWorkFromHome);
        else if ( controllerData.overrideTomorrow == dtWorkAtOffice) selectScreen(scnOverruleTomorrow, cmdWorkAtOffice);
        else if ( controllerData.overrideTomorrow == dtAway        ) selectScreen(scnOverruleTomorrow, cmdOverruleTodayAway);
        else if ( controllerData.overrideTomorrow == dtWeekend     ) selectScreen(scnOverruleTomorrow, cmdWeekend);
        else if ( controllerData.overrideTomorrow == dtAuto        ) selectScreen(scnOverruleTomorrow, cmdOverruleTodayAutomatic);
      }
      else if   ( menu.selectedMenuItem() == cmdMenuOverruleMultipleDays ) {
        if      ( controllerData.overrideMultiple == dtWorkFromHome ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);
        else if ( controllerData.overrideMultiple == dtWorkAtOffice ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);
        else if ( controllerData.overrideMultiple == dtAway         ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);
        else if ( controllerData.overrideMultiple == dtWeekend      ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);
        else if ( controllerData.overrideMultiple == dtAuto         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAutomatic);
      }
      else if   ( menu.selectedMenuItem() == cmdMenuSettings ) selectScreen(scnSettingsMain);
      else if   ( menu.selectedMenuItem() == cmdMenuHome )     selectScreen(scnHome);
    break;
  
    // SCREEN scnOverruleToday
    case scnOverruleToday :
  
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == cmdOverruleTodayWorkFromHome)  sendMessageToController(cmdOverruleTodayWorkFromHome);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayWorkAtOffice) sendMessageToController(cmdOverruleTodayWorkAtOffice);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayWeekend)      sendMessageToController(cmdOverruleTodayWeekend);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayAway)         sendMessageToController(cmdOverruleTodayAway);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayAutomatic)    sendMessageToController(cmdOverruleTodayAutomatic);
  
      selectScreen(scnMain, cmdMenuMain);
    break;
      
    // SCREEN scnOverruleTomorrow
    case scnOverruleTomorrow:
  
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == cmdOverruleTomorrowWorkFromHome)  sendMessageToController(cmdOverruleTomorrowWorkFromHome);
      else if ( menu.selectedMenuItem() == cmdOverruleTomorrowWorkAtOffice) sendMessageToController(cmdOverruleTomorrowWorkAtOffice);
      else if ( menu.selectedMenuItem() == cmdOverruleTomorrowWeekend)      sendMessageToController(cmdOverruleTomorrowWeekend);
      else if ( menu.selectedMenuItem() == cmdOverruleTomorrowAway)         sendMessageToController(cmdOverruleTomorrowAway);
      else if ( menu.selectedMenuItem() == cmdOverruleTomorrowAutomatic)    sendMessageToController(cmdOverruleTomorrowAutomatic);
  
      selectScreen(scnMain, cmdMenuMain);
    break;
  
    // SCREEN scnOverruleMultiple
    case scnOverruleMultiple:
      if      ( menu.selectedMenuItem() == cmdOverruleMultipleMoreDays ) sendMessageToController(cmdOverruleMultipleMoreDays);
      else if ( menu.selectedMenuItem() == cmdOverruleMultipleFewerDays) sendMessageToController(cmdOverruleMultipleFewerDays);
      else if ( menu.selectedMenuItem() == cmdMultipleDaySchedule ) {
        if      (controllerData.overrideMultiple == dtWorkFromHome ) selectScreen(scnOverruleMultipleSchedule, cmdWorkFromHome);
        else if (controllerData.overrideMultiple == dtWorkAtOffice ) selectScreen(scnOverruleMultipleSchedule, cmdWorkAtOffice);
        else if (controllerData.overrideMultiple == dtAway         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAway);
        else if (controllerData.overrideMultiple == dtWeekend      ) selectScreen(scnOverruleMultipleSchedule, cmdWeekend);
        else if (controllerData.overrideMultiple == dtAuto         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAutomatic);
      }
      else if ( menu.selectedMenuItem() == cmdOverruleMultipleAutomatic) {
        sendMessageToController(cmdOverruleMultipleAutomatic);
        selectScreen(scnMain, cmdMenuMain);
      }
      else selectScreen(scnMain, cmdMenuMain);
    break;
  
    // SCREEN scnOverruleMultipleSchedule
    case scnOverruleMultipleSchedule:
      if      ( menu.selectedMenuItem() == cmdWorkFromHome           ) sendMessageToController(cmdOverruleMultipleWorkFromHome);
      else if ( menu.selectedMenuItem() == cmdWorkAtOffice           ) sendMessageToController(cmdOverruleMultipleWorkAtOffice);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayAway      ) sendMessageToController(cmdOverruleMultipleAway);
      else if ( menu.selectedMenuItem() == cmdWeekend                ) sendMessageToController(cmdOverruleMultipleWeekend);
      else if ( menu.selectedMenuItem() == cmdOverruleTodayAutomatic ) sendMessageToController(cmdOverruleMultipleAutomatic);
      // else if( menu.selectedMenuItem() == mnBackToPreviousMenu    ) {  }
      selectScreen(scnOverruleMultiple, cmdMenuMain);
    break;
  
    // SCREEN scnSettingsMain
    case scnSettingsMain:
      if      ( menu.selectedMenuItem() == cmdMenuTemperature  ) selectScreen(scnSettingsTemperature,  cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuSensorOffset ) selectScreen(scnSettingsSensorOffset, cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuWeekSchedule ) selectScreen(scnSettingsWeekSchedule, cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuHomeTimes    ) selectScreen(scnSettingsHomeTimes,    cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuOfficeTimes  ) selectScreen(scnSettingsOfficeTimes,  cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuWeekendTimes ) selectScreen(scnSettingsWeekendTimes, cmdMenuSettings);
      else if ( menu.selectedMenuItem() == cmdMenuMain         ) selectScreen(scnMain, cmdMenuMain);
    break;
  
    // SCREEN scnSettingsTemperature
    case scnSettingsTemperature:
      if      ( menu.selectedMenuItem() == cmdHighTemperatureUp   ) sendMessageToController(cmdHighTemperatureUp);
      else if ( menu.selectedMenuItem() == cmdHighTemperatureDown ) sendMessageToController(cmdHighTemperatureDown);
      else if ( menu.selectedMenuItem() == cmdLowTemperatureUp    ) sendMessageToController(cmdLowTemperatureUp);
      else if ( menu.selectedMenuItem() == cmdLowTemperatureDown  ) sendMessageToController(cmdLowTemperatureDown);
      else if ( menu.selectedMenuItem() == cmdMenuSettings        ) selectScreen(scnSettingsMain);
    break;
  
    // SCREEN scnSettingsSensorOffset
    case scnSettingsSensorOffset:
      if      ( menu.selectedMenuItem() == cmdSensorOffsetUp   ) sendMessageToController(cmdSensorOffsetUp);
      else if ( menu.selectedMenuItem() == cmdSensorOffsetDown ) sendMessageToController(cmdSensorOffsetDown);
      else if ( menu.selectedMenuItem() == cmdMenuSettings     ) selectScreen(scnMain);
    break;
  
    // SCREEN scnSettingsWeekSchedule
    case scnSettingsWeekSchedule:
      menu.daySettingComingFrom = menu.selectedMenuItem();
  
      if (     menu.daySettingComingFrom == cmdSunday    ) menu.daySettingCurrent = controllerData.regularWeek[0];
      else if (menu.daySettingComingFrom == cmdMonday    ) menu.daySettingCurrent = controllerData.regularWeek[1];
      else if (menu.daySettingComingFrom == cmdTuesday   ) menu.daySettingCurrent = controllerData.regularWeek[2];
      else if (menu.daySettingComingFrom == cmdWednesday ) menu.daySettingCurrent = controllerData.regularWeek[3];
      else if (menu.daySettingComingFrom == cmdThursday  ) menu.daySettingCurrent = controllerData.regularWeek[4];
      else if (menu.daySettingComingFrom == cmdFriday    ) menu.daySettingCurrent = controllerData.regularWeek[5];
      else if (menu.daySettingComingFrom == cmdSaturday  ) menu.daySettingCurrent = controllerData.regularWeek[6];
  
      if ( menu.selectedMenuItem() != cmdMenuSettings ) {
        if (     menu.daySettingCurrent == dtWorkFromHome ) selectScreen(scnSettingsDaySchedule, cmdWorkFromHome      );
        else if (menu.daySettingCurrent == dtWorkAtOffice ) selectScreen(scnSettingsDaySchedule, cmdWorkAtOffice      );
        else if (menu.daySettingCurrent == dtWeekend      ) selectScreen(scnSettingsDaySchedule, cmdWeekend           );
        else if (menu.daySettingCurrent == dtAway         ) selectScreen(scnSettingsDaySchedule, cmdOverruleTodayAway );
      }
      else selectScreen(scnSettingsMain, cmdMenuMain);
    break;
  
    // SCREEN scnSettingsDaySchedule
    case scnSettingsDaySchedule:

      switch( menu.selectedMenuItem() ) {
        case cmdWorkFromHome:      menu.daySettingCurrent = dtWorkFromHome; break;
        case cmdWorkAtOffice:      menu.daySettingCurrent = dtWorkAtOffice; break;
        case cmdWeekend:           menu.daySettingCurrent = dtWeekend;      break;
        case cmdOverruleTodayAway: menu.daySettingCurrent = dtAway;         break;
      }
  
      if ( menu.selectedMenuItem() != cmdMenuWeekSchedule  ) {
        switch( menu.daySettingComingFrom) {
          case cmdSunday:    controllerData.regularWeek[0] = menu.daySettingCurrent; break;
          case cmdMonday:    controllerData.regularWeek[1] = menu.daySettingCurrent; break;
          case cmdTuesday:   controllerData.regularWeek[2] = menu.daySettingCurrent; break;
          case cmdWednesday: controllerData.regularWeek[3] = menu.daySettingCurrent; break;
          case cmdThursday:  controllerData.regularWeek[4] = menu.daySettingCurrent; break;
          case cmdFriday:    controllerData.regularWeek[5] = menu.daySettingCurrent; break;
          case cmdSaturday:  controllerData.regularWeek[6] = menu.daySettingCurrent; break;
        }
      }
  
      selectScreen(scnSettingsWeekSchedule);
    break;
  
    // SCREEN scnSettingsHomeTimes
    case scnSettingsHomeTimes:
      if     ( menu.selectedMenuItem() == cmdHomeWakeUpLater      ) sendMessageToController(cmdHomeWakeUpLater);
      else if ( menu.selectedMenuItem() == cmdHomeWakeUpEarlier    ) sendMessageToController(cmdHomeWakeUpEarlier);
      else if ( menu.selectedMenuItem() == cmdHomeGoToSleepLater   ) sendMessageToController(cmdHomeGoToSleepLater);
      else if ( menu.selectedMenuItem() == cmdHomeGoToSleepEarlier ) sendMessageToController(cmdHomeGoToSleepEarlier);
      else if ( menu.selectedMenuItem() == cmdMenuSettings         ) selectScreen(scnSettingsMain);
    break;
  
    // SCREEN scnSettingsOfficeTimes
    case scnSettingsOfficeTimes:
      switch( menu.selectedMenuItem() ) {
        case cmdHomeWakeUpLater:       sendMessageToController(cmdOfficeWakeUpLater);      break;  
        case cmdHomeWakeUpEarlier:     sendMessageToController(cmdOfficeWakeUpEarlier);    break;
        case cmdOfficeLeaveLater:      sendMessageToController(cmdOfficeLeaveLater);       break;
        case cmdOfficeLeaveEarlier:    sendMessageToController(cmdOfficeLeaveEarlier);     break;
        case cmdOfficeComeHomeLater:   sendMessageToController(cmdOfficeComeHomeLater);    break;
        case cmdOfficeComeHomeEarlier: sendMessageToController(cmdOfficeComeHomeEarlier);  break;
        case cmdHomeGoToSleepLater:    sendMessageToController(cmdOfficeGoToSleepLater);   break;
        case cmdHomeGoToSleepEarlier:  sendMessageToController(cmdOfficeGoToSleepEarlier); break;
        case cmdMenuSettings:          selectScreen(scnSettingsMain);                      break;
      };  
    break;
  
    // SCREEN scnSettingsWeekendTimes
    case scnSettingsWeekendTimes:
      if      ( menu.selectedMenuItem() == cmdWeekendWakeUpLater      ) sendMessageToController(cmdWeekendWakeUpLater);
      else if ( menu.selectedMenuItem() == cmdWeekendWakeUpEarlier    ) sendMessageToController(cmdWeekendWakeUpEarlier);
      else if ( menu.selectedMenuItem() == cmdWeekendGoToSleepLater   ) sendMessageToController(cmdWeekendGoToSleepLater);
      else if ( menu.selectedMenuItem() == cmdWeekendGoToSleepEarlier ) sendMessageToController(cmdWeekendGoToSleepEarlier);
      else if ( menu.selectedMenuItem() == cmdMenuSettings            ) selectScreen(scnSettingsMain);
    break;
  } // switch( menu.selectedScreen() }
            
}

void checkMenu() {
  userEventMessage_t message;

  if ( xQueueReceive( menuQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Menu [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed);

    switch( message.command ) {
      
        case cmdKeyUp:

          // Ignore first key press, first wake up backlight
          if( controllerData.backLightOn ) {
            if (menu.selectedScreen() == scnHome) { 
              sendMessageToController(cmdSetpointHigher);
            }
            else {
              menu.prevMenuItem();
              sendMessageToController(cmdUpdateScreen);
            }
          } // controllerData.backLightOn 

          // Disable Telegram until backlight switches off
          sendMessage(sndMenu, cmdDisableTelegram, telegramQueue);

          // Set the backlight timer
          sendMessage(sndMenu, cmdBacklightOn, backlightQueue);
          
          break;
  
        case cmdKeySelect:
          
          // Ignore first key press, first wake up backlight
          if( controllerData.backLightOn ) {
            HandleSelectButton();  
          } // controllerData.backLightOn 

          // Disable Telegram until backlight switches off
          sendMessage(sndMenu, cmdDisableTelegram, telegramQueue);

          // Set the backlight timer
          sendMessage(sndMenu, cmdBacklightOn, backlightQueue);

          break;
  
        case cmdKeyDown:

          // Ignore first key press, first wake up backlight
          if( controllerData.backLightOn ) {
            if (menu.selectedScreen() == scnHome) {
              sendMessage(sndMenu, cmdSetpointLower, controllerQueue);
            }
            else {
              menu.nextMenuItem();
              sendMessage(sndMenu, cmdUpdateScreen, controllerQueue);
            }
          } // controllerData.backLightOn 

          // Disable Telegram until backlight switches off
          sendMessage(sndMenu, cmdDisableTelegram, telegramQueue);

          // Set the backlight timer
          sendMessage(sndMenu, cmdBacklightOn, backlightQueue);

          break;

        case cmdMenuHome:
          // The backlight sends a message that it is switching off
          // Jump to the home screen but do not switch on the backlight again
          menu.selectScreen(scnHome);
          Serial.println("Menu flipping back to home screen since backlight has switched off");          
          break;
          
      } // switch()

  } // xQueueReceive
}
