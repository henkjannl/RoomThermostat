#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_NUM_SENSORS 4

class TemperatureSensor {
  public:
    TemperatureSensor(DallasTemperature & dallasSensor, DataBus & databus);
    void begin();
    void loop();
    void measure();
    void setOffset(float offset);
    void increaseSensorOffset();
    void decreaseSensorOffset();
  
  protected:  
    DallasTemperature *dallasSensor;
    DeviceAddress sensorAddress[MAX_NUM_SENSORS];
    DataBus *databus;
    uint8_t numSensors;
};

TemperatureSensor::TemperatureSensor(DallasTemperature & dallasSensor, DataBus & databus) {
  this->dallasSensor = &dallasSensor;
  this->databus= &databus;
};  
  
void TemperatureSensor::begin() {

  // Initialize thermometer
  dallasSensor->begin();
  numSensors=dallasSensor->getDeviceCount();
  Serial.printf("%d device(s) found on OneWire bus\n", numSensors);

  for(int i=0; i<numSensors; i++) {

    if ( (i<MAX_NUM_SENSORS) and dallasSensor->getAddress(sensorAddress[i], i) ) {
      
        Serial.print("Dallas 18B20 found on address:\t");
        for (uint8_t j = 0; j < 8; j++) Serial.printf("%02X ", sensorAddress[i][j]);
        Serial.println();
  
        // Set the therometer precision
        dallasSensor->setResolution(sensorAddress[i], TEMPERATURE_PRECISION);
        Serial.printf("Resolution of sensor set to: %d\n",dallasSensor->getResolution(sensorAddress[i]));
      }  
  }
    
  Serial.printf("Parasite power on OneWire bus is: %s\n", dallasSensor->isParasitePowerMode() ? "ON": "OFF");
  databus->roomTemperatureStatusOK = (numSensors>0);

  // Do the first temperature measurement
  measure();
};

void TemperatureSensor::loop() {
  static unsigned long previousAttempt = 0;

  // Reconnect if a bad reading has occurred
  if (millis() - previousAttempt > 15*1000) {
    previousAttempt=millis();    
    if ( !databus->roomTemperatureStatusOK) begin();
  }
};

void TemperatureSensor::measure() {
  char buffer[10];
  float tmp;
  float sum = 0;
  int cnt = 0;

  databus->logBusyTime.start(btTemperature);
  
  dallasSensor->requestTemperatures();

  for (int i=0; i<numSensors; i++) {
    tmp=dallasSensor->getTempC(sensorAddress[i]);
    if(tmp!=DEVICE_DISCONNECTED_C) {
      sum+=tmp;
      cnt++;
      //Serial.printf("Sensor %d tmp %.1f ", i, tmp);
    }
    else Serial.printf("Sensor %d not connected ", i);
  };

  // We're happy if at least one sensor is left
  databus->roomTemperatureStatusOK = (cnt>0);

  databus->measuredTemperature = databus->roomTemperatureStatusOK ? sum/cnt+databus->temperatureSensorOffset : 20.5;

  if(databus->measuredTemperature < -40) {
    databus->roomTemperatureStatusOK=false;
    databus->measuredTemperature = 20.5;
  }

  if(databus->roomTemperatureStatusOK) {
    sprintf(buffer, "%.1f", databus->measuredTemperature);
    databus->dpMeasuredTemperature.setValue(buffer);
  }
  else
    databus->dpMeasuredTemperature.setValue("--");

  databus->logBusyTime.finish(btTemperature);  
}

void TemperatureSensor::setOffset(float offset) {
  databus->temperatureSensorOffset = offset;
  measure();
}

void TemperatureSensor::increaseSensorOffset() {
  databus->temperatureSensorOffset+=0.1;
  measure();
}

void TemperatureSensor::decreaseSensorOffset() {
  databus->temperatureSensorOffset-=0.1;
  measure();
}
