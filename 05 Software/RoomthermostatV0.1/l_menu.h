#pragma once

// Response of the menu if the Up button is pressed
void menuUpButtonPressed(DataBus & databus, MenuStructure & menu, Backlight & backlight, Controller & controller) {
  Serial.print("Up ");          

  if(backlight.alreadyOn()) {
    if(menu.selectedScreen()==scHomeScreen) {
      controller.setpointHigher();
    }
    else {
      Serial.printf("Current menu: %s > %s\n", menu.currentScreenTitle().c_str(), umLabel[menu.selectedMenuItem()].c_str() );
      Serial.println(menu.currentScreenTitle().c_str() );
      menu.prevMenuItem();
    }
  }
  
  controller.control();
}


// Response of the menu if the Down button is pressed
void menuDownButtonPressed(DataBus & databus, MenuStructure & menu, Backlight & backlight, Controller & controller) {
  Serial.print("Down ");          

  if(backlight.alreadyOn()) {
    if(menu.selectedScreen()==scHomeScreen) {
      controller.setpointLower();
    }
    else {
      Serial.printf("Current menu: %s > %s\n", menu.currentScreenTitle().c_str(), umLabel[menu.selectedMenuItem()].c_str() );
      menu.nextMenuItem();
    }
  }
  
  controller.control();
}
  
