#pragma once

#include "FS.h"
#include "SPIFFS.h"

#include "a_Constants.h"
#include "b_Buttons.h"
#include "c_Eventlog.h"
#include "d_Heaterprogram.h"

#define LOG_INTERVAL 60000

void newLogFile() {
  File file;
  
  disableKeyboard();

  file = SPIFFS.open(LOG_FILE, "w");
  file.print("Date\t");
  file.print("Time\t");
  file.print("P\t");
  file.print("I\t");
  file.print("D\t");
  file.print("ierr\t");
  file.print("derivative\t");
  file.print("Setpoint\t");
  file.print("measuredTemperature\t");
  file.print("boilerPercentage\t");
  file.print("requestedBoilerTemperature\t");
  file.print("actualBoilerTemperature\t");
  file.print("actualHotWaterTemperature\t");
  file.print("isCentralHeatingActive\t");
  file.print("isHotWaterActive\t");
  file.print("isFlameOn\t");
  file.print("Response\n");
  file.close();

  enableKeyboard();
};

void logIfNeeded(Data & data) {

  static unsigned long previousAttempt = 0;
  File file;
   
  if(millis() - previousAttempt > LOG_INTERVAL){
    previousAttempt=millis();

    Serial.print  (currentDate());
    Serial.print  ("\t");
    Serial.println(currentTime());
    
    disableKeyboard();

    file = SPIFFS.open(LOG_FILE, "r");
    if(!file || file.isDirectory()) newLogFile();
    file.close();
  
    file = SPIFFS.open(LOG_FILE, "a");  
    file.print(currentDate()); file.print("\t");
    file.print(currentTime()); file.print("\t");
    file.printf("%.3f\t", data.P);
    file.printf("%.3f\t", data.I);
    file.printf("%.3f\t", data.D);
    file.printf("%.3f\t", data.ierr);
    file.printf("%.3f\t", data.derivative);
    file.printf("%.3f\t", data.temperatureSetpoint);
    file.printf("%.3f\t", data.measuredTemperature);
    file.printf("%.3f\t", data.boilerPercentage);
    file.printf("%.3f\t", data.requestedBoilerTemperature);
    file.printf("%.3f\t", data.actualBoilerTemperature);
    file.printf("%.3f\t", data.actualDomesticHotWaterTemperature);
    file.printf("%s\t",   data.isCentralHeatingActive ? "Y" : "N");
    file.printf("%s\t",   data.isHotWaterActive       ? "Y" : "N");
    file.printf("%s\t",   data.isFlameOn              ? "Y" : "N");
    file.println(data.responseFromController);
    file.close();

    enableKeyboard();
  }
};

void deleteLogFile(){
  disableKeyboard();

  Serial.printf("Deleting file: %s\r\n", LOG_FILE);
  Serial.println(SPIFFS.remove(LOG_FILE) ? "- file deleted" : "- delete failed");

  deleteEventLog();
  newLogFile();
  newEventLog();

  enableKeyboard();
};

void sendLogFileIfNeeded() {
  static int incomingChar= 0;         // incoming serial byte
  File file;
  
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    incomingChar= Serial.read();

    disableKeyboard();

    file = SPIFFS.open(LOG_FILE);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("******************************************");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();

    file = SPIFFS.open(EVENT_LOG);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("******************************************");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
    Serial.println("******************************************");

    // Clear serial buffer
    while (Serial.available() > 0) {
      // get incoming byte:
      incomingChar= Serial.read();
    }

    enableKeyboard();
  }
};
