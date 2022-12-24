#pragma once

/* =================================================
 *  Controller program class definition
 * ================================================= */

class Controller {
  public:
    Controller(ControllerData_t & controllerData) { this->controllerData = & controllerData; };
    void processCommand(userEventMessage_t messageToController);

  protected:
    ControllerData_t  *controllerData;

    // Various handlers for the user interface, both device and Telegram
    void control();
    void setTemperatureOffset(float offset) { controllerData->sensorOffset=offset; controllerData->settingsChanged = true; };
    void increaseSensorOffset() { controllerData->sensorOffset+=0.1; controllerData->settingsChanged = true; };
    void decreaseSensorOffset() { controllerData->sensorOffset-=0.1; controllerData->settingsChanged = true; };
    void setpointLower();
    void setpointHigh()         { controllerData->overrideTempNow=true; controllerData->overrideSetpoint = controllerData->highTemp; controllerData->settingsChanged = true; };
    void setpointHigher();
    void goAway();
    
    void overruleTodayWorkFromHome()      { controllerData->overrideToday=dtWorkFromHome; controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    void overruleTodayWorkFromOffice()    { controllerData->overrideToday=dtWorkAtOffice; controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    void overruleTodayWeekend()           { controllerData->overrideToday=dtWeekend;      controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    void overruleTodayAway()              { controllerData->overrideToday=dtAway;         controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    void overruleTodayAutomatic()         { controllerData->overrideToday=dtAuto;         controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    
    void overruleTomorrowWorkFromHome()   { controllerData->overrideTomorrow=dtWorkFromHome; controllerData->settingsChanged = true; };
    void overruleTomorrowWorkFromOffice() { controllerData->overrideTomorrow=dtWorkAtOffice; controllerData->settingsChanged = true; };
    void overruleTomorrowWeekend()        { controllerData->overrideTomorrow=dtWeekend;      controllerData->settingsChanged = true; };
    void overruleTomorrowAway()           { controllerData->overrideTomorrow=dtAway;         controllerData->settingsChanged = true; };
    void overruleTomorrowAutomatic()      { controllerData->overrideTomorrow=dtAuto;         controllerData->settingsChanged = true; };

    void overruleMultipleWorkFromHome();
    void overruleMultipleWorkFromOffice();
    void overruleMultipleWeekend();
    void overruleMultipleAway();

    void overruleMultipleAutomatic()   { controllerData->overrideMultiple=dtAuto; controllerData->overrideTempNow=false; controllerData->settingsChanged = true; };
    void overruleMultipleLessDays()    { if(controllerData->overrideMultipleCount > 1) controllerData->overrideMultipleCount--; controllerData->settingsChanged = true; };
    void overruleMultipleMoreDays()    { controllerData->overrideMultipleCount++; controllerData->settingsChanged = true; };
    void setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay) {  controllerData->regularWeek[dayOfWeek]=typeOfDay; controllerData->settingsChanged = true; };
    
    void workFromHomeWakeUpEarlier()   { if( controllerData->workFromHomeWakeUp >= timeValue_t( 0,15) ) { controllerData->workFromHomeWakeUp-=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromHomeWakeUpLater()     { if( controllerData->workFromHomeWakeUp <= timeValue_t(23,15) ) { controllerData->workFromHomeWakeUp+=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromHomeSleepEarlier()    { if( controllerData->workFromHomeSleep  >= timeValue_t( 0,15) ) { controllerData->workFromHomeSleep -=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromHomeSleepLater()      { if( controllerData->workFromHomeSleep  <= timeValue_t(23,15) ) { controllerData->workFromHomeSleep +=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeWakeUpEarlier() { if( controllerData->workAtOfficeWakeUp >= timeValue_t( 0,15) ) { controllerData->workAtOfficeWakeUp-=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeWakeUpLater()   { if( controllerData->workAtOfficeWakeUp <= timeValue_t(23,15) ) { controllerData->workAtOfficeWakeUp+=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeGoOutEarlier()  { if( controllerData->workAtOfficeGoOut  >= timeValue_t( 0,15) ) { controllerData->workAtOfficeGoOut -=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeGoOutLater()    { if( controllerData->workAtOfficeGoOut  <= timeValue_t(23,15) ) { controllerData->workAtOfficeGoOut +=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeComeInEarlier() { if( controllerData->workAtOfficeComeIn >= timeValue_t( 0,15) ) { controllerData->workAtOfficeComeIn-=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeComeInLater()   { if( controllerData->workAtOfficeComeIn <= timeValue_t(23,15) ) { controllerData->workAtOfficeComeIn+=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeSleepEarlier()  { if( controllerData->workAtOfficeSleep  >= timeValue_t( 0,15) ) { controllerData->workAtOfficeSleep -=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void workFromOfficeSleepLater()    { if( controllerData->workAtOfficeSleep  <= timeValue_t(23,15) ) { controllerData->workAtOfficeSleep +=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void weekendWakeUpEarlier()        { if( controllerData->weekendWakeUp      >= timeValue_t( 0,15) ) { controllerData->weekendWakeUp     -=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void weekendWakeUpLater()          { if( controllerData->weekendWakeUp      <= timeValue_t(23,15) ) { controllerData->weekendWakeUp     +=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void weekendSleepEarlier()         { if( controllerData->weekendSleep       >= timeValue_t( 0,15) ) { controllerData->weekendSleep      -=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    void weekendSleepLater()           { if( controllerData->weekendSleep       <= timeValue_t(23,15) ) { controllerData->weekendSleep      +=timeValue_t(0,15); controllerData->settingsChanged = true; } };
    
    void highTempLower()               { if( controllerData->highTemp > controllerData->lowTemp+0.5   ) { controllerData->highTemp-=0.5; controllerData->settingsChanged = true; }};
    void highTempHigher()              { if( controllerData->highTemp < 30)                             { controllerData->highTemp+=0.5; controllerData->settingsChanged = true; }};
    void lowTempLower()                { if( controllerData->lowTemp  > 4.0)                            { controllerData->lowTemp-= 0.5; controllerData->settingsChanged = true; }};
    void lowTempHigher()               { if( controllerData->lowTemp  < controllerData->highTemp-0.5 )  { controllerData->lowTemp+= 0.5; controllerData->settingsChanged = true; }};
};

void Controller::processCommand(userEventMessage_t messageToController) {

  // ToDo: consider always doing control() after switch statement, and remove control() from all functions
  
  switch(messageToController.command) {
    case cmdControl                      : break; // Don't call control, it is called for every function after the switch statement
    case cmdBoilerSending                : break;
    case cmdUpdateScreen                 : break; 
    case cmdSilentUpdate                 : break; 
    case cmdSetTemperatureOffset         : setTemperatureOffset(messageToController.temperatureOffset); break;
    case cmdSensorOffsetUp               : increaseSensorOffset(); break;
    case cmdSensorOffsetDown             : decreaseSensorOffset(); break;
    case cmdSetpointLower                : setpointLower(); break;
    case cmdComeHome                     : setpointHigh(); break;
    case cmdSetpointHigher               : setpointHigher(); break;
    case cmdOverruleTodayWorkFromHome    : overruleTodayWorkFromHome(); break;
    case cmdOverruleTodayWorkAtOffice    : overruleTodayWorkFromOffice(); break;
    case cmdOverruleTodayWeekend         : overruleTodayWeekend(); break;
    case cmdOverruleTodayAway            : overruleTodayAway(); break;
    case cmdOverruleTodayAutomatic       : overruleTodayAutomatic(); break;
    case cmdOverruleTomorrowWorkFromHome : overruleTomorrowWorkFromHome(); break;
    case cmdOverruleTomorrowWorkAtOffice : overruleTomorrowWorkFromOffice(); break;
    case cmdOverruleTomorrowWeekend      : overruleTomorrowWeekend(); break;
    case cmdOverruleTomorrowAway         : overruleTomorrowAway(); break;
    case cmdOverruleTomorrowAutomatic    : overruleTomorrowAutomatic(); break;
    case cmdOverruleMultipleWorkFromHome : overruleMultipleWorkFromHome(); break;
    case cmdOverruleMultipleWorkAtOffice : overruleMultipleWorkFromOffice(); break;
    case cmdOverruleMultipleWeekend      : overruleMultipleWeekend(); break;
    case cmdOverruleMultipleAway         : overruleMultipleAway(); break;
    case cmdOverruleMultipleAutomatic    : overruleMultipleAutomatic(); break;
    case cmdOverruleMultipleFewerDays    : overruleMultipleLessDays(); break;
    case cmdOverruleMultipleMoreDays     : overruleMultipleMoreDays(); break;
    case cmdSetWeekSchedule              : setWeekSchedule(messageToController.dayOfWeek, messageToController.typeOfDay); break;
    case cmdHomeWakeUpEarlier            : workFromHomeWakeUpEarlier(); break;
    case cmdHomeWakeUpLater              : workFromHomeWakeUpLater(); break;
    case cmdHomeGoToSleepEarlier         : workFromHomeSleepEarlier(); break;
    case cmdHomeGoToSleepLater           : workFromHomeSleepLater(); break;
    case cmdOfficeWakeUpEarlier          : workFromOfficeWakeUpEarlier(); break;
    case cmdOfficeWakeUpLater            : workFromOfficeWakeUpLater(); break;
    case cmdOfficeLeaveEarlier           : workFromOfficeGoOutEarlier(); break;
    case cmdOfficeLeaveLater             : workFromOfficeGoOutLater(); break;
    case cmdOfficeComeHomeEarlier        : workFromOfficeComeInEarlier(); break;
    case cmdOfficeComeHomeLater          : workFromOfficeComeInLater(); break;
    case cmdOfficeGoToSleepEarlier       : workFromOfficeSleepEarlier(); break;
    case cmdOfficeGoToSleepLater         : workFromOfficeSleepLater(); break;
    case cmdWeekendWakeUpEarlier         : weekendWakeUpEarlier(); break;
    case cmdWeekendWakeUpLater           : weekendWakeUpLater(); break;
    case cmdWeekendGoToSleepEarlier      : weekendSleepEarlier(); break;
    case cmdWeekendGoToSleepLater        : weekendSleepLater(); break;
    case cmdHighTemperatureDown          : highTempLower(); break;
    case cmdHighTemperatureUp            : highTempHigher(); break;
    case cmdLowTemperatureDown           : lowTempLower(); break;
    case cmdLowTemperatureUp             : lowTempHigher(); break;
  }

  control(); 
};

void Controller::control() {
  static int prevDay=0;
  timeValue_t currTime;
  static timeValue_t prevTime = timeValue_t(0,0);
  dayType_t currDayType;
  dayType_t daytype;
  
  Serial.println("Controller::control()");

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
    controllerData->overrideToday = controllerData->overrideTomorrow;
    controllerData->overrideTomorrow=dtAuto;
    
    if (controllerData->overrideMultiple!=dtAuto) {
      if(controllerData->overrideMultipleCount>0)
        controllerData->overrideMultipleCount--;
      else
        controllerData->overrideMultiple=dtAuto;
    }
  }

  // Determine what kind of day we have today
  currDayType=controllerData->regularWeek[dayOfWeek];
  controllerData->reasonForSetpoint = spWeekSchedule;

  if ( (controllerData->overrideMultiple!=dtAuto) and (controllerData->overrideMultipleCount>0) ) {
    currDayType=controllerData->overrideMultiple;
    controllerData->reasonForSetpoint = spMultipleDays;
  }

  if(controllerData->overrideToday!=dtAuto) {
    currDayType=controllerData->overrideToday;
    controllerData->reasonForSetpoint = spOverrideToday;
  }

  // Check if we just passed a switch moment
  switch(currDayType) {

    case dtWorkFromHome:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < controllerData->workFromHomeWakeUp) and (controllerData->workFromHomeWakeUp <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromHomeWakeUp"); }
      else if( (prevTime < controllerData->workFromHomeSleep ) and (controllerData->workFromHomeSleep  <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromHomeSleep" ); };
      
      if(!controllerData->overrideTempNow) {
        controllerData->temperatureSetpoint = controllerData->lowTemp;
        if( (controllerData->workFromHomeWakeUp <= currTime) and (currTime < controllerData->workFromHomeSleep) ) controllerData->temperatureSetpoint = controllerData->highTemp;
      }
    break;

    case dtWorkAtOffice:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < controllerData->workAtOfficeWakeUp) and (controllerData->workAtOfficeWakeUp <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromOfficeWakeUp"); }
      else if( (prevTime < controllerData->workAtOfficeGoOut ) and (controllerData->workAtOfficeGoOut  <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromOfficeGoOut" ); }
      else if( (prevTime < controllerData->workAtOfficeComeIn) and (controllerData->workAtOfficeComeIn <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromOfficeComeIn"); }
      else if( (prevTime < controllerData->workAtOfficeSleep ) and (controllerData->workAtOfficeSleep  <= currTime) ) { controllerData->overrideTempNow=false; logMessage("WorkFromOfficeSleep" ); };
      
      if(!controllerData->overrideTempNow) {
        controllerData->temperatureSetpoint = controllerData->lowTemp;
        if     ( (controllerData->workAtOfficeWakeUp<=currTime) and (currTime<controllerData->workAtOfficeGoOut) ) controllerData->temperatureSetpoint = controllerData->highTemp;
        else if( (controllerData->workAtOfficeComeIn<=currTime) and (currTime<controllerData->workAtOfficeSleep) ) controllerData->temperatureSetpoint = controllerData->highTemp;
      }      
    break;

    case dtAway:
      if(currDay != prevDay) { controllerData->overrideTempNow=false; logMessage("Day away next day" ); };

      if(!controllerData->overrideTempNow) controllerData->temperatureSetpoint = controllerData->lowTemp;
    break;

    case dtWeekend:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<controllerData->weekendWakeUp) and (controllerData->weekendWakeUp<=currTime) ) { controllerData->overrideTempNow=false; logMessage("WeekendWakeUp"); }
      else if( (prevTime<controllerData->weekendSleep ) and (controllerData->weekendSleep <=currTime) ) { controllerData->overrideTempNow=false; logMessage("WeekendSleep" ); };
      
      if(!controllerData->overrideTempNow) {
        controllerData->temperatureSetpoint = controllerData->lowTemp;
        if( (controllerData->weekendWakeUp<=currTime) and (currTime<controllerData->weekendSleep) ) controllerData->temperatureSetpoint = controllerData->highTemp;
      }
    break;
  }

  if(controllerData->overrideTempNow) {
    controllerData->reasonForSetpoint = spOverrideNow;
    controllerData->temperatureSetpoint=controllerData->overrideSetpoint;
  } else {
    controllerData->overrideSetpoint = int(controllerData->temperatureSetpoint+0.5);
  }

  for(int i=0; i<7; i++) {
    daytype=controllerData->regularWeek[dayOfWeek];  // 0=Sunday
    controllerData->dayTypes[i]=daytype;
    controllerData->dayTypeOverruled[i]=false;
    dayOfWeek++;
    if(dayOfWeek>6) dayOfWeek=0;
  };

  // Override multiple days if needed
  if(controllerData->overrideMultiple!=dtAuto) {
    for(int i=0; i < controllerData->overrideMultipleCount; i++) {
      if(i<7) {
        controllerData->dayTypes        [i] = controllerData->overrideMultiple;
        controllerData->dayTypeOverruled[i] = true;
      }
    };  
  };

  // Override today if needed
  if( controllerData->overrideToday != dtAuto) {
    controllerData->dayTypes        [0] = controllerData->overrideToday;
    controllerData->dayTypeOverruled[0] = true;
  }

  // Override tomorrow if needed
  if(controllerData->overrideTomorrow!=dtAuto) {
    controllerData->dayTypes        [1] = controllerData->overrideTomorrow;
    controllerData->dayTypeOverruled[1] = true;
  }

  // Log the boiler actions at the whole hour
  if( (currTime.minutesSinceMidnight % 60 == 0) and (prevTime.minutesSinceMidnight % 60 == 59) ) {

    String reason;
    
    switch(controllerData->reasonForSetpoint) {
  
      case spWeekSchedule:
        reason = String("WeekSch>") + String( DAY_TYPES[controllerData->regularWeek[dayOfWeek]].c_str() );
      break;
  
      case spMultipleDays:
        reason = String("MultDays>") + String(DAY_TYPES[controllerData->overrideMultiple].c_str() );
      break;
  
      case spOverrideToday:
        reason = String("OverruleToday>") + String( DAY_TYPES[controllerData->overrideToday].c_str() );
      break;
  
      case spOverrideNow:
        reason = String("OverruleNow");
      break;
      
    } // switch(controllerData->reasonForSetpoint)

    controllerData->datalogger.header( String("Time Troom BoilEnabl Tboil,req Tboil,act tboil Reason\n") +
                                              "hh:mm °C Y/N °C °C hh:mm");
    controllerData->datalogger.log( currTime.str()                                               + " " + 
                                    String(controllerData->measuredTemperature()             ,1) + " " + 
                                    String(controllerData->enableCentralHeating ? "Y" : "N"    ) + " " + 
                                    String(controllerData->requestedBoilerTemperature        ,1) + " " + 
                                    String(controllerData->actualBoilerTemperature           ,1) + " " +
                                    controllerData->lastTemperatureMeasurement.str()             + " " +
                                    reason
                                  );    
  }
  
  // Remember for the next calculation
  prevDay=currDay;
  prevTime=currTime;
};

void Controller::setpointLower() { 
  Serial.println("Controller::setpointLower()");
  
  if(!controllerData->overrideTempNow) {
    controllerData->overrideSetpoint = 0.5*round(2*controllerData->measuredTemperature() )-0.5;
  }
  else {
    if(controllerData->overrideSetpoint>0) controllerData->overrideSetpoint-=0.5;
  }
  controllerData->overrideTempNow=true;
  controllerData->settingsChanged = true; 
};

void Controller::setpointHigher() {
  if(!controllerData->overrideTempNow) {
    controllerData->overrideSetpoint= 0.5*round(2*controllerData->measuredTemperature() )+0.5;
  }
  else {
    if(controllerData->overrideSetpoint<30) controllerData->overrideSetpoint+=0.5;
  }
  controllerData->overrideTempNow=true;
  controllerData->settingsChanged = true; 
};

void Controller::goAway() { 
  controllerData->overrideToday=dtAway;
  controllerData->overrideTempNow=false;
  controllerData->settingsChanged = true; 
};


void Controller::overruleMultipleWorkFromHome() { 
  Serial.println("Controller::overruleMultipleWorkFromHome()");
  controllerData->overrideMultiple=dtWorkFromHome;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideTempNow=false;
  controllerData->settingsChanged = true; 
};

void Controller::overruleMultipleWorkFromOffice() { 
  Serial.println("Controller::overruleMultipleWorkFromOffice()");
  controllerData->overrideMultiple=dtWorkAtOffice;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  controllerData->settingsChanged = true; 
};

void Controller::overruleMultipleWeekend() { 
  Serial.println("Controller::overruleMultipleWeekend()");
  controllerData->overrideMultiple=dtWeekend;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  controllerData->settingsChanged = true; 
};

void Controller::overruleMultipleAway() { 
  Serial.println("Controller::overruleMultipleAway()");
  controllerData->overrideMultiple=dtAway;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  controllerData->settingsChanged = true; 
};


/******************************************
 * GLOBAL VARIABLES                       * 
 ******************************************/

Controller controller=Controller(controllerData); // Controller

void processControllerIfNeeded() {
  userEventMessage_t message;

  if( xQueueReceive( controllerQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Controller [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed );

    // Give controller exclusive access to controllerData and execute the command
    controller.processCommand(message);
    
    // Send any request made by Telegram back to Telegram to respond to the user
    if( message.sender == sndTelegram ) {
      message.sender = sndController;
      Serial.printf("Controller sending command back to Telegram with chatID: %s\n", message.chatID);
      xQueueSend( telegramQueue, &message, ( TickType_t ) 10 );  
    }

    message.sender = sndController;
    switch( message.command ) {

      case cmdBoilerSending:
        Serial.println("Controller requests boilersending icon");
        sendMessage(sndController, cmdBoilerSending, displayQueue); // Update screen 
        break;

      case cmdSilentUpdate:
        Serial.println("Controller requests silent screen update");
        sendMessage(sndController, cmdSilentUpdate, displayQueue);  // Update screen 
        break;

      default:
        Serial.printf("Controller requests screen update and backlight due to [%s] command", commandLabels[message.command].c_str());
        sendMessage(sndController, cmdUpdateScreen, displayQueue);  // Update screen 
            
    }; // switch( message.command
    
  }; // if( xQueueReceive( controllerQueue..
}


void startController() {
  sendMessage(sndMenu, cmdUpdateScreen, controllerQueue);
  processControllerIfNeeded();
};
