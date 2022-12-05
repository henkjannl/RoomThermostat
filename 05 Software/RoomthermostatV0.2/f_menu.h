#pragma once

#include <map>
#include <list>

#define BACKLIGHT_TIME_OUT 60*1000

class Screen {
  public:
    Screen() { }; // Never used, but required when class is used in a map
    Screen(std::list<command_t> menu);
    
    void firstItem() { selection = std::begin(menu); };
    void prevItem()  { if( selection !=std::begin(menu) ) --selection; };
    void nextItem()  { if( selection !=--std::end(menu) ) ++selection; };
    
    void selectItem(command_t menuItem);
    command_t selectedItem() { return *selection; };

    std::list<command_t> menu;
    std::list<command_t>::iterator selection;
};

Screen::Screen(std::list<command_t> menu) {
  this->menu=menu;
  firstItem();  
}

void Screen::selectItem(command_t menuItem) {
  bool itemfound=false;
  for(std::list<command_t>::iterator item = std::begin(menu); item != std::end(menu); item++) {
    if(*item==menuItem) {
      selection=item;
      itemfound=true;
      break;
    }  
  }

  // Always produce a valid result, even when item is not found
  if(!itemfound) std::begin(menu);
};


typedef std::map<screen_t, Screen > umScreen_t;

class MenuStructure{
  public:
    MenuStructure(screenInitializer_t structure);
    void selectScreen(screen_t screen) { currentScreen=screen; };
    void selectScreen(screen_t screen, command_t menuItem) { currentScreen=screen; screens[screen].selectItem(menuItem); };
    void selectMenuItem(screen_t screen, command_t menuItem) { screens[screen].selectItem(menuItem); };

    void prevMenuItem()  { screens[currentScreen].prevItem();  };
    void nextMenuItem()  { screens[currentScreen].nextItem();  };
    void firstMenuItem() { screens[currentScreen].firstItem(); };
    
    screen_t selectedScreen()        { return currentScreen;                         };
    command_t selectedMenuItem()     { return screens[currentScreen].selectedItem(); };
    std::list<command_t> menuItems() { return screens[currentScreen].menu;           };   

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
  for(auto & screen : structure) {
    newScreen = Screen(screen.second);
    newScreen.firstItem();
    screens[screen.first]=newScreen;    
  }
  currentScreen=(*std::begin(screens)).first; // By default, select the first screen
};

void MenuStructure::dump() {
  for(auto & screen : screens) {
    screen.second.firstItem();
    Serial.println(screenTitle[screen.first].c_str() );
    Serial.println(String("*")+commandLabels[screen.second.selectedItem()].c_str()+"*");
    for( auto & command : screen.second.menu ) {
      Serial.print( screen.second.selectedItem()==command ? ">" : " "); 
      Serial.println(String(" - ")+commandLabels[command].c_str() );      
    }
  }
  
};

/*******************************************************
 *   GLOBAL VARIABLES                                  *
 *******************************************************/

MenuStructure menu = MenuStructure(SCREENS); // Screens and menus of hardware unit

// Forward declarations
void runMenu( void *pvParameters );
void HandleSelectButton();

// Setup the keyboard handler
void startMenu() {

  menu.selectMenuItem(scnHome, cmdMenuMain);
  menu.dump();
    
  // Setup task
  xTaskCreatePinnedToCore(
    runMenu,
    "Menu",   // A name just for humans
    4096,                   // This stack size can be checked & adjusted by reading the Stack Highwater
    NULL,
    2,                      // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    NULL, 
    ARDUINO_RUNNING_CORE);
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
  userEventMessage_t message(screen, menuItem, cmdUpdateScreen );

  // Only menu or screen changes, skip the controller and directly talk to the display
  xQueueSend( displayQueue, &message, ( TickType_t ) 10 );    
};
  
void selectScreen(screen_t screen) {
  menu.selectScreen(screen);  
  userEventMessage_t message(screen, menu.selectedMenuItem(), cmdUpdateScreen );

  // Only menu or screen changes, skip the controller and directly talk to the display
  xQueueSend( displayQueue, &message, ( TickType_t ) 10 );    
};

