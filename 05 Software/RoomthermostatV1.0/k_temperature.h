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

  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

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

    xSemaphoreGive( controllerDataSemaphore );
  }

  // Do the first temperature measurement
  TemperatureMeasure();
};

void TemperatureMeasure() {
    float measuredTemperature;
    float sum = 0;
    int   cnt = 0;
    bool  allWentWell = true;

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
    allWentWell=(cnt>0);
    measuredTemperature = allWentWell ? sum/cnt : 20.5;
    if(measuredTemperature < -40) {
      allWentWell=false;
      measuredTemperature = 20.5;
    }

    // Post to central record, protected by semaphore
    if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

      controllerData.temperatureMeasurementOK = allWentWell;
      controllerData.rawTemperature = measuredTemperature;

      xSemaphoreGive( controllerDataSemaphore );
    }

    //controllerData.logBusyTime.finish(btTemperature);  
}

void BoilerCommunicate() {
  userEventMessage_t message;
  float requestedBoilerTemperature,        actualBoilerTemperature;
  float requestedDomesticHotWaterSetpoint, actualDomesticHotWaterTemperature;
  float boilerPressure;
  bool enableCentralHeating,               centralHeatingActive;
  bool enableHotWater,                     hotWaterActive;
  bool enableCooling;
  bool boilerOnline;
  bool flameOn;
  boilerResponse_t boilerResponse;

  Serial.println("Boiler::update()");
  //controllerData.logBusyTime.start(btBoiler);  

  // Copy what we need from the controllerData, so we do not block the semaphore too long
  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {

    requestedDomesticHotWaterSetpoint = controllerData.requestedDomesticHotWaterSetpoint;
    enableCentralHeating              = controllerData.enableCentralHeating;
    enableHotWater                    = controllerData.enableHotWater;
    enableCooling                     = controllerData.enableCooling;
    requestedBoilerTemperature        = controllerData.requestedBoilerTemperature;

    xSemaphoreGive( controllerDataSemaphore );
  }else {
    Serial.println("Controller could not get access to controllerData before talking to boiler");
  };

  // Make arrows on boiler connect icon red
  message.sender  = sndBoiler;
  message.command = cmdBoilerSending;
  xQueueSend( displayQueue, &message, ( TickType_t ) 100 );  
  // We should wait for a response from the display before proceeding because we are about to kill task switching

  //controllerData.logBusyTime.start(btOpentherm);  
  
  // Switch keyboard off (just to make sure, potentially not required)
  disableKeyboard(); 

  bool prevKeyboardMode = keyboardMode;
  keyboardMode=true; // block Telegram

  // Stop switching tasks to ensure accurate timing of the OpenTherm protocol
  vTaskSuspendAll();

  unsigned long response = opentherm.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
  OpenThermResponseStatus responseStatus = opentherm.getLastResponseStatus();

  switch(responseStatus) {

    case OpenThermResponseStatus::NONE:
      boilerResponse       = boilerNone;
      boilerOnline         = false;
      break;

    case OpenThermResponseStatus::SUCCESS:
      boilerResponse       = boilerSuccess;
      boilerOnline         = true;
      centralHeatingActive = opentherm.isCentralHeatingActive(response);
      hotWaterActive       = opentherm.isHotWaterActive(response);
      flameOn              = opentherm.isFlameOn(response);
      break;

    case OpenThermResponseStatus::INVALID:
      boilerResponse       = boilerInvalid;
      boilerOnline         = false;
      break;

    case OpenThermResponseStatus::TIMEOUT:
      boilerResponse       = boilerTimeout;
      boilerOnline         = false;
      break;
    }

  //Set boiler temperature
  opentherm.setBoilerTemperature(requestedBoilerTemperature);

  //Get Boiler Temperature
  actualBoilerTemperature = opentherm.getBoilerTemperature();

  //Set domestic hot water setpoint
  opentherm.setDHWSetpoint(controllerData.requestedDomesticHotWaterSetpoint);
  
  //Get domestic hot water temperature
  actualDomesticHotWaterTemperature = opentherm.getDHWTemperature();

  //Get boile pressure
  boilerPressure = opentherm.getPressure();

  // OpenTherm communications done, resume switching tasks
  xTaskResumeAll();

  // Restore previous input mode
  keyboardMode=prevKeyboardMode;

  // Switch keyboard back on
  enableKeyboard(); 

  //controllerData.logBusyTime.finish(btOpentherm);  

  if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
    controllerData.boilerResponse = boilerResponse;
    controllerData.boilerOnline = boilerOnline;
    controllerData.actualBoilerTemperature = actualBoilerTemperature;
    controllerData.actualDomesticHotWaterTemperature = actualDomesticHotWaterTemperature;
    controllerData.boilerPressure = boilerPressure;
    controllerData.centralHeatingActive = centralHeatingActive;
    controllerData.hotWaterActive = hotWaterActive;
    controllerData.flameOn = flameOn;
    controllerData.dspShowerIcon = hotWaterActive ? iconShowerOn : iconShowerOff;
    controllerData.dspBoilerIcon = ( boilerResponse == boilerSuccess ) ? iconBoilerConnected : iconBoilerLost;

    if(!controllerData.boilerOnline) {
      controllerData.centralHeatingActive = false;
      controllerData.hotWaterActive       = false;
      controllerData.flameOn              = false;      
      controllerData.dspShowerIcon        = iconShowerOff;
    }

    xSemaphoreGive( controllerDataSemaphore );
  } else {
    Serial.println("Controller could not get access to controllerData after boiler response");
  }

  // Notify the display that communication with the boiler is completed
  message.sender  = sndBoiler;
  message.command = cmdUpdateScreen;  
  xQueueSend( displayQueue, &message, ( TickType_t ) 100 );  
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
    message = userEventMessage_t(sndThermometer, cmdControl);
    xQueueSend( controllerQueue, &message, ( TickType_t ) 10 );

    // Display temperature on screen
    message = userEventMessage_t(sndThermometer, cmdUpdateScreen);
    xQueueSend( displayQueue, &message, ( TickType_t ) 10 );

    lastTempMeas=millis();
  }

  if( millis() - lastBoilerConnect > BOILER_INTERVAL ) {
    BoilerCommunicate();
    lastBoilerConnect=millis();
  }

  if (millis() - lastReconnect > RECONNECT_INTERVAL) {
    if(!controllerData.temperatureMeasurementOK) TemperatureSensorConnect();
    lastTempMeas=millis();
  }

}
