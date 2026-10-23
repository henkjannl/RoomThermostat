#pragma once

#include <WiFi.h> 
#include <WiFiMulti.h>
#include "a_Constants.h"

WiFiMulti wifiMulti;

const int WIFI_RECONNECT_INTERVAL_MILLIS = 60*5*1000;


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

void reconnectIfNeeded(Data & data) {
  static unsigned long previousAttempt = millis()-WIFI_RECONNECT_INTERVAL_MILLIS;
  
  if((wifiMulti.run() != WL_CONNECTED)) {
    if(millis() - previousAttempt > WIFI_RECONNECT_INTERVAL_MILLIS){
      previousAttempt=millis();
    }
  }
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