void runMenu( void *pvParameters ) {
  userEventMessage_t message;
  bool backlight;

  while(true) {

    if( xQueueReceive( menuQueue, &message, 0) == pdPASS ) {

      Serial.printf("%s > Menu [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed);

      // Copy value of backlight by protection of semaphore
      if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
        backlight = controllerData.backLightOn;
        xSemaphoreGive( controllerDataSemaphore );
      }

      if( !backlight) {
        userEventMessage_t backlightCommand = userEventMessage_t(sndMenu, cmdBacklightOn);
        xQueueSend( backlightQueue, &backlightCommand, ( TickType_t ) 10 );  
      }
      else {

        // Ignore the keystroke if the backlight was off
        switch(message.command) {
          case cmdKeyUp:
            if(menu.selectedScreen()==scnHome) {
              sendMessageToController(cmdSetpointHigher); 
            }
            else {
              menu.prevMenuItem();
              userEventMessage_t message(menu.selectedScreen(), menu.selectedMenuItem(), cmdUpdateScreen);
              xQueueSend( displayQueue, &message, ( TickType_t ) 10 );    
            }
          break;
  
          case cmdKeySelect:
            if( menu.selectedScreen() == scnHome ) {
              selectScreen(scnMain);
            }
            else {
              HandleSelectButton();
            }
  
          break;
  
          case cmdKeyDown:
            if(menu.selectedScreen()==scnHome) {
              sendMessageToController(cmdSetpointLower);
            }
            else {
              menu.nextMenuItem();
              userEventMessage_t message(menu.selectedScreen(), menu.selectedMenuItem(), cmdUpdateScreen);
              xQueueSend( displayQueue, &message, ( TickType_t ) 10 );    
            }
          break;
  
          // Backlight puts menu back to home screen
          case cmdMenuHome:
            menu.selectScreen(scnHome);

            userEventMessage_t homeCommand = userEventMessage_t(sndMenu, cmdMenuHome);
            homeCommand.screen           = scnHome;
            homeCommand.selectedMenuItem = cmdMenuMain;
            xQueueSend( displayQueue, &homeCommand, ( TickType_t ) 10 );  
            
            break;
          
        } // switch          
      } // backlight
    } // xQueueReceive

    vTaskDelay(100 / portTICK_PERIOD_MS);
  } // while true
}

