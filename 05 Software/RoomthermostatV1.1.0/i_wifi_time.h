#pragma once

#include <WiFi.h> 
#include <WiFiMulti.h>
#include "time.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

const unsigned long WIFI_RECONNECT_INTERVAL_MILLIS   = 1*60*1000; // Re-attempt to connect
const unsigned long WIFI_MAX_TIME_UNCONNECTED_MILLIS = 5*60*1000; // Restart ESP if unsuccessful

WiFiMulti wifiMulti;

bool wifiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void setupWifi(ControllerData_t & controllerData) {

  for(auto accessPoint : controllerData.AccessPoints) {
    wifiMulti.addAP(accessPoint.ssid, accessPoint.password);
    //Serial.println(accessPoint.ssid);
  }

  wifiMulti.run();
  
  if(wifiConnected()){
    for(auto accessPoint : controllerData.AccessPoints) {
      if(WiFi.SSID()==accessPoint.ssid) {
        controllerData.locationKnown=true;            
        strcpy(controllerData.timezone, accessPoint.timezone);
        Serial.printf("WiFi connected to %s\n", accessPoint.ssid);
      }
    }
  }

}

void connectToWifi(ControllerData_t & controllerData) {
  // Check location based on WiFi ssid
  if( !wifiConnected() ) {
    for(auto accessPoint : controllerData.AccessPoints) {
      if ( WiFi.SSID().compareTo(String(accessPoint.ssid))==0) {
        Serial.printf("Access point: %s Connected to: %s\n", accessPoint.ssid, WiFi.SSID());
        controllerData.locationKnown=true;            
        strcpy(controllerData.timezone, accessPoint.timezone);
        break;
      }
    }
  }  
}

void reconnectWiFiIfNeeded(ControllerData_t & controllerData) {
  static unsigned long previousReconnect = millis()-WIFI_RECONNECT_INTERVAL_MILLIS;
  static unsigned long lastTimeConnected = millis();
  static bool prevConnect = false;
  userEventMessage_t message;

  bool connected = wifiConnected();
  controllerData.dspWifiIcon = connected ? iconWifiConnected : iconWifiLost;

  if(connected != prevConnect) {
    prevConnect = connected;

    // Notify to the display that wifi icon was changed 
    sendMessage(sndWifi, cmdSilentUpdate, displayQueue);
  }
  
  if (connected) {
    lastTimeConnected = millis();
    previousReconnect = millis();
    return;
  }

  if(millis() - previousReconnect > WIFI_RECONNECT_INTERVAL_MILLIS){
    previousReconnect=millis();
    Serial.println("reconnectIfNeeded");
    connectToWifi(controllerData);
  }

  if(millis() - lastTimeConnected > WIFI_MAX_TIME_UNCONNECTED_MILLIS){
    logMessage("Restart due to WiFi timeout");
    esp_restart();
  }
}

void syncTimeIfNeeded(ControllerData_t & controllerData) {
  static bool firstTime=true;
  static int resyncTimeInterval = 15*1000; // Until synched, attempt to resync time every 15 seconds
  static unsigned long previousAttempt = 0;
  userEventMessage_t message;

  if( !wifiConnected()) return;

  //controllerData.logBusyTime.start(btTimeSynch);  
  
  if ( firstTime or (millis() - previousAttempt > resyncTimeInterval) ){
    firstTime=false;
    previousAttempt=millis();    
    Serial.println("syncTimeIfNeeded");
    const char* ntpServer = "pool.ntp.org";
    configTime(controllerData.timeZoneOffset, controllerData.timeDSToffset, ntpServer);

    time_t now;
    time(&now); // Get current time
    struct tm * local;
    local = localtime(&now);
    
    controllerData.timeSynched =(local->tm_year > 70);
    
    if (controllerData.timeSynched) {
      Serial.printf("Time synched: %02d:%02d:%02d\n", local->tm_hour, local->tm_min, local->tm_sec);
      resyncTimeInterval = 24*60*1000; // If synched, increase resync interval

      // Notify to the display that the time was synched
      sendMessage(sndWifi, cmdSilentUpdate, controllerQueue);
    };
  };

  //controllerData.logBusyTime.finish(btTimeSynch);  
};

void requestTimeZoneIfNeeded(ControllerData_t & controllerData) {
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
      snprintf(request, 200, "http://worldtimeapi.org/api/timezone/%s",controllerData.timezone);      
      Serial.println(F("Requesting time zone and daylight saving time"));
      Serial.printf("Time zone [%s], request [%s]\n", controllerData.timezone, request);
      
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
        controllerData.timeDSToffset = doc["dst_offset"]; // 3600
        //const char* dst_until = doc["dst_until"]; // "2021-10-31T01:00:00+00:00"
        controllerData.timeZoneOffset = doc["raw_offset"]; // 3600
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
