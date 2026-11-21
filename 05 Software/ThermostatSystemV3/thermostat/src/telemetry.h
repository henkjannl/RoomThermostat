#pragma once
#include "telemetry_buffer.h"
#include <HTTPClient.h>
#include <esp_timer.h>
#include <sys/time.h>
#if __has_include("influx_config.h") && !defined(THERMOSTAT_INFLUX_ENABLED)
#include "influx_config.h"
#endif
#ifndef THERMOSTAT_INFLUX_ENABLED
#define THERMOSTAT_INFLUX_ENABLED 0
#endif
#ifndef THERMOSTAT_INFLUX_URL
#define THERMOSTAT_INFLUX_URL ""
#endif
#ifndef THERMOSTAT_INFLUX_TOKEN
#define THERMOSTAT_INFLUX_TOKEN ""
#endif

namespace Telemetry {
constexpr size_t Capacity=128, BatchSize=4;
QueueHandle_t records=nullptr; // Created before worker startup, immutable afterwards.
uint32_t sequence=0, dropped=0, boot=0; // Core 1 owns sequence/dropped.
int8_t status=-1;
bool haveStatus=false;
uint32_t statusAt=0;

void begin() {
  if(!THERMOSTAT_INFLUX_ENABLED || !strlen(THERMOSTAT_INFLUX_TOKEN)) {
    Serial.println("Influx logging disabled (missing local configuration/token)");
    return;
  }
  boot=esp_random();
  records=xQueueCreate(Capacity,sizeof(Record));
  Serial.printf("Influx RAM logging %s; capacity=%u record-bytes=%u\n",records?"ready":"unavailable",unsigned(Capacity),unsigned(sizeof(Record)));
}
void capture(Kind kind) {
  if(!records) return;
  Record r{};
  r.uptimeUs=esp_timer_get_time(); r.sequence=++sequence;
  r.room=controllerData.measuredTemperature(); r.setpoint=controllerData.temperatureSetpoint;
  r.demandPercent=controllerData.boilerPercentage*100.0f;
  r.boilerSetpoint=controllerData.requestedBoilerTemperature;
  r.flags=(roomTemperatureValid()?RoomValid:0) | (status>=0?StatusValid:0) |
    (status==1?HotWaterActive:0) | (controllerData.enableHotWater?HotWaterEnabled:0) |
    (controllerData.enableCentralHeating?HeatingEnabled:0);
  r.kind=kind; r.dropped=dropped;
  if(xQueueSend(records,&r,0)==pdPASS) return;
  Record discarded;
  // Core 0 may have drained the queue meanwhile; all operations remain zero-wait.
  if(xQueueReceive(records,&discarded,0)==pdPASS) ++dropped;
  r.dropped=dropped;
  if(xQueueSend(records,&r,0)!=pdPASS) ++dropped;
}
void serviceControl() {
  if(!records) return;
  if(status>=0 && uint32_t(millis()-statusAt)>15000) {
    status=-1; capture(StatusChange);
  }
  static bool initialized=false, enabled=false;
  bool current=controllerData.enableHotWater;
  if(!initialized || current!=enabled) {
    initialized=true; enabled=current; capture(EnableChange);
  }
  static int64_t lastSample=-10000000LL;
  int64_t now=esp_timer_get_time();
  if(now-lastSample>=10000000LL) {lastSample=now;capture(Sample);}
}

// Core 0 only: owns HTTP, pending batch and retry state. No flash or control access.
void serviceWorker(bool connected) {
  if(!records || !connected) return;
  static uint32_t lastAttempt=0, waitMs=30000, failures=0, writes=0;
  uint32_t now=millis();
  if(uint32_t(now-lastAttempt)<waitMs) return;
  timeval tv; gettimeofday(&tv,nullptr);
  if(tv.tv_sec<1704067200) return; // No fabricated calendar dates before first NTP sync.
  static char body[2048];
  static size_t used=0, pending=0;
  static Clock clock;
  if(!pending) {
    int64_t utcMs=int64_t(tv.tv_sec)*1000+tv.tv_usec/1000;
    int64_t uptimeUs=esp_timer_get_time();
    for(size_t i=0;i<BatchSize;++i) {
      Record r;
      if(xQueueReceive(records,&r,0)!=pdPASS) break;
      int count=format(body+used,sizeof(body)-used,r,boot,clock.stamp(r,utcMs,uptimeUs));
      if(count<=0 || size_t(count)>=sizeof(body)-used) {
        Serial.println("Influx record formatting failed; record discarded");
        continue;
      }
      used+=size_t(count); ++pending;
    }
  }
  if(!pending) return;
  lastAttempt=now;
  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(1000); http.setTimeout(1000);
  http.setReuse(false);
  http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
  int code=-1;
  if(http.begin(client,THERMOSTAT_INFLUX_URL)) {
    http.addHeader("Authorization",String("Token ")+THERMOSTAT_INFLUX_TOKEN);
    http.addHeader("Content-Type","text/plain; charset=utf-8");
    code=http.POST(reinterpret_cast<uint8_t*>(body),used);
    http.end();
  }
  if(code==204) {
    writes+=pending; pending=0; used=0; failures=0;
    waitMs=uxQueueMessagesWaiting(records)?1000:30000;
  } else {
    ++failures;
    waitMs=failures<7?(5000UL<<(failures-1)):300000UL;
    // Retain exactly the same bytes/timestamps until acknowledged. Do not print tokens/body.
    Serial.printf("Influx write failed status=%d retry-ms=%lu pending=%u\n",code,(unsigned long)waitMs,unsigned(pending));
  }
  if(code==204) Serial.printf("Influx acknowledged=%lu queued=%u\n",(unsigned long)writes,unsigned(uxQueueMessagesWaiting(records)));
}
}

void telemetryObserveStatus(bool valid, bool active) {
  int8_t next=valid?(active?1:0):-1;
  if(valid) Telemetry::statusAt=millis();
  bool changed=!Telemetry::haveStatus || next!=Telemetry::status;
  Telemetry::haveStatus=true; Telemetry::status=next;
  if(changed) Telemetry::capture(Telemetry::StatusChange);
}
