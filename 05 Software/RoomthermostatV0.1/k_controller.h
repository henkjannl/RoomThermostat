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
    Controller(DataBus & databus, MenuStructure & menu, TemperatureSensor & temperatureSensor, Display & display);
    
    // Calculate icons and update setpoint
    void loop();
    void control();

    // Various handlers for the user interface, both device and Telegram
    void setTemperatureOffset(float offset) { temperatureSensor->setOffset(offset);      };
    void increaseSensorOffset()             { temperatureSensor->increaseSensorOffset(); };
    void decreaseSensorOffset()             { temperatureSensor->decreaseSensorOffset(); };
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

  protected:
    DataBus           *databus;
    MenuStructure     *menu;
    TemperatureSensor *temperatureSensor;
    Display           *display;
    PID pidController;    
};

Controller::Controller(DataBus & databus, MenuStructure & menu, TemperatureSensor & temperatureSensor, Display & display) { 
  this->databus           = & databus; 
  this->menu              = & menu;
  this->temperatureSensor = & temperatureSensor; 
  this->display           = & display;
};

void Controller::loop() {
  static unsigned long previousAttempt = 0;

  if( millis() - previousAttempt > CONTROLLER_INTERVAL) {
    Serial.println("Controller::loop()");
    control();
    previousAttempt=millis();
  }; 
};

