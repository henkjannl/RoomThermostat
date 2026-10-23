#pragma once

#include <WiFi.h> 
#include <WiFiMulti.h>
#include "a_Constants.h"
#include "d_Heaterprogram.h"

WiFiMulti wifiMulti;

const unsigned long WIFI_RECONNECT_INTERVAL_MILLIS = 3*60*1000;
const unsigned long WIFI_MAX_TIME_UNCONNECTED_MILLIS = 15*60*1000;

void setupWifi(Data & data) {

  for(auto accessPoint : data.AccessPoints) {
    wifiMulti.addAP(accessPoint.ssid, accessPoint.password);
    Serial.println(accessPoint.ssid);
  }

  wifiMulti.run();

  data.wifiConnected = (WiFi.status() == WL_CONNECTED);
  
  if(data.wifiConnected){
    for(auto accessPoint : data.AccessPoints) {
      if(WiFi.SSID()==accessPoint.ssid) {
        data.locationKnown=true;            
        strcpy(data.timezone, accessPoint.timezone);
      }
    }
  }

  // If network is available, sync what's needed
  //requestTimeZone(data);
  //syncTime(data);
}

void connectToWifi(Data & data) {
  // Check location based on WiFi ssid
  if((wifiMulti.run() != WL_CONNECTED)) {
    for(auto accessPoint : data.AccessPoints) {
      if ( WiFi.SSID().compareTo(String(accessPoint.ssid))==0) {
        Serial.printf("Access point: %s Connected to: %s\n", accessPoint.ssid, WiFi.SSID());
        data.locationKnown=true;            
        strcpy(data.timezone, accessPoint.timezone);
        break;
      }
    }
  }  
}

void reconnectIfNeeded(Data & data) {
  static unsigned long previousAttempt = millis()-WIFI_RECONNECT_INTERVAL_MILLIS;
  static unsigned long lastTimeConnected = millis();
  
  if((wifiMulti.run() != WL_CONNECTED)) {
    if(millis() - previousAttempt > WIFI_RECONNECT_INTERVAL_MILLIS){
      previousAttempt=millis();
      connectToWifi(data);
    }
    if(millis() - lastTimeConnected > WIFI_MAX_TIME_UNCONNECTED_MILLIS){
      logMessage("Restart due to WiFi timeout");
      esp_restart();
    }
  }
  else {
    lastTimeConnected = millis();
  }
}
  
