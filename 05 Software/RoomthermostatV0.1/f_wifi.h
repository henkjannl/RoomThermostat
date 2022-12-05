#pragma once

#include <WiFi.h> 
#include <WiFiMulti.h>

const unsigned long WIFI_RECONNECT_INTERVAL_MILLIS   = 1*60*1000; // Re-attempt to connect
const unsigned long WIFI_MAX_TIME_UNCONNECTED_MILLIS = 5*60*1000; // Restart ESP if unsuccessful

WiFiMulti wifiMulti;

bool wifiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void setupWifi(DataBus & databus) {

  for(auto accessPoint : databus.AccessPoints) {
    wifiMulti.addAP(accessPoint.ssid, accessPoint.password);
    Serial.println(accessPoint.ssid);
  }

  wifiMulti.run();
  
  if(wifiConnected()){
    for(auto accessPoint : databus.AccessPoints) {
      if(WiFi.SSID()==accessPoint.ssid) {
        databus.locationKnown=true;            
        strcpy(databus.timezone, accessPoint.timezone);
        Serial.printf("WiFi connected to %s\n", accessPoint.ssid);
      }
    }
  }
}

void connectToWifi(DataBus & databus) {
  // Check location based on WiFi ssid
  if( !wifiConnected() ) {
    for(auto accessPoint : databus.AccessPoints) {
      if ( WiFi.SSID().compareTo(String(accessPoint.ssid))==0) {
        Serial.printf("Access point: %s Connected to: %s\n", accessPoint.ssid, WiFi.SSID());
        databus.locationKnown=true;            
        strcpy(databus.timezone, accessPoint.timezone);
        break;
      }
    }
  }  
}

void reconnectIfNeeded(DataBus & databus) {
  static unsigned long previousReconnect = millis()-WIFI_RECONNECT_INTERVAL_MILLIS;
  static unsigned long lastTimeConnected = millis();

  bool connected = wifiConnected();
  databus.dpWifiIcon.setValue(connected ? iconWifiConnected : iconWifiLost );

  if (connected) {
    lastTimeConnected = millis();
    return;
  }

  databus.logBusyTime.start(btWifi);  

  if(millis() - previousReconnect > WIFI_RECONNECT_INTERVAL_MILLIS){
    previousReconnect=millis();
    Serial.println("reconnectIfNeeded");
    connectToWifi(databus);
  }

  if(millis() - lastTimeConnected > WIFI_MAX_TIME_UNCONNECTED_MILLIS){
    logMessage("Restart due to WiFi timeout");
    esp_restart();
  }

  databus.logBusyTime.finish(btWifi);  
}
