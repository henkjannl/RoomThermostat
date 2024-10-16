#pragma once

/* The controller module has three functions:
  processControllerIfNeeded() checks the controller queue, calls processControllerCommand() and forwards the message to other recipients if needed
  processControllerCommand() changes controller settings after receiving a message from the controller queue
  calculateNewSetpoint() calculates the new setpoint for the boiler, if the measured temperature is different or if controller settings have been changed
 */
void calculateNewSetpoint() {
  static int prevDay=0;
  static timeValue_t prevTime = timeValue_t(0,0);

  timeValue_t currTime;
  dayType_t currDayType;
  dayType_t daytype;
  
  Serial.println("calculateNewSetpoint()");

  // Current day and time
  time_t now;
  time(&now); // Get current time
  struct tm * localTime;
  localTime = localtime(&now);
  int currDay  = localTime->tm_yday;
  currTime = timeValue_t(localTime->tm_hour, localTime->tm_min);
  uint8_t dayOfWeek = localTime->tm_wday;

  // Calculate new setpoint

  // Shift days at midnight
  if(currDay != prevDay) {
    controllerData.overrideToday = controllerData.overrideTomorrow;
    controllerData.overrideTomorrow = dtAuto;
    
    if( controllerData.overrideMultiple != dtAuto and !controllerData.multipleForever ) {
      if( controllerData.overrideMultipleCount > 0 )
        controllerData.overrideMultipleCount--;
      else {
        controllerData.overrideMultiple = dtAuto;
        controllerData.overrideMultipleCount = 3;
      }
    }
  }

  // Determine what kind of day we have today
  currDayType=controllerData.regularWeek[dayOfWeek];
  controllerData.reasonForSetpoint = spWeekSchedule;

  if ( (controllerData.overrideMultiple!=dtAuto) and ( (controllerData.overrideMultipleCount>0) or (controllerData.multipleForever) ) ) {
    currDayType=controllerData.overrideMultiple;
    controllerData.reasonForSetpoint = spMultipleDays;
  }

  if(controllerData.overrideToday!=dtAuto) {
    currDayType=controllerData.overrideToday;
    controllerData.reasonForSetpoint = spOverrideToday;
  }

  // Check if we just passed a switch moment
  switch(currDayType) {

    case dtWorkFromHome:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < controllerData.workFromHomeWakeUp) and (controllerData.workFromHomeWakeUp <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromHomeWakeUp"); }
      else if( (prevTime < controllerData.workFromHomeSleep ) and (controllerData.workFromHomeSleep  <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromHomeSleep" ); };
      
      if(!controllerData.overrideTempNow) {
        controllerData.temperatureSetpoint = controllerData.lowTemp;
        if( (controllerData.workFromHomeWakeUp <= currTime) and (currTime < controllerData.workFromHomeSleep) ) controllerData.temperatureSetpoint = controllerData.highTemp;
      }
    break;

    case dtWorkAtOffice:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < controllerData.workAtOfficeWakeUp) and (controllerData.workAtOfficeWakeUp <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromOfficeWakeUp"); }
      else if( (prevTime < controllerData.workAtOfficeGoOut ) and (controllerData.workAtOfficeGoOut  <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromOfficeGoOut" ); }
      else if( (prevTime < controllerData.workAtOfficeComeIn) and (controllerData.workAtOfficeComeIn <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromOfficeComeIn"); }
      else if( (prevTime < controllerData.workAtOfficeSleep ) and (controllerData.workAtOfficeSleep  <= currTime) ) { controllerData.overrideTempNow=false; logMessage("WorkFromOfficeSleep" ); };
      
      if(!controllerData.overrideTempNow) {
        controllerData.temperatureSetpoint = controllerData.lowTemp;
        if     ( (controllerData.workAtOfficeWakeUp<=currTime) and (currTime<controllerData.workAtOfficeGoOut) ) controllerData.temperatureSetpoint = controllerData.highTemp;
        else if( (controllerData.workAtOfficeComeIn<=currTime) and (currTime<controllerData.workAtOfficeSleep) ) controllerData.temperatureSetpoint = controllerData.highTemp;
      }      
    break;

    case dtAway:
      if(currDay != prevDay) { controllerData.overrideTempNow=false; logMessage("Day away next day" ); };

      if(!controllerData.overrideTempNow) controllerData.temperatureSetpoint = controllerData.lowTemp;
    break;

    case dtWeekend:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<controllerData.weekendWakeUp) and (controllerData.weekendWakeUp<=currTime) ) { controllerData.overrideTempNow=false; logMessage("WeekendWakeUp"); }
      else if( (prevTime<controllerData.weekendSleep ) and (controllerData.weekendSleep <=currTime) ) { controllerData.overrideTempNow=false; logMessage("WeekendSleep" ); };
      
      if(!controllerData.overrideTempNow) {
        controllerData.temperatureSetpoint = controllerData.lowTemp;
        if( (controllerData.weekendWakeUp<=currTime) and (currTime<controllerData.weekendSleep) ) controllerData.temperatureSetpoint = controllerData.highTemp;
      }
    break;
  }

  if(controllerData.overrideTempNow) {
    controllerData.reasonForSetpoint = spOverrideNow;
    controllerData.temperatureSetpoint=controllerData.overrideSetpoint;
  } else {
    controllerData.overrideSetpoint = int(controllerData.temperatureSetpoint+0.5);
  }

  for(int i=0; i<7; i++) {
    daytype=controllerData.regularWeek[dayOfWeek];  // 0=Sunday
    controllerData.dayTypes[i]=daytype;
    controllerData.dayTypeOverruled[i]=false;
    dayOfWeek++;
    if(dayOfWeek>6) dayOfWeek=0;
  };

  // Override multiple days if needed
  if(controllerData.overrideMultiple!=dtAuto) {
    if( controllerData.multipleForever ) {
      for(int i=0; i < 7; i++) {
        controllerData.dayTypes        [i] = controllerData.overrideMultiple;
        controllerData.dayTypeOverruled[i] = true;
      };  
    }
    else {
      for(int i=0; i < controllerData.overrideMultipleCount; i++) {
        if(i<7) {
          controllerData.dayTypes        [i] = controllerData.overrideMultiple;
          controllerData.dayTypeOverruled[i] = true;
        }
      };  
    }
  };

  // Override today if needed
  if( controllerData.overrideToday != dtAuto) {
    controllerData.dayTypes        [0] = controllerData.overrideToday;
    controllerData.dayTypeOverruled[0] = true;
  }

  // Override tomorrow if needed
  if(controllerData.overrideTomorrow!=dtAuto) {
    controllerData.dayTypes        [1] = controllerData.overrideTomorrow;
    controllerData.dayTypeOverruled[1] = true;
  }

  // Log the boiler actions at the whole hour
  if( (currTime.minutesSinceMidnight % 60 == 0) and (prevTime.minutesSinceMidnight % 60 == 59) ) {

    String reason;
    
    switch(controllerData.reasonForSetpoint) {
  
      case spWeekSchedule:
        reason = String("WeekSch>") + String( DAY_TYPES[controllerData.regularWeek[dayOfWeek]].c_str() );
      break;
  
      case spMultipleDays:
        reason = String("MultDays>") + String(DAY_TYPES[controllerData.overrideMultiple].c_str() );
      break;
  
      case spOverrideToday:
        reason = String("OverruleToday>") + String( DAY_TYPES[controllerData.overrideToday].c_str() );
      break;
  
      case spOverrideNow:
        reason = String("OverruleNow");
      break;
      
    } // switch(controllerData.reasonForSetpoint)

    controllerData.datalogger.header( String("Time Troom BoilEnabl Tboil,req Tboil,act tboil Reason\n") +
                                              "hh:mm °C Y/N °C °C hh:mm");
    controllerData.datalogger.log( currTime.str()                                               + " " + 
                                    String(controllerData.measuredTemperature()             ,1) + " " + 
                                    String(controllerData.enableCentralHeating ? "Y" : "N"    ) + " " + 
                                    String(controllerData.requestedBoilerTemperature        ,1) + " " + 
                                    String(controllerData.actualBoilerTemperature           ,1) + " " +
                                    controllerData.lastTemperatureMeasurement.str()             + " " +
                                    reason
                                  );    
  }
  
  // Remember for the next calculation
  prevDay=currDay;
  prevTime=currTime;
};

void processControllerCommand(userEventMessage_t messageToController) {

  switch(messageToController.command) {
    case cmdControl                      : break; // Don't call control, it is called for every function after the switch statement
    case cmdUpdateScreen                 : break; 

    case cmdBoilerSending:
      sendMessage(sndController, cmdBoilerSending, displayQueue); // Update screen 
      break;

    case cmdSilentUpdate:
      sendMessage(sndController, cmdSilentUpdate, displayQueue);  // Update screen 
      break;

    case cmdSetTemperatureOffset: 
      controllerData.sensorOffset=messageToController.temperatureOffset; 
      controllerData.settingsChanged = true;
      break;

    case cmdSensorOffsetUp: 
      controllerData.sensorOffset+=0.1; 
      controllerData.settingsChanged = true;
      break;

    case cmdSensorOffsetDown: 
      controllerData.sensorOffset-=0.1; 
      controllerData.settingsChanged = true;
      break;
    
    case cmdSetpointLower: 
      if(!controllerData.overrideTempNow) {
        controllerData.overrideSetpoint = controllerData.highTemp;
        if ( abs( controllerData.overrideSetpoint - controllerData.temperatureSetpoint) < 0.4 ) controllerData.overrideSetpoint-=0.5;
        controllerData.overrideTempNow=true;
      }
      else {
        if(controllerData.overrideSetpoint>0) controllerData.overrideSetpoint-=0.5;
      }
      controllerData.settingsChanged = true; 

      break;

    case cmdSetpointHigher:
      if(!controllerData.overrideTempNow) {
        controllerData.overrideSetpoint= controllerData.highTemp;
        if ( abs( controllerData.overrideSetpoint - controllerData.temperatureSetpoint) < 0.4 ) controllerData.overrideSetpoint +=0.5;
        controllerData.overrideTempNow=true;
      }
      else {
        if(controllerData.overrideSetpoint<30) controllerData.overrideSetpoint+=0.5;
      }
      controllerData.settingsChanged = true; 

      break;
      
    case cmdSetpointAutomatic:
      controllerData.overrideTempNow = false;
      controllerData.settingsChanged = true; 

      break;
    
    case cmdComeHome: 
      controllerData.overrideSetpoint = controllerData.highTemp;
      controllerData.overrideTempNow = true;
      break;
    
    case cmdOverruleTodayWorkFromHome:
      controllerData.overrideToday = dtWorkFromHome; 
      controllerData.overrideTempNow = false; 
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleTodayWorkAtOffice: 
      controllerData.overrideToday = dtWorkAtOffice; 
      controllerData.overrideTempNow = false; 
      controllerData.settingsChanged = true;      
      break;

    case cmdOverruleTodayWeekend: 
      controllerData.overrideToday = dtWeekend;      
      controllerData.overrideTempNow = false; 
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleTodayAway: 
      controllerData.overrideToday = dtAway;         
      controllerData.overrideTempNow = false; 
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleTodayAutomatic: 
      controllerData.overrideToday = dtAuto;         
      controllerData.overrideTempNow = false; 
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleTomorrowWorkFromHome: 
      controllerData.overrideTomorrow = dtWorkFromHome; 
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleTomorrowWorkAtOffice : 
      controllerData.overrideTomorrow = dtWorkAtOffice; 
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleTomorrowWeekend: 
      controllerData.overrideTomorrow=dtWeekend;      
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleTomorrowAway: 
      controllerData.overrideTomorrow=dtAway;         
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleTomorrowAutomatic: 
      controllerData.overrideTomorrow = dtAuto;         
      controllerData.settingsChanged = true; 
      break;
    
    case cmdOverruleMultipleWorkFromHome: 
      controllerData.overrideMultiple=dtWorkFromHome;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideTempNow=false;

      //Switch off the higher priority settings
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleMultipleWorkAtOffice:
      controllerData.overrideMultiple=dtWorkAtOffice;

      //Switch off the higher priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTempNow=false;
      
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleMultipleWeekend: 
      controllerData.overrideMultiple=dtWeekend;

      //Switch off the higher priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTempNow=false;
      
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleMultipleAway:
      controllerData.overrideMultiple=dtAway;

      //Switch off the higher priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTempNow=false;
      
      controllerData.settingsChanged = true; 
      break;

    case cmdOverruleMultipleAutomatic: 
      controllerData.multipleForever=false; 
      controllerData.overrideMultiple=dtAuto; 
      controllerData.overrideTempNow=false; 
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleMultipleFewerDays: 
      controllerData.multipleForever=false; 
      if(controllerData.overrideMultipleCount > 1) controllerData.overrideMultipleCount--; 

      //Switch off the higher priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.settingsChanged = true;
      break;

    case cmdOverruleMultipleMoreDays: 
      controllerData.multipleForever=false; 
      controllerData.overrideMultipleCount++; 

      //Switch off the higher priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTempNow=false;

      controllerData.settingsChanged = true;
      break;

    case cmdOverruleMultipleForever: 
      controllerData.multipleForever=true;

      // Reset the multiple day counter if needed
      if( controllerData.overrideMultipleCount < 2) controllerData.overrideMultipleCount = 3;

      //Switch off the lower priority settings
      controllerData.overrideTomorrow=dtAuto;
      controllerData.overrideToday=dtAuto;
      controllerData.overrideTempNow=false;

      controllerData.settingsChanged = true; 
      break;

    case cmdSetWeekSchedule: 
      controllerData.regularWeek[messageToController.dayOfWeek]=messageToController.typeOfDay; 
      controllerData.settingsChanged = true;
      break;

    case cmdHomeWakeUpEarlier: 
      if( controllerData.workFromHomeWakeUp >= timeValue_t( 0,15) ) { 
        controllerData.workFromHomeWakeUp-=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      }
      break;

    case cmdHomeWakeUpLater:
      if( controllerData.workFromHomeWakeUp <= timeValue_t(23,30) ) { 
        controllerData.workFromHomeWakeUp+=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdHomeGoToSleepEarlier: 
      if( controllerData.workFromHomeSleep  >= timeValue_t( 0,15) ) { 
        controllerData.workFromHomeSleep -=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      }; 
      break;

    case cmdHomeGoToSleepLater: 
      if( controllerData.workFromHomeSleep  <= timeValue_t(23,30) ) { 
        controllerData.workFromHomeSleep +=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeWakeUpEarlier: 
      if( controllerData.workAtOfficeWakeUp >= timeValue_t( 0,15) ) { 
        controllerData.workAtOfficeWakeUp-=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeWakeUpLater: 
      if( controllerData.workAtOfficeWakeUp <= timeValue_t(23,30) ) { 
        controllerData.workAtOfficeWakeUp+=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      }
      break;

    case cmdOfficeLeaveEarlier: 
      if( controllerData.workAtOfficeGoOut  >= timeValue_t( 0,15) ) { 
        controllerData.workAtOfficeGoOut -=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeLeaveLater: 
      if( controllerData.workAtOfficeGoOut  <= timeValue_t(23,30) ) { 
        controllerData.workAtOfficeGoOut +=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeComeHomeEarlier: 
      if( controllerData.workAtOfficeComeIn >= timeValue_t( 0,15) ) { 
        controllerData.workAtOfficeComeIn-=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeComeHomeLater: 
      if( controllerData.workAtOfficeComeIn <= timeValue_t(23,30) ) { 
        controllerData.workAtOfficeComeIn+=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeGoToSleepEarlier: 
      if( controllerData.workAtOfficeSleep  >= timeValue_t( 0,15) ) { 
        controllerData.workAtOfficeSleep -=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdOfficeGoToSleepLater: 
      if( controllerData.workAtOfficeSleep  <= timeValue_t(23,30) ) { 
        controllerData.workAtOfficeSleep +=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdWeekendWakeUpEarlier:
      if( controllerData.weekendWakeUp >= timeValue_t( 0,15) ) { 
        controllerData.weekendWakeUp-=timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };  
      break;

    case cmdWeekendWakeUpLater:
      if( controllerData.weekendWakeUp <= timeValue_t(23,30) ) { 
        controllerData.weekendWakeUp += timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdWeekendGoToSleepEarlier: 
      if( controllerData.weekendSleep >= timeValue_t( 0,15) ) { 
        controllerData.weekendSleep -= timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      };
      break;

    case cmdWeekendGoToSleepLater: 
      if( controllerData.weekendSleep <= timeValue_t(23,15) ) { 
        controllerData.weekendSleep += timeValue_t(0,15); 
        controllerData.settingsChanged = true; 
      }; 
      break;

    case cmdHighTemperatureDown: 
      if( controllerData.highTemp > 4.0)  { 
        controllerData.highTemp-=0.5; 
        if( controllerData.lowTemp + 0.5 > controllerData.highTemp)  { 
          controllerData.lowTemp = controllerData.highTemp - 0.5;
        };      
        controllerData.settingsChanged = true;
      };
      break;

    case cmdHighTemperatureUp: 
      if( controllerData.highTemp < 30.0)  { 
        controllerData.highTemp += 0.5; 
        controllerData.settingsChanged = true;
      };
      break;
       
    case cmdLowTemperatureDown: 
      if( controllerData.lowTemp  > 4.0)  { 
        controllerData.lowTemp -= 0.5; 
        controllerData.settingsChanged = true;
      };
      break;

    case cmdLowTemperatureUp:
      if( controllerData.lowTemp < 30.0)  { 
        controllerData.lowTemp += 0.5; 
        if( controllerData.lowTemp + 0.5 > controllerData.highTemp)  { 
          controllerData.highTemp = controllerData.lowTemp + 0.5;
        };      
        controllerData.settingsChanged = true;
      };
      break;

  } // switch command

  if( controllerData.settingsChanged or (messageToController.command == cmdUpdateScreen) ) {
    sendMessage(sndController, cmdUpdateScreen, displayQueue);  
  }  

  // Always calculate new setpoint for boiler
  calculateNewSetpoint(); 
};


/******************************************
 * GLOBAL VARIABLES                       * 
 ******************************************/

void processControllerIfNeeded() {
  userEventMessage_t message;

  if( xQueueReceive( controllerQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Controller [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed );

    // Give controller exclusive access to controllerData and execute the command
    processControllerCommand(message);
    
    // Send any request made by Telegram back to Telegram to send a response to the user
    if( message.sender == sndTelegram ) {
      message.sender = sndController;
      //Serial.printf("Controller sending command back to Telegram with chatID: %s\n", message.chatID);
      xQueueSend( telegramQueue, &message, ( TickType_t ) 10 );  
    }
            
  }; // if( xQueueReceive( controllerQueue..
}


void startController() {
  sendMessage(sndMenu, cmdUpdateScreen, controllerQueue);
  processControllerIfNeeded();
};