void HandleSelectButton() {

  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

    if( menu.selectedScreen() == scnMain ) {
      if ( menu.selectedMenuItem() == cmdMenuOverruleToday ) { 
        if     (controllerData.overrideToday == dtWorkFromHome) selectScreen(scnOverruleToday, cmdWorkFromHome);   
        else if(controllerData.overrideToday == dtWorkAtOffice) selectScreen(scnOverruleToday, cmdWorkAtOffice); 
        else if(controllerData.overrideToday == dtAway        ) selectScreen(scnOverruleToday, cmdOverruleTodayAway);     
        else if(controllerData.overrideToday == dtWeekend     ) selectScreen(scnOverruleToday, cmdWeekend);        
        else if(controllerData.overrideToday == dtAuto        ) selectScreen(scnOverruleToday, cmdOverruleTodayAutomatic);      
        }
      else if ( menu.selectedMenuItem() == cmdMenuOverruleTomorrow ) { 
        if     (controllerData.overrideTomorrow == dtWorkFromHome) selectScreen(scnOverruleTomorrow, cmdWorkFromHome);   
        else if(controllerData.overrideTomorrow == dtWorkAtOffice) selectScreen(scnOverruleTomorrow, cmdWorkAtOffice); 
        else if(controllerData.overrideTomorrow == dtAway        ) selectScreen(scnOverruleTomorrow, cmdOverruleTodayAway);     
        else if(controllerData.overrideTomorrow == dtWeekend     ) selectScreen(scnOverruleTomorrow, cmdWeekend);        
        else if(controllerData.overrideTomorrow == dtAuto        ) selectScreen(scnOverruleTomorrow, cmdOverruleTodayAutomatic);      
        }
      else if ( menu.selectedMenuItem() == cmdMenuOverruleMultipleDays ) { 
        if     (controllerData.overrideMultiple == dtWorkFromHome ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);  
        else if(controllerData.overrideMultiple == dtWorkAtOffice ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);  
        else if(controllerData.overrideMultiple == dtAway         ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);  
        else if(controllerData.overrideMultiple == dtWeekend      ) selectScreen(scnOverruleMultiple,         cmdOverruleMultipleMoreDays);  
        else if(controllerData.overrideMultiple == dtAuto         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAutomatic); 
      }
      else if ( menu.selectedMenuItem() == cmdMenuSettings ) selectScreen(scnSettingsMain); 
      else if ( menu.selectedMenuItem() == cmdMenuHome )     selectScreen(scnHome); 
    }
  
    // SCREEN scnOverruleToday
    else if( menu.selectedScreen() ==scnOverruleToday) {
  
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == dtWorkFromHome ) sendMessageToController(cmdOverruleTodayWorkFromHome); 
      else if( menu.selectedMenuItem() == dtWorkAtOffice ) sendMessageToController(cmdOverruleTodayWorkAtOffice); 
      else if( menu.selectedMenuItem() == dtAway         ) sendMessageToController(cmdOverruleTodayAway); 
      else if( menu.selectedMenuItem() == dtWeekend      ) sendMessageToController(cmdOverruleTodayWeekend); 
      else if( menu.selectedMenuItem() == dtAuto         ) sendMessageToController(cmdOverruleTodayAutomatic); 
  
      selectScreen(scnMain, cmdMenuMain);
    }
  
    // SCREEN scnOverruleTomorrow
    else if( menu.selectedScreen() ==scnOverruleTomorrow) {
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == cmdWorkFromHome    ) sendMessageToController(cmdOverruleTomorrowWorkFromHome); 
      else if( menu.selectedMenuItem() == cmdWorkAtOffice    ) sendMessageToController(cmdOverruleTomorrowWorkAtOffice); 
      else if( menu.selectedMenuItem() == cmdOverruleTodayAway ) sendMessageToController(cmdOverruleTomorrowWorkAtOffice); 
      else if( menu.selectedMenuItem() == cmdWeekend         ) sendMessageToController(cmdOverruleTomorrowWeekend); 
      else if( menu.selectedMenuItem() == cmdOverruleTodayAutomatic  ) sendMessageToController(cmdOverruleTomorrowAutomatic); 
      //else if( menu.selectedMenuItem() == cmdMenuMain) { };
      selectScreen(scnMain, cmdMenuMain);
    }
  
    // SCREEN scnOverruleMultiple
    else if( menu.selectedScreen() ==scnOverruleMultiple) {
      if     ( menu.selectedMenuItem() == cmdOverruleMultipleMoreDays    ) sendMessageToController(cmdOverruleMultipleMoreDays);   
      else if( menu.selectedMenuItem() == cmdOverruleMultipleFewerDays   ) sendMessageToController(cmdOverruleMultipleFewerDays);
      else if( menu.selectedMenuItem() == cmdMultipleDaySchedule ) {
        if     (controllerData.overrideMultiple==dtWorkFromHome ) selectScreen(scnOverruleMultipleSchedule, cmdWorkFromHome);   
        else if(controllerData.overrideMultiple==dtWorkAtOffice ) selectScreen(scnOverruleMultipleSchedule, cmdWorkAtOffice); 
        else if(controllerData.overrideMultiple==dtAway         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAway);     
        else if(controllerData.overrideMultiple==dtWeekend      ) selectScreen(scnOverruleMultipleSchedule, cmdWeekend);        
        else if(controllerData.overrideMultiple==dtAuto         ) selectScreen(scnOverruleMultipleSchedule, cmdOverruleTodayAutomatic);      
      }
      else if( menu.selectedMenuItem() == cmdOverruleMultipleAutomatic) { 
        sendMessageToController(cmdOverruleMultipleAutomatic); 
        selectScreen(scnMain, cmdMenuMain); 
      }
      else selectScreen(scnMain, cmdMenuMain); 
    }
  
    // SCREEN scnOverruleMultipleSchedule
    else if( menu.selectedScreen() ==scnOverruleMultipleSchedule) {
      if     ( menu.selectedMenuItem() == cmdWorkFromHome    ) sendMessageToController(cmdOverruleMultipleWorkFromHome);   
      else if( menu.selectedMenuItem() == cmdWorkAtOffice    ) sendMessageToController(cmdOverruleMultipleWorkAtOffice); 
      else if( menu.selectedMenuItem() == cmdOverruleTodayAway ) sendMessageToController(cmdOverruleMultipleAway);           
      else if( menu.selectedMenuItem() == cmdWeekend         ) sendMessageToController(cmdOverruleMultipleWeekend);        
      else if( menu.selectedMenuItem() == cmdOverruleTodayAutomatic  ) sendMessageToController(cmdOverruleMultipleAutomatic);      
      // else if( menu.selectedMenuItem() == mnBackToPreviousMenu  ) {  }
      selectScreen(scnOverruleMultiple, cmdMenuMain);
    }
  
    // SCREEN scnSettingsMain
    else if( menu.selectedScreen() ==scnSettingsMain) {
      if     ( menu.selectedMenuItem() == cmdMenuTemperature  ) selectScreen(scnSettingsTemperature, cmdMenuSettings);
      else if( menu.selectedMenuItem() == cmdMenuSensorOffset ) selectScreen(scnSettingsSensorOffset, cmdMenuSettings);     
      else if( menu.selectedMenuItem() == cmdMenuWeekSchedule ) selectScreen(scnSettingsWeekSchedule, cmdMenuSettings); 
      else if( menu.selectedMenuItem() == cmdMenuHomeTimes    ) selectScreen(scnSettingsHomeTimes, cmdMenuSettings); 
      else if( menu.selectedMenuItem() == cmdMenuOfficeTimes  ) selectScreen(scnSettingsOfficeTimes, cmdMenuSettings); 
      else if( menu.selectedMenuItem() == cmdMenuWeekendTimes ) selectScreen(scnSettingsWeekendTimes, cmdMenuSettings); 
      else if( menu.selectedMenuItem() == cmdMenuMain         ) selectScreen(scnMain,cmdMenuMain);
    }
  
    // SCREEN scnSettingsTemperature
    else if( menu.selectedScreen() ==scnSettingsTemperature) {
      if     ( menu.selectedMenuItem() == cmdHighTemperatureUp   ) sendMessageToController(cmdHighTemperatureUp);
      else if( menu.selectedMenuItem() == cmdHighTemperatureDown ) sendMessageToController(cmdHighTemperatureDown);
      else if( menu.selectedMenuItem() == cmdLowTemperatureUp    ) sendMessageToController(cmdLowTemperatureUp);
      else if( menu.selectedMenuItem() == cmdLowTemperatureDown  ) sendMessageToController(cmdLowTemperatureDown);
      else if( menu.selectedMenuItem() == cmdMenuSettings        ) selectScreen(scnSettingsMain);
    }
  
    // SCREEN scnSettingsSensorOffset
    else if( menu.selectedScreen() ==scnSettingsSensorOffset) {
      if     ( menu.selectedMenuItem() == cmdSensorOffsetUp   ) sendMessageToController(cmdSensorOffsetUp); 
      else if( menu.selectedMenuItem() == cmdSensorOffsetDown ) sendMessageToController(cmdSensorOffsetDown); 
      else if( menu.selectedMenuItem() == cmdMenuSettings     ) selectScreen(scnMain); 
    }
  
    // SCREEN scnSettingsWeekSchedule
    else if( menu.selectedScreen() ==scnSettingsWeekSchedule) {
      menu.daySettingComingFrom=menu.selectedMenuItem();
  
      if(     menu.daySettingComingFrom==cmdSunday)    menu.daySettingCurrent=controllerData.regularWeek[0];
      else if(menu.daySettingComingFrom==cmdMonday)    menu.daySettingCurrent=controllerData.regularWeek[1];
      else if(menu.daySettingComingFrom==cmdTuesday)   menu.daySettingCurrent=controllerData.regularWeek[2];
      else if(menu.daySettingComingFrom==cmdWednesday) menu.daySettingCurrent=controllerData.regularWeek[3];
      else if(menu.daySettingComingFrom==cmdThursday)  menu.daySettingCurrent=controllerData.regularWeek[4];
      else if(menu.daySettingComingFrom==cmdFriday)    menu.daySettingCurrent=controllerData.regularWeek[5];
      else if(menu.daySettingComingFrom==cmdSaturday)  menu.daySettingCurrent=controllerData.regularWeek[6];

      if( menu.selectedMenuItem() != cmdMenuSettings  ) { 
        if(     menu.daySettingCurrent==dtWorkFromHome) selectScreen(scnSettingsDaySchedule, cmdWorkFromHome  );
        else if(menu.daySettingCurrent==dtWorkAtOffice) selectScreen(scnSettingsDaySchedule, cmdWorkAtOffice);
        else if(menu.daySettingCurrent==dtWeekend     ) selectScreen(scnSettingsDaySchedule, cmdWeekend       );
        else if(menu.daySettingCurrent==dtAway        ) selectScreen(scnSettingsDaySchedule, cmdOverruleTodayAway    );
        else selectScreen(scnSettingsMain, cmdMenuMain); 
        }
      }    
  
    // SCREEN scnSettingsDaySchedule
    else if( menu.selectedScreen() ==scnSettingsDaySchedule) {
      
      if     ( menu.selectedMenuItem() == cmdWorkFromHome    ) menu.daySettingCurrent=dtWorkFromHome;
      else if( menu.selectedMenuItem() == cmdWorkAtOffice    ) menu.daySettingCurrent=dtWorkAtOffice;
      else if( menu.selectedMenuItem() == cmdWeekend         ) menu.daySettingCurrent=dtWeekend;
      else if( menu.selectedMenuItem() == cmdOverruleTodayAway ) menu.daySettingCurrent=dtAway;
  
      if( menu.selectedMenuItem() != cmdMenuWeekSchedule  ) {  
          if(      menu.daySettingComingFrom == cmdSunday)    controllerData.regularWeek[0]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdMonday)    controllerData.regularWeek[1]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdTuesday)   controllerData.regularWeek[2]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdWednesday) controllerData.regularWeek[3]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdThursday)  controllerData.regularWeek[4]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdFriday)    controllerData.regularWeek[5]=menu.daySettingCurrent;
          else if( menu.daySettingComingFrom == cmdSaturday)  controllerData.regularWeek[6]=menu.daySettingCurrent;
        }
  
      selectScreen(scnSettingsWeekSchedule);
    }
  
    // SCREEN scnSettingsHomeTimes
    else if( menu.selectedScreen() ==scnSettingsHomeTimes) {
      if     ( menu.selectedMenuItem() == cmdHomeWakeUpLater      ) sendMessageToController(cmdHomeWakeUpLater);
      else if( menu.selectedMenuItem() == cmdHomeWakeUpEarlier    ) sendMessageToController(cmdHomeWakeUpEarlier);
      else if( menu.selectedMenuItem() == cmdHomeGoToSleepLater   ) sendMessageToController(cmdHomeGoToSleepLater);
      else if( menu.selectedMenuItem() == cmdHomeGoToSleepEarlier ) sendMessageToController(cmdHomeGoToSleepEarlier);
      else if( menu.selectedMenuItem() == cmdMenuSettings         ) selectScreen(scnSettingsMain);
    }
  
    // SCREEN scnSettingsOfficeTimes
    else if( menu.selectedScreen() ==scnSettingsOfficeTimes) {
      if     ( menu.selectedMenuItem() == cmdHomeWakeUpLater       ) sendMessageToController(cmdOfficeWakeUpLater);
      else if( menu.selectedMenuItem() == cmdHomeWakeUpEarlier     ) sendMessageToController(cmdOfficeWakeUpEarlier);
      else if( menu.selectedMenuItem() == cmdOfficeLeaveLater      ) sendMessageToController(cmdOfficeLeaveLater);
      else if( menu.selectedMenuItem() == cmdOfficeLeaveEarlier    ) sendMessageToController(cmdOfficeLeaveEarlier);
      else if( menu.selectedMenuItem() == cmdOfficeComeHomeLater   ) sendMessageToController(cmdOfficeComeHomeLater);
      else if( menu.selectedMenuItem() == cmdOfficeComeHomeEarlier ) sendMessageToController(cmdOfficeComeHomeEarlier);
      else if( menu.selectedMenuItem() == cmdHomeGoToSleepLater    ) sendMessageToController(cmdOfficeGoToSleepLater);
      else if( menu.selectedMenuItem() == cmdHomeGoToSleepEarlier  ) sendMessageToController(cmdOfficeGoToSleepEarlier);
      else if( menu.selectedMenuItem() == cmdMenuSettings          ) selectScreen(scnSettingsMain);
    }
  
    // SCREEN scnSettingsWeekendTimes
    else if( menu.selectedScreen() ==scnSettingsWeekendTimes) {
      if     ( menu.selectedMenuItem() == cmdWeekendWakeUpLater      ) sendMessageToController(cmdWeekendWakeUpLater);
      else if( menu.selectedMenuItem() == cmdWeekendWakeUpEarlier    ) sendMessageToController(cmdWeekendWakeUpEarlier);
      else if( menu.selectedMenuItem() == cmdWeekendGoToSleepLater   ) sendMessageToController(cmdWeekendGoToSleepLater);
      else if( menu.selectedMenuItem() == cmdWeekendGoToSleepEarlier ) sendMessageToController(cmdWeekendGoToSleepEarlier);
      else if( menu.selectedMenuItem() == cmdMenuSettings            ) selectScreen(scnSettingsMain);
    }

    xSemaphoreGive( controllerDataSemaphore );
  }

}