// Response of the menu if the Select button is pressed
void menuSelectButtonPressed(DataBus & databus, MenuStructure & menu, Backlight & backlight, Controller & controller) {
  static umMenuItem_t dayToBeOverruled = mnOverruleToday;
  static umMenuItem_t daySettingComingFrom;
  static dayType_t    daySettingCurrent; 

  if(backlight.alreadyOn()) {
    // SCREEN scHomeScreen
    if( menu.selectedScreen() ==scHomeScreen) {
      menu.selectScreen(scMain);
    }
  
    // SCREEN scMain
    else if( menu.selectedScreen() ==scMain) {
      
      if ( menu.selectedMenuItem() == mnOverruleToday ) { 
        if     (databus.overrideToday == dtWorkFromHome) menu.selectScreen(scOverruleToday, mnWorkFromHome);   
        else if(databus.overrideToday == dtWorkAtOffice) menu.selectScreen(scOverruleToday, mnWorkAtOffice); 
        else if(databus.overrideToday == dtAway        ) menu.selectScreen(scOverruleToday, mnAllDayAway);     
        else if(databus.overrideToday == dtWeekend     ) menu.selectScreen(scOverruleToday, mnWeekend);        
        else if(databus.overrideToday == dtAuto        ) menu.selectScreen(scOverruleToday, mnAutomatic);      
      }
      else if( menu.selectedMenuItem() == mnOverruleTomorrow    ) { 
        if     (databus.overrideTomorrow == dtWorkFromHome) menu.selectScreen(scOverruleTomorrow, mnWorkFromHome);   
        else if(databus.overrideTomorrow == dtWorkAtOffice) menu.selectScreen(scOverruleTomorrow, mnWorkAtOffice); 
        else if(databus.overrideTomorrow == dtAway        ) menu.selectScreen(scOverruleTomorrow, mnAllDayAway);     
        else if(databus.overrideTomorrow == dtWeekend     ) menu.selectScreen(scOverruleTomorrow, mnWeekend);        
        else if(databus.overrideTomorrow == dtAuto        ) menu.selectScreen(scOverruleTomorrow, mnAutomatic);      
      }
      else if( menu.selectedMenuItem() == mnOverruleMultipleDays) { 
        if     (databus.overrideMultiple == dtWorkFromHome) menu.selectScreen(scOverruleMultiple,  mnMoreDays);  
        else if(databus.overrideMultiple == dtWorkAtOffice) menu.selectScreen(scOverruleMultiple,  mnMoreDays);  
        else if(databus.overrideMultiple == dtAway        ) menu.selectScreen(scOverruleMultiple,  mnMoreDays);  
        else if(databus.overrideMultiple == dtWeekend     ) menu.selectScreen(scOverruleMultiple,  mnMoreDays);  
        else if(databus.overrideMultiple == dtAuto        ) menu.selectScreen(scOverruleMultiple2, mnAutomatic); 
      }
      else if( menu.selectedMenuItem() == mnSettings) menu.selectScreen(scSettings); 
      else if( menu.selectedMenuItem() == mnExitMenu) menu.selectScreen(scHomeScreen); 
    }
  
    // SCREEN scOverruleToday
    else if( menu.selectedScreen() ==scOverruleToday) {
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == mnWorkFromHome  ) controller.overruleTodayWorkFromHome(); 
      else if( menu.selectedMenuItem() == mnWorkAtOffice) controller.overruleTodayWorkFromOffice(); 
      else if( menu.selectedMenuItem() == mnAllDayAway    ) controller.overruleTodayAway(); 
      else if( menu.selectedMenuItem() == mnWeekend       ) controller.overruleTodayWeekend(); 
      else if( menu.selectedMenuItem() == mnAutomatic     ) controller.overruleTodayAutomatic(); 
      //else if( menu.selectedMenuItem() == mnBackToMainMenu) { };
  
      menu.selectScreen(scMain, mnExitMenu);
    }
  
    // SCREEN scOverruleTomorrow
    else if( menu.selectedScreen() ==scOverruleTomorrow) {
      // Change the day in the controller program
      if     ( menu.selectedMenuItem() == mnWorkFromHome  ) controller.overruleTomorrowWorkFromHome(); 
      else if( menu.selectedMenuItem() == mnWorkAtOffice) controller.overruleTomorrowWorkFromOffice(); 
      else if( menu.selectedMenuItem() == mnAllDayAway    ) controller.overruleTomorrowAway(); 
      else if( menu.selectedMenuItem() == mnWeekend       ) controller.overruleTomorrowWeekend(); 
      else if( menu.selectedMenuItem() == mnAutomatic     ) controller.overruleTomorrowAutomatic(); 
      //else if( menu.selectedMenuItem() == mnBackToMainMenu) { };
      menu.selectScreen(scMain, mnExitMenu);
    }
  
    // SCREEN scOverruleMultiple
    else if( menu.selectedScreen() ==scOverruleMultiple) {
      if     ( menu.selectedMenuItem() == mnMoreDays            ) controller.overruleMultipleMoreDays();   
      else if( menu.selectedMenuItem() == mnFewerDays           ) controller.overruleMultipleLessDays();
      else if( menu.selectedMenuItem() == mnSetDaySchedule      ) {
        if     (databus.overrideMultiple==dtWorkFromHome) menu.selectScreen(scOverruleMultiple2, mnWorkFromHome);   
        else if(databus.overrideMultiple==dtWorkAtOffice) menu.selectScreen(scOverruleMultiple2, mnWorkAtOffice); 
        else if(databus.overrideMultiple==dtAway)         menu.selectScreen(scOverruleMultiple2, mnAllDayAway);     
        else if(databus.overrideMultiple==dtWeekend)      menu.selectScreen(scOverruleMultiple2, mnWeekend);        
        else if(databus.overrideMultiple==dtAuto)         menu.selectScreen(scOverruleMultiple2, mnAutomatic);      
      }
      else if( menu.selectedMenuItem() == mnResetToAutomatic    ) { 
        controller.overruleMultipleAutomatic(); 
        menu.selectScreen(scMain, mnExitMenu); 
      }
      else menu.selectScreen(scMain, mnExitMenu); 
    }
  
    // SCREEN scOverruleMultiple2
    else if( menu.selectedScreen() ==scOverruleMultiple2) {
      if     ( menu.selectedMenuItem() == mnWorkFromHome        ) controller.overruleMultipleWorkFromHome();   
      else if( menu.selectedMenuItem() == mnWorkAtOffice      ) controller.overruleMultipleWorkFromOffice(); 
      else if( menu.selectedMenuItem() == mnAllDayAway          ) controller.overruleMultipleAway();           
      else if( menu.selectedMenuItem() == mnWeekend             ) controller.overruleMultipleWeekend();        
      else if( menu.selectedMenuItem() == mnAutomatic           ) controller.overruleMultipleAutomatic();      
      // else if( menu.selectedMenuItem() == mnBackToPreviousMenu  ) {  }
      menu.selectScreen(scOverruleMultiple, mnBackToMainMenu);
    }
  
    // SCREEN scSettings
    else if( menu.selectedScreen() ==scSettings) {
      if     ( menu.selectedMenuItem() == mnSetTemperatures     ) menu.selectScreen(scTemperatureSettings, mnBackToSettingsMenu);
      else if( menu.selectedMenuItem() == mnSetSensorOffset     ) menu.selectScreen(scAdjustSensorOffset, mnBackToSettingsMenu);     
      else if( menu.selectedMenuItem() == mnSetWeekSchedule     ) menu.selectScreen(scSetWeekSchedule, mnBackToSettingsMenu); 
      else if( menu.selectedMenuItem() == mnWorkFromHomeTimes   ) menu.selectScreen(scSetHomeTimes, mnBackToSettingsMenu); 
      else if( menu.selectedMenuItem() == mnWorkFromOfficeTimes ) menu.selectScreen(scSetOfficeTimes, mnBackToSettingsMenu); 
      else if( menu.selectedMenuItem() == mnSetWeekendTimes     ) menu.selectScreen(scSetWeekendTimes, mnBackToSettingsMenu); 
      else if( menu.selectedMenuItem() == mnBackToMainMenu      ) menu.selectScreen(scMain,mnExitMenu);
    }
  
    // SCREEN scTemperatureSettings
    else if( menu.selectedScreen() ==scTemperatureSettings) {
      if     ( menu.selectedMenuItem() == mnHighTemperatureUp   ) controller.highTempHigher();
      else if( menu.selectedMenuItem() == mnHighTemperatureDown ) controller.highTempLower();
      else if( menu.selectedMenuItem() == mnLowTemperatureUp    ) controller.lowTempHigher();
      else if( menu.selectedMenuItem() == mnLowTemperatureDown  ) controller.lowTempLower();
      else if( menu.selectedMenuItem() == mnBackToSettingsMenu  ) menu.selectScreen(scSettings);
    }

    // SCREEN scAdjustSensorOffset
    else if( menu.selectedScreen() ==scAdjustSensorOffset) {
      if     ( menu.selectedMenuItem() == mnIncreaseOffset      ) controller.increaseSensorOffset(); 
      else if( menu.selectedMenuItem() == mnReduceOffset        ) controller.decreaseSensorOffset(); 
      else if( menu.selectedMenuItem() == mnBackToSettingsMenu  ) menu.selectScreen(scMain); 
    }
    
    // SCREEN scSetWeekSchedule
    else if( menu.selectedScreen() ==scSetWeekSchedule) {
      daySettingComingFrom=menu.selectedMenuItem();
      
      if(     daySettingComingFrom==mnSunday)    daySettingCurrent=databus.regularWeek[0];
      else if(daySettingComingFrom==mnMonday)    daySettingCurrent=databus.regularWeek[1];
      else if(daySettingComingFrom==mnTuesday)   daySettingCurrent=databus.regularWeek[2];
      else if(daySettingComingFrom==mnWednesday) daySettingCurrent=databus.regularWeek[3];
      else if(daySettingComingFrom==mnThursday)  daySettingCurrent=databus.regularWeek[4];
      else if(daySettingComingFrom==mnFriday)    daySettingCurrent=databus.regularWeek[5];
      else if(daySettingComingFrom==mnSaturday)  daySettingCurrent=databus.regularWeek[6];
  
      if( menu.selectedMenuItem() != mnBackToSettingsMenu  ) { 
        if(     daySettingCurrent==dtWorkFromHome) menu.selectScreen(scSetDaySchedule, mnWorkFromHome  );
        else if(daySettingCurrent==dtWorkAtOffice) menu.selectScreen(scSetDaySchedule, mnWorkAtOffice);
        else if(daySettingCurrent==dtWeekend     ) menu.selectScreen(scSetDaySchedule, mnWeekend       );
        else if(daySettingCurrent==dtAway        ) menu.selectScreen(scSetDaySchedule, mnAllDayAway    );
      }    
      else menu.selectScreen(scSettings, mnBackToMainMenu); 
    }
  
    // SCREEN scSetDaySchedule
    else if( menu.selectedScreen() ==scSetDaySchedule) {
      
      if     ( menu.selectedMenuItem() == mnWorkFromHome        ) daySettingCurrent=dtWorkFromHome;
      else if( menu.selectedMenuItem() == mnWorkAtOffice      ) daySettingCurrent=dtWorkAtOffice;
      else if( menu.selectedMenuItem() == mnWeekend             ) daySettingCurrent=dtWeekend;
      else if( menu.selectedMenuItem() == mnAllDayAway          ) daySettingCurrent=dtAway;
  
      if( menu.selectedMenuItem() != mnBackToWeekSchedule  ) {  
          if(     daySettingComingFrom==mnSunday)    databus.regularWeek[0]=daySettingCurrent;
          else if(daySettingComingFrom==mnMonday)    databus.regularWeek[1]=daySettingCurrent;
          else if(daySettingComingFrom==mnTuesday)   databus.regularWeek[2]=daySettingCurrent;
          else if(daySettingComingFrom==mnWednesday) databus.regularWeek[3]=daySettingCurrent;
          else if(daySettingComingFrom==mnThursday)  databus.regularWeek[4]=daySettingCurrent;
          else if(daySettingComingFrom==mnFriday)    databus.regularWeek[5]=daySettingCurrent;
          else if(daySettingComingFrom==mnSaturday)  databus.regularWeek[6]=daySettingCurrent;
        }
  
      menu.selectScreen(scSetWeekSchedule);
    }
  
    // SCREEN scSetHomeTimes
    else if( menu.selectedScreen() ==scSetHomeTimes) {
      if     ( menu.selectedMenuItem() == mnWakeUpLater         ) controller.workFromHomeWakeUpLater();
      else if( menu.selectedMenuItem() == mnWakeUpEarlier       ) controller.workFromHomeWakeUpEarlier();
      else if( menu.selectedMenuItem() == mnGoToSleepLater      ) controller.workFromHomeSleepLater();
      else if( menu.selectedMenuItem() == mnGoToSleepEarlier    ) controller.workFromHomeSleepEarlier();
      else if( menu.selectedMenuItem() == mnBackToSettingsMenu  ) menu.selectScreen(scSettings);
    }
  
    // SCREEN scSetOfficeTimes
    else if( menu.selectedScreen() ==scSetOfficeTimes) {
      if     ( menu.selectedMenuItem() == mnWakeUpLater         ) controller.workFromOfficeWakeUpLater();
      else if( menu.selectedMenuItem() == mnWakeUpEarlier       ) controller.workFromOfficeWakeUpEarlier();
      else if( menu.selectedMenuItem() == mnLeaveLater          ) controller.workFromOfficeGoOutLater();
      else if( menu.selectedMenuItem() == mnLeaveEarlier        ) controller.workFromOfficeGoOutEarlier();
      else if( menu.selectedMenuItem() == mnComeHomeLater       ) controller.workFromOfficeComeInLater();
      else if( menu.selectedMenuItem() == mnComeHomeEarlier     ) controller.workFromOfficeComeInEarlier();
      else if( menu.selectedMenuItem() == mnGoToSleepLater      ) controller.workFromOfficeSleepLater();
      else if( menu.selectedMenuItem() == mnGoToSleepEarlier    ) controller.workFromOfficeSleepEarlier();
      else if( menu.selectedMenuItem() == mnBackToSettingsMenu  ) menu.selectScreen(scSettings);
    }
  
    // SCREEN scSetWeekendTimes
    else if( menu.selectedScreen() ==scSetWeekendTimes) {
      if     ( menu.selectedMenuItem() == mnWakeUpLater         ) controller.weekendWakeUpLater();
      else if( menu.selectedMenuItem() == mnWakeUpEarlier       ) controller.weekendWakeUpEarlier();
      else if( menu.selectedMenuItem() == mnGoToSleepLater      ) controller.weekendSleepLater();
      else if( menu.selectedMenuItem() == mnGoToSleepEarlier    ) controller.weekendSleepEarlier();
      else if( menu.selectedMenuItem() == mnBackToSettingsMenu  ) menu.selectScreen(scSettings);
    }
  }
  
  controller.control();
}