void Controller::control() {
  static int prevDay=0;
  static timeValue_t prevTime = timeValue_t(0,0);
  dayType_t currDayType;
  dayType_t daytype;
  char buffer[32];

  Serial.println("Controller::control()");
  databus->logBusyTime.start(btController);  

  // Measure temperature
  temperatureSensor->measure();

  // Current day and time
  time_t now;
  time(&now); // Get current time

  struct tm * timeinfo;
  timeinfo = localtime(&now);
  int currDay  = timeinfo->tm_yday;
  timeValue_t currTime = timeValue_t(timeinfo->tm_hour, timeinfo->tm_min);
  uint8_t dayOfWeek = timeinfo->tm_wday;

  // Calculate new setpoint

  // Shift days at midnight
  if(currDay != prevDay) {
    databus->overrideToday = databus->overrideTomorrow;
    databus->overrideTomorrow=dtAuto;
    
    if (databus->overrideMultiple!=dtAuto) {
      if(databus->overrideMultipleCount>0)
        databus->overrideMultipleCount--;
      else
        databus->overrideMultiple=dtAuto;
    }
  }

  // Determine what kind of day we have today
  currDayType=databus->regularWeek[dayOfWeek];
  databus->reasonForSetpoint = spWeekSchedule;

  if ( (databus->overrideMultiple!=dtAuto) and (databus->overrideMultipleCount>0) ) {
    currDayType=databus->overrideMultiple;
    databus->reasonForSetpoint = spMultipleDays;
  }

  if(databus->overrideToday!=dtAuto) {
    currDayType=databus->overrideToday;
    databus->reasonForSetpoint = spOverrideToday;
  }

  // Check if we just passed a switch moment
  switch(currDayType) {

    case dtWorkFromHome:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < databus->workFromHomeWakeUp) and (databus->workFromHomeWakeUp <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromHomeWakeUp"); }
      else if( (prevTime < databus->workFromHomeSleep ) and (databus->workFromHomeSleep  <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromHomeSleep" ); };
      
      if(!databus->overrideTempNow) {
        databus->temperatureSetpoint = databus->lowTemp;
        if( (databus->workFromHomeWakeUp <= currTime) and (currTime < databus->workFromHomeSleep) ) databus->temperatureSetpoint = databus->highTemp;
      }
    break;

    case dtWorkAtOffice:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime < databus->workAtOfficeWakeUp) and (databus->workAtOfficeWakeUp <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromOfficeWakeUp"); }
      else if( (prevTime < databus->workAtOfficeGoOut ) and (databus->workAtOfficeGoOut  <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromOfficeGoOut" ); }
      else if( (prevTime < databus->workAtOfficeComeIn) and (databus->workAtOfficeComeIn <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromOfficeComeIn"); }
      else if( (prevTime < databus->workAtOfficeSleep ) and (databus->workAtOfficeSleep  <= currTime) ) { databus->overrideTempNow=false; logMessage("WorkFromOfficeSleep" ); };
      
      if(!databus->overrideTempNow) {
        databus->temperatureSetpoint = databus->lowTemp;
        if     ( (databus->workAtOfficeWakeUp<=currTime) and (currTime<databus->workAtOfficeGoOut) ) databus->temperatureSetpoint = databus->highTemp;
        else if( (databus->workAtOfficeComeIn<=currTime) and (currTime<databus->workAtOfficeSleep) ) databus->temperatureSetpoint = databus->highTemp;
      }      
    break;

    case dtAway:
      if(currDay != prevDay) { databus->overrideTempNow=false; logMessage("Day away next day" ); };

      if(!databus->overrideTempNow) databus->temperatureSetpoint = databus->lowTemp;
    break;

    case dtWeekend:
      // Switch off temperature override if we pass a switch moment
      if     ( (prevTime<databus->weekendWakeUp) and (databus->weekendWakeUp<=currTime) ) { databus->overrideTempNow=false; logMessage("WeekendWakeUp"); }
      else if( (prevTime<databus->weekendSleep ) and (databus->weekendSleep <=currTime) ) { databus->overrideTempNow=false; logMessage("WeekendSleep" ); };
      
      if(!databus->overrideTempNow) {
        databus->temperatureSetpoint = databus->lowTemp;
        if( (databus->weekendWakeUp<=currTime) and (currTime<databus->weekendSleep) ) databus->temperatureSetpoint = databus->highTemp;
      }
    break;
  }

  databus->dpOverrideTempNow.setValue(databus->overrideTempNow);

  if(databus->overrideTempNow) {
    databus->reasonForSetpoint = spOverrideNow;
    databus->temperatureSetpoint=databus->overrideSetpoint;
  } else {
    databus->overrideSetpoint = int(databus->temperatureSetpoint+0.5);
  }

  pidController.setpoint=databus->temperatureSetpoint;

  // Ask PID controller for new value
  databus->requestedBoilerTemperature = pidController.newValue(databus->measuredTemperature);
  databus->boilerPercentage=pidController.boilerPercentage;

  // Switch off central heating if it becomes too hot in the room
  databus->enableCentralHeating = (databus->measuredTemperature < (databus->temperatureSetpoint+2.0));
  
  // Also switch off heater if we request almost no heat
  if(databus->boilerPercentage < 0.05) databus->enableCentralHeating=false;
  
  databus->P          = pidController.P;
  databus->I          = pidController.I;
  databus->D          = pidController.D;
  databus->ierr       = pidController.ierr;
  databus->derivative = pidController.derivative;

  if (databus->flameOn) {
    if      (databus->boilerPercentage < 0.05) databus->dpFlameIcon.setValue(iconFlame0); 
    else if (databus->boilerPercentage < 0.10) databus->dpFlameIcon.setValue(iconFlame1); 
    else if (databus->boilerPercentage < 0.15) databus->dpFlameIcon.setValue(iconFlame2); 
    else if (databus->boilerPercentage < 0.20) databus->dpFlameIcon.setValue(iconFlame3); 
    else if (databus->boilerPercentage < 0.35) databus->dpFlameIcon.setValue(iconFlame4); 
    else if (databus->boilerPercentage < 0.50) databus->dpFlameIcon.setValue(iconFlame5); 
    else if (databus->boilerPercentage < 0.65) databus->dpFlameIcon.setValue(iconFlame6); 
    else if (databus->boilerPercentage < 0.80) databus->dpFlameIcon.setValue(iconFlame7); 
    else                                       databus->dpFlameIcon.setValue(iconFlame8); 
  }
  else databus->dpFlameIcon.setValue(iconFlame0);

  // CREATE SUBHEADERS FOR ACTIVE SCREEN
  // By default, assume screen menus have no subtitles
  databus->showSubMenuLines=0;

  // Homescreen
  if(menu->selectedScreen()==scHomeScreen) {
  
    strftime(buffer, 32, "%A %e %B", timeinfo);
    databus->dpDate.setValue(databus->timeSynched ? buffer: " "  );
  
    strftime(buffer, 32, "%H:%M", timeinfo);
    databus->dpTime.setValue(databus->timeSynched ? buffer : "--:--");
  
    sprintf(buffer, "%.1f",databus->temperatureSetpoint);
    databus->dpTemperatureSetpoint.setValue(buffer);
    
    // Create temporary icons as not to unnecessarily trigger databus->dpWeekProgramIcons.invalid
    // By default, set the icons according to the weekly schedule
    enum icon_t icons[7];
    for(int i=0; i<7; i++) {
      daytype=databus->regularWeek[dayOfWeek];  // 0=Sunday
      icons[i]=DAYTYPE_TO_ICON[daytype];
      dayOfWeek++;
      if(dayOfWeek>6) dayOfWeek=0;
    };
  
    // Override multiple days if needed
    if(databus->overrideMultiple!=dtAuto) {
      for(int i=0; i<databus->overrideMultipleCount; i++) {
        if(i<7) icons[i]=DAYTYPE_TO_ICON[databus->overrideMultiple];
      };  
    };
  
    // Override today if needed
    if(databus->overrideToday!=dtAuto) {
      icons[0]=DAYTYPE_TO_ICON[databus->overrideToday];
    }
  
    // Override tomorrow if needed
    if(databus->overrideTomorrow!=dtAuto) {
      icons[1]=DAYTYPE_TO_ICON[databus->overrideTomorrow];
    }
  
    // Copy icons to databus
    for(int i=0; i<7; i++) databus->dpWeekProgramIcons[i].setValue(icons[i]);
  } // if(menu->selectedScreen()==scHomeScreen) {
  
  // Overrule multiple days
  else if ( (menu->selectedScreen()==scOverruleMultiple) or (menu->selectedScreen()==scOverruleMultiple2) ) {
    if(databus->overrideMultiple==dtAuto) {
      databus->showSubMenuLines=1;
      snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "Multple days currently not set");
    }
    else {
      timeinfo = localtime(&now);
      timeinfo->tm_mday += databus->overrideMultipleCount; // Add some days
      mktime(timeinfo);
            
      strftime(buffer, 32, "%A %e %B", timeinfo);
      //sprintf(buffer,"Wednesday 30 September"); // Longest date possible to check if it fits the screen

      databus->showSubMenuLines=2;
      snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "%s for %d days", DAY_TYPES[databus->overrideMultiple].c_str(), databus->overrideMultipleCount);
      snprintf(databus->subMenuLine2, MAX_LENGTH_SUBMENU, "last day is %s", buffer);
    }
  }

  // Temperature settings
  else if(menu->selectedScreen()==scTemperatureSettings) {
    databus->showSubMenuLines=2;
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "High temperature %.1f°C", databus->highTemp);
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "Low temperature %.1f°C",  databus->lowTemp);
  }

  // Sensor offset
  else if(menu->selectedScreen()==scAdjustSensorOffset) {
    databus->showSubMenuLines=2;
    temperatureSensor->measure();
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "%.1f°C added to sensor signal", databus->temperatureSensorOffset);
    snprintf(databus->subMenuLine2, MAX_LENGTH_SUBMENU, "Current temperature %.1f°C", databus->measuredTemperature);
    }

  // Set home times
  else if(menu->selectedScreen()==scSetHomeTimes) {
    databus->showSubMenuLines=1;
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", databus->workFromHomeWakeUp.str(), databus->workFromHomeSleep.str());
    }

  // Set office times
  else if(menu->selectedScreen()==scSetOfficeTimes) {
    databus->showSubMenuLines=2;
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s go out %s", databus->workAtOfficeWakeUp.str(), databus->workAtOfficeGoOut.str());
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "come home %s sleep %s", databus->workAtOfficeComeIn.str(), databus->workAtOfficeSleep.str());
    }

  // Set weekend times
  else if(menu->selectedScreen()==scSetWeekendTimes) {
    databus->showSubMenuLines=1;
    snprintf(databus->subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", databus->weekendWakeUp.str(), databus->weekendSleep.str());
    }

  display->loop();

  databus->logBusyTime.finish(btController);  

  // Remember for the next calculation
  prevDay=currDay;
  prevTime=currTime;
};

void Controller::setpointLower() { 
  if(!databus->overrideTempNow) {
    databus->overrideSetpoint = 0.5*round(2*databus->measuredTemperature)-0.5;
  }
  else {
    if(databus->overrideSetpoint>0) databus->overrideSetpoint-=0.5;
  }
  databus->overrideTempNow=true;
  control();
};

void Controller::setpointHigher() {
  if(!databus->overrideTempNow) {
    databus->overrideSetpoint= 0.5*round(2*databus->measuredTemperature)+0.5;
  }
  else {
    if(databus->overrideSetpoint<30) databus->overrideSetpoint+=0.5;
  }
  databus->overrideTempNow=true;
  control();
};

void Controller::setpointAuto() { 
  databus->overrideTempNow=false;
  control();
};

void Controller::goAway() { 
  databus->overrideToday=dtAway;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWorkFromHome() { 
  databus->overrideToday=dtWorkFromHome;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWorkFromOffice() { 
  databus->overrideToday=dtWorkAtOffice;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTodayWeekend() {
  databus->overrideToday=dtWeekend;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTodayAway() { 
  databus->overrideToday=dtAway;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTodayAutomatic() { 
  databus->overrideToday=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleTomorrowWorkFromHome() { 
  databus->overrideTomorrow=dtWorkFromHome;
  control();
};

void Controller::overruleTomorrowWorkFromOffice() { 
  databus->overrideTomorrow=dtWorkAtOffice;
  control();
};

void Controller::overruleTomorrowWeekend() { 
  databus->overrideTomorrow=dtWeekend;
  control();
};

void Controller::overruleTomorrowAway() { 
  databus->overrideTomorrow=dtAway;
  control();
};

void Controller::overruleTomorrowAutomatic() { 
  databus->overrideTomorrow= dtAuto;
  control();
};

void Controller::overruleMultipleWorkFromHome() { 
  databus->overrideMultiple=dtWorkFromHome;
  databus->overrideToday=dtAuto;
  databus->overrideTomorrow=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleWorkFromOffice() { 
  databus->overrideMultiple=dtWorkAtOffice;
  databus->overrideTomorrow=dtAuto;
  databus->overrideToday=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleWeekend() { 
  databus->overrideMultiple=dtWeekend;
  databus->overrideTomorrow=dtAuto;
  databus->overrideToday=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleAway() { 
  databus->overrideMultiple=dtAway;
  databus->overrideTomorrow=dtAuto;
  databus->overrideToday=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleAutomatic() { 
  databus->overrideToday=dtAuto;
  databus->overrideTempNow=false;
  control();
};

void Controller::overruleMultipleLessDays() { 
  if(databus->overrideMultipleCount > 2) databus->overrideMultipleCount--;
  control();
};

void Controller::overruleMultipleMoreDays() { 
  databus->overrideMultipleCount++;
  control();
};

void Controller::setWeekSchedule(uint8_t dayOfWeek, dayType_t typeOfDay) {
  databus->regularWeek[dayOfWeek]=typeOfDay;
  control();
};

void Controller::workFromHomeWakeUpEarlier() { 
  if(databus->workFromHomeWakeUp >= timeValue_t(0,15)) databus->workFromHomeWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeWakeUpLater() { 
  if(databus->workFromHomeWakeUp <= timeValue_t(23,15)) databus->workFromHomeWakeUp+=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeSleepEarlier() { 
  if(databus->workFromHomeSleep >= timeValue_t(0,15)) databus->workFromHomeSleep-=timeValue_t(0,15);
  control();
};

void Controller::workFromHomeSleepLater() { 
  if(databus->workFromHomeSleep <= timeValue_t(23,15)) databus->workFromHomeSleep+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeWakeUpEarlier() { 
  if(databus->workAtOfficeWakeUp >= timeValue_t(0,15)) databus->workAtOfficeWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeWakeUpLater() { 
  if(databus->workAtOfficeWakeUp <= timeValue_t(23,15)) databus->workAtOfficeWakeUp+=timeValue_t(0,15);
  control();
};
 
void Controller::workFromOfficeGoOutEarlier() { 
  if(databus->workAtOfficeGoOut >= timeValue_t(0,15)) databus->workAtOfficeGoOut-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeGoOutLater() { 
  if(databus->workAtOfficeGoOut <= timeValue_t(23,15)) databus->workAtOfficeGoOut+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeComeInEarlier() { 
  if(databus->workAtOfficeComeIn >= timeValue_t(0,15)) databus->workAtOfficeComeIn-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeComeInLater() { 
  if(databus->workAtOfficeComeIn <= timeValue_t(23,15)) databus->workAtOfficeComeIn+=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeSleepEarlier() { 
  if(databus->workAtOfficeSleep >= timeValue_t(0,15)) databus->workAtOfficeSleep-=timeValue_t(0,15);
  control();
};

void Controller::workFromOfficeSleepLater() { 
  if(databus->workAtOfficeSleep <= timeValue_t(23,15)) databus->workAtOfficeSleep+=timeValue_t(0,15);
  control();
};

void Controller::weekendWakeUpEarlier() { 
  if(databus->weekendWakeUp >= timeValue_t(0,15)) databus->weekendWakeUp-=timeValue_t(0,15);
  control();
};

void Controller::weekendWakeUpLater() { 
  if(databus->weekendWakeUp <= timeValue_t(23,15)) databus->weekendWakeUp+=timeValue_t(0,15);
  control();
};

void Controller::weekendSleepEarlier() { 
  if(databus->weekendSleep >= timeValue_t(0,15)) databus->weekendSleep-=timeValue_t(0,15);
  control();
};

void Controller::weekendSleepLater() { 
  if(databus->weekendSleep <= timeValue_t(23,15)) databus->weekendSleep+=timeValue_t(0,15);
  control();
};

void Controller::highTempLower() { 
  if(databus->highTemp > databus->lowTemp+0.5) databus->highTemp-=0.5;
  control();
};

void Controller::highTempHigher() { 
  if(databus->highTemp<30) databus->highTemp+=0.5;
  control();
};

void Controller::lowTempLower() { 
  if(databus->lowTemp > 4.0) databus->lowTemp-=0.5;
  control();
};

void Controller::lowTempHigher() { 
  if(databus->lowTemp < databus->highTemp-0.5) databus->lowTemp+=0.5;
  control();
};
