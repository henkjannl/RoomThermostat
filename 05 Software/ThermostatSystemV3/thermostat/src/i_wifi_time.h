#pragma once
#include <WiFi.h>
#include <esp_wifi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "time.h"
#include "timezone_rules.h"

WiFiMulti wifiMulti;
bool wifiConnected() { return WiFi.status() == WL_CONNECTED; }

// Core 0 only. Check the actual driver state, not WiFi.getSleep()'s cached preference.
void serviceReceiverWifi() {
  static uint32_t lastCheck=millis()-10000UL;
  static int previousStatus=-1,previousChannel=-1;
  const int status=WiFi.status();
  const int channel=WiFi.channel();
  if(status!=previousStatus||channel!=previousChannel){
    Serial.printf("Receiver WiFi uptime=%lu status=%d channel=%d rssi=%d\n",millis(),status,channel,WiFi.RSSI());
    previousStatus=status;previousChannel=channel;
    lastCheck=millis()-10000UL;
  }
  if(millis()-lastCheck<10000)return;
  lastCheck=millis();wifi_ps_type_t powerSave;
  if(esp_wifi_get_ps(&powerSave)==ESP_OK && powerSave!=WIFI_PS_NONE){
    esp_err_t result=esp_wifi_set_ps(WIFI_PS_NONE);
    Serial.printf("Receiver WiFi corrected power-save=%d result=%d\n",int(powerSave),int(result));
  }
}

void setupWifi(ControllerData_t &data) {
  WiFi.persistent(false); // Network worker must never persist Wi-Fi configuration.
  WiFi.mode(WIFI_STA); // Persistence must be selected before driver initialization.
  IPAddress dns1(1,1,1,1), dns2(8,8,8,8);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, dns1, dns2);
  for (const auto &ap : data.AccessPoints) wifiMulti.addAP(ap.ssid, ap.password);
}

void reconnectWiFiIfNeeded(ControllerData_t &data) {
  static unsigned long lastAttempt = millis() - 10000UL;
  if (!wifiConnected() && millis() - lastAttempt >= 10000) {
    lastAttempt = millis();
    wifiMulti.run(3000); // Blocks this worker only. Never reboot control for a network outage.
  }
  if (wifiConnected()) {
    for (const auto &ap : data.AccessPoints) {
      if (WiFi.SSID() == ap.ssid) {
        data.locationKnown = true;
        strlcpy(data.timezone, ap.timezone, sizeof(data.timezone));
        break;
      }
    }
  }
}

void syncTimeIfNeeded(ControllerData_t &data) {
  static unsigned long lastAttempt = millis() - 15000UL;
  static int previousOffset = INT32_MIN;
  static char previousZone[64] = "";
  bool amsterdam = strcmp(data.timezone, "Europe/Amsterdam") == 0;
  bool zoneChanged = strcmp(previousZone, data.timezone) != 0;
  if (zoneChanged) {
    strlcpy(previousZone, data.timezone, sizeof(previousZone));
    if (amsterdam) {
      setenv("TZ", AMSTERDAM_TZ, 1);
      tzset(); // Local rules apply even without a network connection.
    }
  }
  time_t now = time(nullptr);
  data.timeSynched = now > 1609459200;
  if (amsterdam) {
    struct tm local;
    localtime_r(&now, &local);
    data.timeZoneOffset = 3600;
    data.timeDSToffset = data.timeSynched && local.tm_isdst > 0 ? 3600 : 0;
  }
  int offset = data.timeZoneOffset + data.timeDSToffset;
  unsigned long interval = data.timeSynched ? 24UL*60*60*1000 : 15000UL;
  if (wifiConnected() && (zoneChanged || previousOffset == INT32_MIN ||
      (!amsterdam && offset != previousOffset) || millis() - lastAttempt >= interval)) {
    lastAttempt = millis();
    previousOffset = offset;
    if (amsterdam) configTzTime(AMSTERDAM_TZ, "pool.ntp.org");
    else configTime(data.timeZoneOffset, data.timeDSToffset, "pool.ntp.org");
  }
}

void requestTimeZoneIfNeeded(ControllerData_t &data) {
  if (strcmp(data.timezone, "Europe/Amsterdam") == 0) return;
  static unsigned long lastAttempt = millis() - 60000UL;
  static bool retrieved = false;
  if (!wifiConnected() || millis() - lastAttempt < (retrieved ? 3600000UL : 60000UL)) return;
  lastAttempt = millis();
  char url[200];
  snprintf(url, sizeof(url), "http://worldtimeapi.org/api/timezone/%s", data.timezone);
  HTTPClient http;
  http.setConnectTimeout(3000);
  http.setTimeout(3000);
  if (http.begin(url) && http.GET() == HTTP_CODE_OK) {
    StaticJsonDocument<1024> doc;
    if (!deserializeJson(doc, http.getString()) && doc["raw_offset"].is<int>() && doc["dst_offset"].is<int>()) {
      data.timeZoneOffset = doc["raw_offset"];
      data.timeDSToffset = doc["dst_offset"];
      retrieved = true;
    }
  }
  http.end();
}
