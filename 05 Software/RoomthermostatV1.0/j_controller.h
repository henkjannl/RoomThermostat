#pragma once

#include <OpenTherm.h>

#define CONTROLLER_INTERVAL 5000

/* ======================================
   PID CONTROLLER CLASS DEFINITION
   ====================================== */
class PID {
  public:
    float KP = 10;
    float KI = 0.02;  
    float KD = 0;

    // Upper and lower bounds on requested boiler temperature
    float outputHigh = 80;
    float outputLow  = 10;
      
    float setpoint=4.0;
    
    unsigned long currentTime;
    unsigned long previousTime;
    
    float currentTemperature;
    float previousTemperature;

    float boilerTemperature;
    float boilerPercentage;

    float timeStep;

    float ierr = 0;
    float previousError = 0;
    float derivative = 0;

    float P;
    float I;
    float D;
    
    float newValue(float newTemperature);
};

float PID::newValue(float newTemperature) {

  // Update temperature
  previousTemperature = currentTemperature;
  currentTemperature  = newTemperature;

  // Store time in native format to ensure right outcome on millis() overflow 
  previousTime=currentTime;
  currentTime=millis();
  timeStep=(currentTime-previousTime)/1000; // converted to seconds here
    
  // Calculate the control error
  float error = setpoint - currentTemperature;
  
  // Calculate the integral error
  ierr += error * timeStep;  
  
  // !! D-action probably not implemented well yet !!
  // perhaps we need to filter the signal to prevent noise in the D-action
  // Calculate the measurement derivative
  derivative = (timeStep!=0) ? (error - previousError) / timeStep : 0;
  
  // Calculate the PID output
  P = KP * error;        // proportional contribution
  I = KI * ierr;         // integral contribution  
  D = KD * derivative;   // differential contribution
  boilerTemperature = P+I+D;
  
  // Implement anti-windup
  // !! Should the (error<0) clause be there? !! What does it do?
  if ((boilerTemperature < outputLow)  and (error<0)) ierr -= error * timeStep;    
  if ((boilerTemperature > outputHigh) and (error>0)) ierr -= error * timeStep;    
  
  // Clip output
  boilerTemperature = max(outputLow, min(outputHigh, boilerTemperature));
  boilerPercentage=(boilerTemperature-outputLow)/(outputHigh-outputLow);
  
  //Serial.printf("setpoint= %.3f°C measured=%.3f°C P=%.3f I=%.3f D=%.3f boiler=%.3f°C\n",setpoint, currentTemperature, P, I, D, boilerTemperature);

  previousError = error;

  return boilerTemperature;
};

/* =================================================
 *  Controller program class definition
 * ================================================= */

class Controller {
  public:
    Controller(ControllerData_t & controllerData) { this->controllerData = & controllerData; };
    void processCommand(userEventMessage_t messageToController);

  protected:
    ControllerData_t  *controllerData;
    PID pidController;    

    // Various handlers for the user interface, both device and Telegram
    void control();
    void setTemperatureOffset(float offset);
    void increaseSensorOffset();
    void decreaseSensorOffset();
    void setpointLower();
    void setpointAuto();
    void setpointHigher();
    void goAway();
    void overruleTodayWorkFromHome();
    void overruleTodayWorkFromOffice();
    void overruleTodayWeekend();
    void overruleTodayAway();
    void overruleTodayAutomatic();
    void overruleTomorrowWorkFromHome();
    void overruleTomorrowWorkFromOffice();
    void overruleTomorrowWeekend();
    void overruleTomorrowAway();
    void overruleTomorrowAutomatic();
    void overruleMultipleWorkFromHome();
    void overruleMultipleWorkFromOffice();
    void overruleMultipleWeekend();
    void overruleMultipleAway();
    void overruleMultipleAutomatic();
    void overruleMultipleLessDays();
    void overruleMultipleMoreDays();
    void setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay);
    void workFromHomeWakeUpEarlier();
    void workFromHomeWakeUpLater();
    void workFromHomeSleepEarlier();
    void workFromHomeSleepLater();
    void workFromOfficeWakeUpEarlier();
    void workFromOfficeWakeUpLater();
    void workFromOfficeGoOutEarlier();
    void workFromOfficeGoOutLater();
    void workFromOfficeComeInEarlier();
    void workFromOfficeComeInLater();
    void workFromOfficeSleepEarlier();
    void workFromOfficeSleepLater();
    void weekendWakeUpEarlier();
    void weekendWakeUpLater();
    void weekendSleepEarlier();
    void weekendSleepLater();
    void highTempLower();
    void highTempHigher();
    void lowTempLower();
    void lowTempHigher();
};

