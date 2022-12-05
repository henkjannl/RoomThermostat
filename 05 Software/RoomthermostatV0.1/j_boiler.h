#pragma once

#include <OpenTherm.h>

// Global variables
#define BOILER_INTERVAL 20000
//#define BOILER_INTERVAL 200000


class Boiler {
  public:
    Boiler(OpenTherm & opentherm, DataBus & databus, Display & display, keyboardFunction disableKeyboard, keyboardFunction enableKeyboard);
    void begin();
    void loop();
    void update();
  protected:
    OpenTherm *opentherm;
    DataBus *databus;
    Display *display; // The boiler needs a pointer to the screen to modify the boiler icon
    keyboardFunction disableKeyboard;  // pointer to the function that disables the keyboard
    keyboardFunction enableKeyboard;   // pointer to the function that enables the keyboard

};

Boiler::Boiler(OpenTherm & opentherm, DataBus & databus, Display & display, keyboardFunction disableKeyboard, keyboardFunction enableKeyboard) {
  // Connect the right pointers but do not initialize yet
  this->opentherm       = & opentherm; 
  this->databus         = & databus; 
  this->display         = & display; 
  this->disableKeyboard = disableKeyboard; 
  this->enableKeyboard  = enableKeyboard; 
};

void Boiler::begin() {
  Serial.println("Boiler started.");
  // Nothing to do, only for consistency
};  

// Boiler status is requested every 20 seconds, or if the controller says so
void Boiler::loop() {
  static unsigned long previousAttempt = 0;

  if( millis() - previousAttempt > BOILER_INTERVAL ) {
    previousAttempt=millis();
    update();
  }
}

void Boiler::update() {
  String boilerStatusMessage;

  Serial.println("Boiler::update()");
  databus->logBusyTime.start(btBoiler);  

  // Make arrows on boiler connect icon red
  databus->dpBoilerConnIcon.setValue(iconBoilerTransmit);
  display->loop();

  // Switch keyboard off (just to make sure, perhaps not required)
  disableKeyboard(); 

  Serial.printf("Boiler request: heating %s, hotwater %s, cooling %s.\n", 
    databus->enableCentralHeating ? "enabled" : "disabled", 
    databus->enableHotWater       ? "enabled" : "disabled", 
    databus->enableCooling        ? "enabled" : "disabled");

  databus->logBusyTime.start(btOpentherm);  
  
  unsigned long response = opentherm->setBoilerStatus(databus->enableCentralHeating, databus->enableHotWater, databus->enableCooling);
  OpenThermResponseStatus responseStatus = opentherm->getLastResponseStatus();

  switch(responseStatus) {

    case OpenThermResponseStatus::NONE:
      Serial.print("Error: OpenTherm is not initialized");
      databus->boilerOnline = false;
      databus->responseFromBoiler="OpenTherm not initialized";
      break;

    case OpenThermResponseStatus::SUCCESS:
      databus->boilerOnline=true;
      databus->centralHeatingActive = opentherm->isCentralHeatingActive(response);
      databus->hotWaterActive       = opentherm->isHotWaterActive(response);
      databus->flameOn              = opentherm->isFlameOn(response);
      databus->responseFromBoiler   = "OpenTherm success";
      Serial.print("Central Heating: " + String(databus->centralHeatingActive ? "on " : "off "));
      Serial.print("Hot Water: "       + String(databus->hotWaterActive       ? "on " : "off "));
      Serial.print("Flame: "           + String(databus->flameOn              ? "on " : "off "));
      break;

    case OpenThermResponseStatus::INVALID:
      Serial.print("Error: Invalid response " + String(response, HEX));
      databus->responseFromBoiler="Invalid response from OpenTherm";
      databus->boilerOnline=false;
      break;

    case OpenThermResponseStatus::TIMEOUT:
      Serial.print("Error: Response timeout");
      databus->boilerOnline=false;
      databus->responseFromBoiler="OpenTherm timeout";
      break;
    }

  //Set boiler temperature
  opentherm->setBoilerTemperature(databus->requestedBoilerTemperature);
  Serial.printf("Boiler request for %.1f°C\n", databus->requestedBoilerTemperature);

  //Get Boiler Temperature
  databus->actualBoilerTemperature = opentherm->getBoilerTemperature();
  Serial.printf("Actual boiler temperature is %.1f°C\n", databus->actualBoilerTemperature );

  //Set domestic hot water setpoint
  opentherm->setDHWSetpoint(databus->requestedDomesticHotWaterSetpoint);
  
  //Get domestic hot water temperature
  databus->actualDomesticHotWaterTemperature = opentherm->getDHWTemperature();
  Serial.printf("Actual hot water temperature is %.3f°C\n", databus->actualDomesticHotWaterTemperature );

  // Switch keyboard back on
  enableKeyboard(); 

  databus->logBusyTime.finish(btOpentherm);  

  if(!databus->boilerOnline) {
    databus->centralHeatingActive = false;
    databus->hotWaterActive       = false;
    databus->flameOn              = false;      
  }

  // Change boiler connect icon
  databus->dpBoilerConnIcon.setValue(databus->boilerOnline   ? iconBoilerConnected : iconBoilerLost );  
  databus->dpShowerIcon    .setValue(databus->hotWaterActive ? iconShowerOn        : iconShowerOff  );

  display->loop();
  databus->logBusyTime.finish(btBoiler);  
}
