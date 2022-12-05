#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

#define RECONNECT_INTERVAL   5*60*1000
#define TEMP_MEAS_INTERVAL     30*1000 
#define BOILER_INTERVAL      3*60*1000

OneWire oneWire(PIN_ONE_WIRE_BUS);                         // Bus for the temperature sensors 
DallasTemperature dallasSensor(&oneWire);                  // Driver for temperature sensors
OpenTherm opentherm(PIN_OPENTHERM_IN, PIN_OPENTHERM_OUT);  // Driver for boiler

//Interrupthandler for OpenTherm
void ICACHE_RAM_ATTR handleInterrupt() {
  opentherm.handleInterrupt();
}


void TemperatureMeasure();

void TemperatureSensorConnect() {
  
  // Initialize thermometer
  dallasSensor.begin();

  controllerData.numSensors=dallasSensor.getDeviceCount();
  Serial.printf("%d device(s) found on OneWire bus\n", controllerData.numSensors);

  for(int i=0; i<controllerData.numSensors; i++) {

    if ( (i<MAX_NUM_SENSORS) and dallasSensor.getAddress(controllerData.sensorAddress[i], i) ) {
      
        Serial.print("Dallas 18B20 found on address:\t");
        for (uint8_t j = 0; j < 8; j++) Serial.printf("%02X ", controllerData.sensorAddress[i][j]);
        Serial.println();
  
        // Set the therometer precision
        dallasSensor.setResolution(controllerData.sensorAddress[i], TEMPERATURE_PRECISION);
        Serial.printf("Resolution of sensor set to: %d\n",dallasSensor.getResolution(controllerData.sensorAddress[i]));
      }  
  }
    
  Serial.printf("Parasite power on OneWire bus is: %s\n", dallasSensor.isParasitePowerMode() ? "ON": "OFF");
  controllerData.temperatureMeasurementOK = (controllerData.numSensors>0);

  // Do the first temperature measurement
  TemperatureMeasure();
};

void TemperatureMeasure() {
  float measuredTemperature;
  float sum = 0;
  int   cnt = 0;
  
  controllerData.temperatureMeasurementOK = true;

  //controllerData.logBusyTime.start(btTemperature);

  dallasSensor.requestTemperatures();

  for (int i=0; i<controllerData.numSensors; i++) {
    measuredTemperature=dallasSensor.getTempC(controllerData.sensorAddress[i]);
    if(measuredTemperature!=DEVICE_DISCONNECTED_C) {
      sum+=measuredTemperature;
      cnt++;
    }
    else Serial.printf("Sensor %d not connected ", i);
  };

  // We're happy if at least one sensor is left
  if( cnt<1 ) controllerData.temperatureMeasurementOK = false;

  if(controllerData.temperatureMeasurementOK) {
    measuredTemperature = sum/cnt;    
    if(measuredTemperature < -40) controllerData.temperatureMeasurementOK = false;
  }

  // Only update raw temperture if all went well
  if ( controllerData.temperatureMeasurementOK ) {
    controllerData.rawTemperature = measuredTemperature;
    controllerData.lastTemperatureMeasurement = timeNow();
  }

  //controllerData.logBusyTime.finish(btTemperature);  
}