void Controller::processCommand(userEventMessage_t messageToController) {
  
  switch(messageToController.command) {
    case cmdControl                      : control(); break;
    case cmdUpdateScreen                 : control(); break; // First update the control data before rewriting the screen
    case cmdSetTemperatureOffset         : setTemperatureOffset(messageToController.temperatureOffset); break;
    case cmdSensorOffsetUp               : increaseSensorOffset(); break;
    case cmdSensorOffsetDown             : decreaseSensorOffset(); break;
    case cmdSetpointLower                : setpointLower(); break;
    case cmdSetpointAuto                 : setpointAuto(); break;
    case cmdSetpointHigher               : setpointHigher(); break;
    case cmdGoAway                       : goAway(); break;
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
};

void Controller::control() {
  static int prevDay=0;
  static timeValue_t prevTime = timeValue_t(0,0);
  dayType_t currDayType;
  dayType_t daytype;
  
  Serial.println("Controller::control()");
  //controllerData->logBusyTime.start(btController);  

  // Measure temperature
  //temperatureSensor->measure(); // Should happen automatically 

  // Current day and time
  time_t now;
  time(&now); // Get current time

  struct tm * localTime;
  localTime = localtime(&now);
  int currDay  = localTime->tm_yday;
  timeValue_t currTime = timeValue_t(localTime->tm_hour, localTime->tm_min);
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

  pidController.setpoint=controllerData->temperatureSetpoint;

  // Ask PID controller for new value
  controllerData->requestedBoilerTemperature = pidController.newValue( controllerData->measuredTemperature() );
  controllerData->boilerPercentage=pidController.boilerPercentage;

  // Switch off central heating if it becomes too hot in the room
  controllerData->enableCentralHeating = (controllerData->measuredTemperature() < (controllerData->temperatureSetpoint+2.0));

  // Also switch off heater if we request almost no heat
  if(controllerData->boilerPercentage < 0.05) controllerData->enableCentralHeating=false;

  if (controllerData->flameOn) {
    if      (controllerData->boilerPercentage < 0.05) controllerData->dspFlameIcon = iconFlame0; 
    else if (controllerData->boilerPercentage < 0.10) controllerData->dspFlameIcon = iconFlame1; 
    else if (controllerData->boilerPercentage < 0.15) controllerData->dspFlameIcon = iconFlame2; 
    else if (controllerData->boilerPercentage < 0.20) controllerData->dspFlameIcon = iconFlame3; 
    else if (controllerData->boilerPercentage < 0.35) controllerData->dspFlameIcon = iconFlame4; 
    else if (controllerData->boilerPercentage < 0.50) controllerData->dspFlameIcon = iconFlame5; 
    else if (controllerData->boilerPercentage < 0.65) controllerData->dspFlameIcon = iconFlame6; 
    else if (controllerData->boilerPercentage < 0.80) controllerData->dspFlameIcon = iconFlame7; 
    else                                              controllerData->dspFlameIcon = iconFlame8; 
  }
  else controllerData->dspFlameIcon = iconFlame0;

  for(int i=0; i<7; i++) {
    daytype=controllerData->regularWeek[dayOfWeek];  // 0=Sunday
    controllerData->dayIcons[i]=DAYTYPE_TO_ICON[daytype];
    dayOfWeek++;
    if(dayOfWeek>6) dayOfWeek=0;
  };

  // Override multiple days if needed
  if(controllerData->overrideMultiple!=dtAuto) {
    for(int i=0; i < controllerData->overrideMultipleCount; i++) {
      if(i<7) controllerData->dayIcons[i]=DAYTYPE_OVERRULED_TO_ICON[ controllerData->overrideMultiple ];
    };  
  };

  // Override today if needed
  if( controllerData->overrideToday != dtAuto) {
    controllerData->dayIcons[0]=DAYTYPE_OVERRULED_TO_ICON[ controllerData->overrideToday ];
  }

  // Override tomorrow if needed
  if(controllerData->overrideTomorrow!=dtAuto) {
    controllerData->dayIcons[1]=DAYTYPE_OVERRULED_TO_ICON[controllerData->overrideTomorrow];
  }

  controllerData->P          = pidController.P;
  controllerData->I          = pidController.I;
  controllerData->D          = pidController.D;
  controllerData->ierr       = pidController.ierr;
  controllerData->derivative = pidController.derivative;

  //controllerData->logBusyTime.finish(btController);  

  // Remember for the next calculation
  prevDay=currDay;
  prevTime=currTime;
};

void Controller::setTemperatureOffset(float offset) { 
    controllerData->sensorOffset=offset;
};

void Controller::increaseSensorOffset() { 
    controllerData->sensorOffset+=0.1;
}

void Controller::decreaseSensorOffset() { 
    controllerData->sensorOffset-=0.1;
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
  control();
};

void Controller::setpointHigher() {
  Serial.println("Controller::setpointHigher()");
  
  if(!controllerData->overrideTempNow) {
    controllerData->overrideSetpoint= 0.5*round(2*controllerData->measuredTemperature() )+0.5;
  }
  else {
    if(controllerData->overrideSetpoint<30) controllerData->overrideSetpoint+=0.5;
  }
  controllerData->overrideTempNow=true;
  control();
};

void Controller::setpointAuto() { 
  Serial.println("Controller::setpointAuto()");
  
  controllerData->overrideTempNow=false;
  control();
};

void Controller::goAway() { 
  Serial.println("Controller::goAway()");

  controllerData->overrideToday=dtAway;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWorkFromHome() { 
  Serial.println("Controller::overruleTodayWorkFromHome()");

  controllerData->overrideToday=dtWorkFromHome;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWorkFromOffice() { 
  Serial.println("Controller::overruleTodayWorkFromOffice()");

  controllerData->overrideToday=dtWorkAtOffice;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWeekend() {
  Serial.println("Controller::overruleTodayWeekend()");
  controllerData->overrideToday=dtWeekend;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTodayAway() { 
  Serial.println("Controller::overruleTodayAway()");
  controllerData->overrideToday=dtAway;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTodayAutomatic() { 
  Serial.println("Controller::overruleTodayAutomatic()");
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleTomorrowWorkFromHome() { 
  Serial.println("Controller::overruleTomorrowWorkFromHome()");
  controllerData->overrideTomorrow=dtWorkFromHome;
  control();
};

void Controller::overruleTomorrowWorkFromOffice() { 
  Serial.println("Controller::overruleTomorrowWorkFromOffice()");
  controllerData->overrideTomorrow=dtWorkAtOffice;
  control();
};

void Controller::overruleTomorrowWeekend() { 
  Serial.println("Controller::overruleTomorrowWeekend()");
  controllerData->overrideTomorrow=dtWeekend;
  control();
};

void Controller::overruleTomorrowAway() { 
  Serial.println("Controller::overruleTomorrowAway()");
  controllerData->overrideTomorrow=dtAway;
  control();
};

void Controller::overruleTomorrowAutomatic() { 
  Serial.println("Controller::overruleTomorrowAutomatic()");
  controllerData->overrideTomorrow= dtAuto;
  control();
};

void Controller::overruleMultipleWorkFromHome() { 
  Serial.println("Controller::overruleMultipleWorkFromHome()");
  controllerData->overrideMultiple=dtWorkFromHome;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleWorkFromOffice() { 
  Serial.println("Controller::overruleMultipleWorkFromOffice()");
  controllerData->overrideMultiple=dtWorkAtOffice;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleWeekend() { 
  Serial.println("Controller::overruleMultipleWeekend()");
  controllerData->overrideMultiple=dtWeekend;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleAway() { 
  Serial.println("Controller::overruleMultipleAway()");
  controllerData->overrideMultiple=dtAway;
  controllerData->overrideTomorrow=dtAuto;
  controllerData->overrideToday=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleAutomatic() { 
  Serial.println("Controller::overruleMultipleAutomatic()");
  controllerData->overrideMultiple=dtAuto;
  controllerData->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleLessDays() { 
  Serial.println("Controller::overruleMultipleLessDays()");
  if(controllerData->overrideMultipleCount > 2) controllerData->overrideMultipleCount--;
  control();
};

void Controller::overruleMultipleMoreDays() { 
  Serial.println("Controller::overruleMultipleMoreDays()");
  controllerData->overrideMultipleCount++;
  control();
};

void Controller::setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay) {
  Serial.println("Controller::setWeekSchedule()");
  controllerData->regularWeek[dayOfWeek]=typeOfDay;
  control();
};

void Controller::workFromHomeWakeUpEarlier() { 
  Serial.println("Controller::workFromHomeWakeUpEarlier()");
  if(controllerData->workFromHomeWakeUp >= timeValue_t(0,15)) controllerData->workFromHomeWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeWakeUpLater() { 
  Serial.println("Controller::workFromHomeWakeUpLater()");
  if(controllerData->workFromHomeWakeUp <= timeValue_t(23,15)) controllerData->workFromHomeWakeUp+=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeSleepEarlier() { 
  Serial.println("Controller::workFromHomeSleepEarlier()");
  if(controllerData->workFromHomeSleep >= timeValue_t(0,15)) controllerData->workFromHomeSleep-=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeSleepLater() { 
  Serial.println("Controller::workFromHomeSleepLater()");
  if(controllerData->workFromHomeSleep <= timeValue_t(23,15)) controllerData->workFromHomeSleep+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeWakeUpEarlier() { 
  Serial.println("Controller::workFromOfficeWakeUpEarlier()");
  if(controllerData->workAtOfficeWakeUp >= timeValue_t(0,15)) controllerData->workAtOfficeWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeWakeUpLater() { 
  Serial.println("Controller::workFromOfficeWakeUpLater()");
  if(controllerData->workAtOfficeWakeUp <= timeValue_t(23,15)) controllerData->workAtOfficeWakeUp+=timeValue_t(0,15);
  control();
};
 
void Controller::workFromOfficeGoOutEarlier() { 
  Serial.println("Controller::workFromOfficeGoOutEarlier()");
  if(controllerData->workAtOfficeGoOut >= timeValue_t(0,15)) controllerData->workAtOfficeGoOut-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeGoOutLater() { 
  Serial.println("Controller::workFromOfficeGoOutLater()");
  if(controllerData->workAtOfficeGoOut <= timeValue_t(23,15)) controllerData->workAtOfficeGoOut+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeComeInEarlier() { 
  Serial.println("Controller::workFromOfficeComeInEarlier()");
  if(controllerData->workAtOfficeComeIn >= timeValue_t(0,15)) controllerData->workAtOfficeComeIn-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeComeInLater() { 
  Serial.println("Controller::workFromOfficeComeInLater()");
  if(controllerData->workAtOfficeComeIn <= timeValue_t(23,15)) controllerData->workAtOfficeComeIn+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeSleepEarlier() { 
  Serial.println("Controller::workFromOfficeSleepEarlier()");
  if(controllerData->workAtOfficeSleep >= timeValue_t(0,15)) controllerData->workAtOfficeSleep-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeSleepLater() { 
  Serial.println("Controller::workFromOfficeSleepLater()");
  if(controllerData->workAtOfficeSleep <= timeValue_t(23,15)) controllerData->workAtOfficeSleep+=timeValue_t(0,15);
  control();
};

void Controller::weekendWakeUpEarlier() { 
  Serial.println("Controller::weekendWakeUpEarlier()");
  if(controllerData->weekendWakeUp >= timeValue_t(0,15)) controllerData->weekendWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::weekendWakeUpLater() { 
  Serial.println("Controller::weekendWakeUpLater()");
  if(controllerData->weekendWakeUp <= timeValue_t(23,15)) controllerData->weekendWakeUp+=timeValue_t(0,15);
  control();
};

void Controller::weekendSleepEarlier() { 
  Serial.println("Controller::weekendSleepEarlier()");
  if(controllerData->weekendSleep >= timeValue_t(0,15)) controllerData->weekendSleep-=timeValue_t(0,15);
  control();
};

void Controller::weekendSleepLater() { 
  Serial.println("Controller::weekendSleepLater()");
  if(controllerData->weekendSleep <= timeValue_t(23,15)) controllerData->weekendSleep+=timeValue_t(0,15);
  control();
};

void Controller::highTempLower() { 
  Serial.println("Controller::highTempLower()");
  if(controllerData->highTemp > controllerData->lowTemp+0.5) controllerData->highTemp-=0.5;
  control();
};

void Controller::highTempHigher() { 
  Serial.println("Controller::highTempHigher()");
  if(controllerData->highTemp<30) controllerData->highTemp+=0.5;
  control();
};

void Controller::lowTempLower() { 
  Serial.println("Controller::lowTempLower()");
  if(controllerData->lowTemp > 4.0) controllerData->lowTemp-=0.5;
  control();
};

void Controller::lowTempHigher() { 
  Serial.println("Controller::lowTempHigher()");
  if(controllerData->lowTemp < controllerData->highTemp-0.5) controllerData->lowTemp+=0.5;
  control();
};

/******************************************
 * GLOBAL VARIABLES                       * 
 ******************************************/

Controller controller=Controller(controllerData); // Controller

void processControllerIfNeeded() {
  userEventMessage_t message;

  if( xQueueReceive( controllerQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Controller [%s] k=%d keyboardMode: %s\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed, keyboardMode ? "Y" : "N");

    // Give controller exclusive access to controllerData and execute the command
    if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
      controller.processCommand(message);
      xSemaphoreGive( controllerDataSemaphore );
    } else {
      Serial.println("Time out for controllerDataSemaphore in handleController");    
      TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder( controllerDataSemaphore );    
      char *pcTaskName = pcTaskGetTaskName(mutexHolder);
      Serial.printf("Mutex held by %s\n", pcTaskName);
    }

    // Report to display or Telegram that the message was carried out
    if( message.sender == sndMenu ) {
      message.sender = sndController;
      xQueueSend( displayQueue, &message, ( TickType_t ) 0 );  
    }
    else if( message.sender == sndTelegram ) {
      message.sender = sndController;
      xQueueSend( telegramQueue, &message, ( TickType_t ) 0 );  
    }

  }
}
