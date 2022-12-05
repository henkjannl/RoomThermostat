#pragma once

#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

void syncTimeIfNeeded(DataBus & databus) {
  static bool firstTime=true;
  static int resyncTimeInterval = 15*1000; // Until synched, attempt to resync time every 15 seconds
  static unsigned long previousAttempt = 0;

  if( !wifiConnected()) return;

  databus.logBusyTime.start(btTimeSynch);  
  
  if ( firstTime or (millis() - previousAttempt > resyncTimeInterval) ){
    firstTime=false;
    previousAttempt=millis();    
    Serial.println("syncTimeIfNeeded");
    const char* ntpServer = "pool.ntp.org";
    configTime(databus.timeZoneOffset, databus.timeDSToffset, ntpServer);

    time_t now;
    time(&now); // Get current time
    struct tm * local;
    local = localtime(&now);
    
    databus.timeSynched =(local->tm_year > 70);
    
    if (databus.timeSynched) {
      Serial.printf("Time synched: %02d:%02d:%02d\n", local->tm_hour, local->tm_min, local->tm_sec);
      resyncTimeInterval = 24*60*1000; // If synched, increase resync interval
    };
  };

  databus.logBusyTime.finish(btTimeSynch);  
};

void requestTimeZoneIfNeeded(DataBus & databus) {
  static bool firstTime=true;
  static unsigned long resyncTimeInterval = 60*1000; // Until synched, attempt to resync time at the whole hour
  static unsigned long previousAttempt = 0;
  
  if (wifiConnected() and ((millis() - previousAttempt > resyncTimeInterval) or firstTime) ) {

    previousAttempt=millis();    
    time_t now;
    time(&now); // Get current time
    struct tm * local;
    local = localtime(&now);
    bool wholeHour =(local->tm_min ==0);

    if (firstTime or wholeHour ) {
  
      disableKeyboard();
  
      previousAttempt=millis();    
      Serial.println("requestTimeZoneIfNeeded");
  
      char request[200];
      snprintf(request, 200, "http://worldtimeapi.org/api/timezone/%s",databus.timezone);      
      Serial.println(F("Requesting time zone and daylight saving time"));
      Serial.printf("Time zone [%s], request [%s]\n", databus.timezone, request);
      
      HTTPClient http;
      http.begin(request); 
      //http.begin(F("http://worldtimeapi.org/api/timezone/Europe/Amsterdam")); 
  
      int httpCode = http.GET();
  
      if(httpCode==HTTP_CODE_OK) {   
        StaticJsonDocument<1024> doc;
        DeserializationError error;
        error = deserializeJson(doc, http.getString());
  
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          Serial.println(http.getString());
          return;
        }
  
        //const char* abbreviation = doc["abbreviation"]; // "CEST"
        //const char* client_ip = doc["client_ip"]; // "80.100.167.45"
        //const char* datetime = doc["datetime"]; // "2021-04-05T14:28:36.119148+02:00"
        //int day_of_week = doc["day_of_week"]; // 1
        //int day_of_year = doc["day_of_year"]; // 95
        //bool dst = doc["dst"]; // true
        //const char* dst_from = doc["dst_from"]; // "2021-03-28T01:00:00+00:00"
        databus.timeDSToffset = doc["dst_offset"]; // 3600
        //const char* dst_until = doc["dst_until"]; // "2021-10-31T01:00:00+00:00"
        databus.timeZoneOffset = doc["raw_offset"]; // 3600
        //const char* timezone = doc["timezone"]; // "Europe/Amsterdam"
        //long unixtime = doc["unixtime"]; // 1617625716
        //const char* utc_datetime = doc["utc_datetime"]; // "2021-04-05T12:28:36.119148+00:00"
        //const char* utc_offset = doc["utc_offset"]; // "+02:00"
        //int week_number = doc["week_number"]; // 14
    
        Serial.println(F("Time zone difference retrieved"));
        firstTime=false;
      } // httpCode==HTTP_CODE_OK
      else {
        Serial.printf("Unexpected httpCode: %d (expected %d)\n", httpCode, HTTP_CODE_OK);
        Serial.printf("Return string [%s]\n",http.getString());
      }; // other httpCode
    
    enableKeyboard();
    };
  };
};
