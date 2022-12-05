#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

#define CONTROLLER_INTERVAL 5000

#include "a_Constants.h"
#include "b_Buttons.h"
#include "d_Heaterprogram.h"


/* ======================================
   CONTROLLER CLASS DEFINITION
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
   GLOBAL OBJECTS
   ================================================= */
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress deviceAddress;
PID pidController;
OpenTherm ot(OT_IN_PIN, OT_OUT_PIN);

void ICACHE_RAM_ATTR handleInterrupt() {
  ot.handleInterrupt();
}


/* ================================================
   INIT SENSOR AND CONTROLLER
   ================================================ */
void startController() {

  // Initialize thermometer
  sensor.begin();
  Serial.printf("%d device(s) found on OneWire bus\n", sensor.getDeviceCount());
  Serial.printf("Parasite power on OneWire bus is: %s\n", sensor.isParasitePowerMode() ? "ON": "OFF");
  if (!sensor.getAddress(deviceAddress, 0)) {
    Serial.println("Unable to find address for device 0");
  }
  else {
    Serial.print("Dallas 18B20 found on address:\t");
    for (uint8_t i = 0; i < 8; i++) Serial.printf("%02X ", deviceAddress[i]);
    Serial.println();
  }

  // Set the therometer precision
  sensor.setResolution(deviceAddress, TEMPERATURE_PRECISION);
  Serial.printf("Resolution of sensor set to: %d\n",sensor.getResolution(deviceAddress));

  // Driver for opentherm shield
  ot.begin(handleInterrupt);
};


void controlTemperatureIfNeeded(Data & data) {
  float rawTemperature;

  static unsigned long previousAttempt = 0;

  if(millis() - previousAttempt > CONTROLLER_INTERVAL){
    previousAttempt=millis();
    
    // Check if the main loop has sent a message to the controller
    pidController.setpoint=data.temperatureSetpoint;
    
    // Measure temperture    
    sensor.requestTemperatures();
    rawTemperature = sensor.getTempC(deviceAddress);
    if(rawTemperature<-40) rawTemperature=20.5;
    data.measuredTemperature = rawTemperature+data.measuredTemperatureOffset;

    // Ask PID controller for new value
    data.requestedBoilerTemperature = pidController.newValue(data.measuredTemperature);

    // Switch off the heater if the room is too warm
    data.enableCentralHeating = (data.measuredTemperature<(data.temperatureSetpoint+2.0));

    // Send data back to main process
    data.boilerPercentage = pidController.boilerPercentage;
    data.P                = pidController.P;
    data.I                = pidController.I;
    data.D                = pidController.D;
    data.ierr             = pidController.ierr;
    data.derivative       = pidController.derivative;

    disableKeyboard();

    //unsigned long buildSetBoilerStatusRequest(bool enableCentralHeating, bool enableHotWater = false, bool enableCooling = false, 
    //   bool enableOutsideTemperatureCompensation = false, bool enableCentralHeating2 = false);

    unsigned long response = ot.setBoilerStatus(data.enableCentralHeating, true, false);
    OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
    data.responseFromController = String(ot.statusToString(responseStatus));
    Serial.print("Status: " + data.responseFromController+" ");

    if (responseStatus == OpenThermResponseStatus::SUCCESS) {
        data.isCentralHeatingActive = ot.isCentralHeatingActive(response);
        data.isHotWaterActive       = ot.isHotWaterActive(response);
        data.isFlameOn              = ot.isFlameOn(response);
        Serial.print("Central Heating: " + String(data.isCentralHeatingActive ? "on " : "off "));
        Serial.print("Hot Water: "       + String(data.isHotWaterActive       ? "on " : "off "));
        Serial.print("Flame: "           + String(data.isFlameOn              ? "on " : "off "));
    }
    if (responseStatus == OpenThermResponseStatus::NONE) {
        Serial.print("Error: OpenTherm is not initialized");
    }
    else if (responseStatus == OpenThermResponseStatus::INVALID) {
        Serial.print("Error: Invalid response " + String(response, HEX));
    }
    else if (responseStatus == OpenThermResponseStatus::TIMEOUT) {
        Serial.print("Error: Response timeout");
    }

    //Set Boiler Temperature to 55 degrees C
    ot.setBoilerTemperature(data.requestedBoilerTemperature);

    //Get Boiler Temperature
    data.actualBoilerTemperature = ot.getBoilerTemperature();
    Serial.print(" boiler temperature is " + String(data.actualBoilerTemperature) + "°C");

    //Set DHW setpoint to 55°C
    ot.setDHWSetpoint(data.domesticHotWaterSetpoint);

    //Get DHW Temperature
    data.actualDomesticHotWaterTemperature = ot.getDHWTemperature();
    Serial.println(" hot water temperature " + String(data.actualDomesticHotWaterTemperature) + "°C");

    enableKeyboard();
  }; // millis - previousAttempt ...
};