void BoilerCommunicate() {  
  static bool  firstEnable                   = true;
  static bool  firstTemperature              = true;
  static bool  lastEnableCentralHeating      = false; 
  static bool  lastEnableHotWater            = false;
  static bool  lastEnableCooling             = false;
  static float lastBoilerTemperatureSetpoint = -300;
  static float lastDomesticHotWaterSetpoint  = -300;

  userEventMessage_t message;
  
  unsigned long response;
  
  Serial.println("BoilerCommunicate()");

  // Current day and time
  time_t now;
  time(&now); // Get current time
  struct tm * localTime;
  localTime = localtime(&now);
  int currDay  = localTime->tm_yday;
  timeValue_t currTime = timeValue_t(localTime->tm_hour, localTime->tm_min);
  
  // Switch keyboard off (just to make sure, potentially not required)
  disableKeyboard(); 

  // Enable central heating
  /*
  if(   firstEnable or 
      ( lastEnableCentralHeating != controllerData.enableCentralHeating ) or 
      ( lastEnableHotWater       != controllerData.enableHotWater       ) or 
      ( lastEnableCooling        != controllerData.enableCooling        ) ) {
        
  */
  if(true) {
    response = opentherm.setBoilerStatus(controllerData.enableCentralHeating, controllerData.enableHotWater, controllerData.enableCooling);
    
    switch (opentherm.getLastResponseStatus() ) {
      case OpenThermResponseStatus::SUCCESS:
        Serial.println("Boiler enable response: success");
        firstEnable                         = false;
        controllerData.boilerOnline         = true;
        lastEnableCentralHeating            = controllerData.enableCentralHeating;
        lastEnableHotWater                  = controllerData.enableHotWater;
        lastEnableCooling                   = controllerData.enableCooling;
  
        controllerData.boilerResponse       = boilerSuccess;
        controllerData.lastBoilerUpdate     = currTime;
        controllerData.boilerOnline         = true;
        controllerData.centralHeatingActive = opentherm.isCentralHeatingActive(response);
        controllerData.hotWaterActive       = opentherm.isHotWaterActive(response);
        controllerData.flameOn              = opentherm.isFlameOn(response);
      break;

      case OpenThermResponseStatus::NONE:
        Serial.println("Boiler enable response: none");
        controllerData.boilerResponse = boilerNone;
        controllerData.boilerOnline   = false;
      break;

      case OpenThermResponseStatus::INVALID:
        Serial.println("Boiler enable response: invalid");
        controllerData.boilerResponse = boilerInvalid;
        controllerData.boilerOnline   = false;
      break;

      case OpenThermResponseStatus::TIMEOUT:
        Serial.println("Boiler response: timeout");
        controllerData.boilerResponse = boilerTimeout;
        controllerData.boilerOnline   = false;
      break;
    } // switch (opentherm.getLastResponseStatus() )
  } // if firstStatus    
  
  // Set boiler temperature once boiler enable status was successful
  /*
  if( !firstEnable and ( firstTemperature or (controllerData.requestedBoilerTemperature != lastBoilerTemperatureSetpoint) ) ) {
  */

  if(true) {
    opentherm.setBoilerTemperature(controllerData.requestedBoilerTemperature);
    switch( opentherm.getLastResponseStatus() ) {
      case OpenThermResponseStatus::SUCCESS:
        firstTemperature                = false;
        lastBoilerTemperatureSetpoint   = controllerData.requestedBoilerTemperature; // Update last requested setpoint
        controllerData.lastBoilerUpdate = currTime;
      break;

      case OpenThermResponseStatus::NONE:
        Serial.println("Boiler response: none");
        controllerData.boilerResponse = boilerNone;
        controllerData.boilerOnline   = false;
      break;

      case OpenThermResponseStatus::INVALID:
        Serial.println("Boiler response: invalid");
        controllerData.boilerResponse = boilerInvalid;
        controllerData.boilerOnline   = false;
      break;

      case OpenThermResponseStatus::TIMEOUT:
        Serial.println("Boiler response: timeout");
        controllerData.boilerResponse = boilerTimeout;
        controllerData.boilerOnline   = false;
      break;
    }
  }

  // Set domestic hot water setpoint
  /*
  if( lastDomesticHotWaterSetpoint != controllerData.requestedDomesticHotWaterSetpoint ) {
  */

  if(true) {
    opentherm.setDHWSetpoint(controllerData.requestedDomesticHotWaterSetpoint);  
    if (opentherm.getLastResponseStatus() == OpenThermResponseStatus::SUCCESS) {
      lastDomesticHotWaterSetpoint = controllerData.requestedDomesticHotWaterSetpoint;
    }
  }

  if (opentherm.getLastResponseStatus()==OpenThermResponseStatus::SUCCESS) {
    controllerData.lastBoilerUpdate                  = currTime;
    controllerData.actualBoilerTemperature           = opentherm.getBoilerTemperature();
    controllerData.actualDomesticHotWaterTemperature = opentherm.getDHWTemperature();
    controllerData.boilerPressure                    = opentherm.getPressure();
  }

  // Switch keyboard on
  enableKeyboard(); 

  //controllerData.logBusyTime.finish(btOpentherm);  

  controllerData.dspShowerIcon = controllerData.hotWaterActive ? iconShowerOn : iconShowerOff;
  controllerData.dspBoilerIcon = ( controllerData.boilerResponse == boilerSuccess ) ? iconBoilerConnected : iconBoilerLost;

  if(!controllerData.boilerOnline) {
    controllerData.centralHeatingActive = false;
    controllerData.hotWaterActive       = false;
    controllerData.flameOn              = false;      
    controllerData.dspShowerIcon        = iconShowerOff;
  }

}

void startTemperature() {
  // Connect temperature sensors
  TemperatureSensorConnect();    

  // Start connection with boiler
  opentherm.begin(handleInterrupt);
}

void checkTemperatureIfNeeded() {
  static unsigned long lastTempMeas=0;      // last time temperature measurement was done
  static unsigned long lastReconnect=0;     // last time temperaturesensors were reconnected was done
  static unsigned long lastBoilerConnect=0; // last time boiler was talked to
  userEventMessage_t message;
  
  if (millis() - lastTempMeas > TEMP_MEAS_INTERVAL) {
    TemperatureMeasure();

    // Request controller to calculate new setpoint
    // Send new temperature to display without switching on backlight
    sendMessage(sndTemperature, cmdSilentUpdate, controllerQueue);
    
    lastTempMeas=millis();
  }

  if( millis() - lastBoilerConnect > BOILER_INTERVAL ) {

    // Request controller to calculate new setpoint
    sendMessage(sndTemperature, cmdBoilerSending, controllerQueue);
    lastBoilerConnect=millis();
  }

  if (millis() - lastReconnect > RECONNECT_INTERVAL) {
    if(!controllerData.temperatureMeasurementOK) TemperatureSensorConnect();
    lastReconnect=millis();
  }

  if( xQueueReceive( temperatureQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Temperature [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed );
    BoilerCommunicate();
    sendMessage(sndTemperature, cmdSilentUpdate, controllerQueue);
  }
}
