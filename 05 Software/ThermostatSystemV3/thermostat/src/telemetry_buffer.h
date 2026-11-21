#pragma once
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>

namespace Telemetry {
enum Flags : uint8_t { RoomValid=1, StatusValid=2, HotWaterActive=4, HotWaterEnabled=8, HeatingEnabled=16 };
enum Kind : uint8_t { Sample, StatusChange, EnableChange };
struct Record {
  int64_t uptimeUs;
  uint32_t sequence, dropped;
  float room, setpoint, demandPercent, boilerSetpoint;
  uint8_t flags, kind;
};
// Deterministic timestamps are retained across retries, including after clock corrections.
struct Clock {
  int64_t lastMs=0;
  int64_t stamp(const Record &r, int64_t utcMs, int64_t uptimeUs) {
    int64_t candidate=utcMs-(uptimeUs-r.uptimeUs)/1000;
    if(candidate<=lastMs) candidate=lastMs+1;
    lastMs=candidate;
    return candidate;
  }
};
inline int format(char *out, size_t size, const Record &r, uint32_t boot, int64_t stamp) {
  char room[48]="", active[32]="";
  if((r.flags&RoomValid) && isfinite(r.room)) snprintf(room,sizeof(room),",room_temperature_c=%.3f",double(r.room));
  if(r.flags&StatusValid) snprintf(active,sizeof(active),",dhw_active=%s",r.flags&HotWaterActive?"true":"false");
  if(!isfinite(r.setpoint)||!isfinite(r.demandPercent)||!isfinite(r.boilerSetpoint)) return -1;
  return snprintf(out,size,
    "room_thermostat,device=room-thermostat kind=%ui,boot_id=%ui,sequence=%ui,uptime_ms=%lldi,dropped_records=%ui,room_valid=%s,status_valid=%s,dhw_enabled=%s,ch_enabled=%s,room_setpoint_c=%.3f,heat_demand_pct=%.3f,boiler_setpoint_c=%.3f%s%s %lld\n",
    unsigned(r.kind),unsigned(boot),unsigned(r.sequence),(long long)(r.uptimeUs/1000),unsigned(r.dropped),
    r.flags&RoomValid?"true":"false",r.flags&StatusValid?"true":"false",
    r.flags&HotWaterEnabled?"true":"false",r.flags&HeatingEnabled?"true":"false",
    double(r.setpoint),double(r.demandPercent),double(r.boilerSetpoint),room,active,(long long)stamp);
}
}

// Implemented after the control/sensor definitions. Called only on Core 1.
void telemetryObserveStatus(bool valid, bool active);
