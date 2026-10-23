#pragma once
#include <LittleFS.h>
#include <esp_partition.h>
#include <sys/time.h>

struct __attribute__((packed)) BatteryLogRecord {
  uint32_t magic, sequence, session;
  int64_t seconds;
  uint32_t awakeMs;
  uint16_t millivolts;
  uint8_t percent, flags; // bit0 battery-only; bit1 valid voltage; bit2 connected; bit3 rise
  uint32_t checksum;
};
static_assert(sizeof(BatteryLogRecord)==32,"Battery log schema");
constexpr uint32_t LOG_MAGIC=0x33425431;
constexpr unsigned LOG_CAPACITY=2048, LOG_BATCH=8;
RTC_DATA_ATTR uint32_t logRtcMagic=0, logSession=0, logNext=1;
RTC_DATA_ATTR int64_t lastLogSeconds=0, lastFlushSeconds=0;
RTC_DATA_ATTR uint16_t previousVoltage=0;
RTC_DATA_ATTR uint8_t previousLogFlags=0, pendingRecords=0;
RTC_DATA_ATTR BatteryLogRecord pendingLog[LOG_BATCH];
RTC_DATA_ATTR bool batteryLogOK=true;
bool logMounted=false, coldLogStart=false;

uint32_t logChecksum(const BatteryLogRecord &r) {
  const uint8_t *bytes=reinterpret_cast<const uint8_t*>(&r);
  uint32_t crc=0xFFFFFFFF;
  for (size_t i=0;i<offsetof(BatteryLogRecord,checksum);++i) {
    crc^=bytes[i];
    for (int j=0;j<8;++j) crc=(crc>>1)^(0xEDB88320U & (0U-(crc&1)));
  }
  return ~crc;
}
bool mountBatteryLog() {
  if (logMounted) return true;
  if (LittleFS.begin(false)) { logMounted=true; return true; }
  // Only auto-format a demonstrably erased partition, never an unknown filesystem.
  const esp_partition_t *part=esp_partition_find_first(ESP_PARTITION_TYPE_DATA,ESP_PARTITION_SUBTYPE_DATA_SPIFFS,nullptr);
  uint8_t block[256];
  bool erased=part!=nullptr;
  for (size_t offset=0;erased && offset<part->size;offset+=sizeof(block)) {
    if (esp_partition_read(part,offset,block,sizeof(block))!=ESP_OK) {erased=false;break;}
    for (uint8_t byte:block) if (byte!=255) {erased=false;break;}
    delay(0);
  }
  if (erased && LittleFS.begin(true)) {logMounted=true;return true;}
  batteryLogOK=false; return false;
}
void beginBatteryLog() {
  bool continuing=esp_reset_reason()==ESP_RST_DEEPSLEEP && logRtcMagic==LOG_MAGIC && pendingRecords<=LOG_BATCH;
  if (continuing) return;
  coldLogStart=true; pendingRecords=0;logNext=1;logSession=esp_random();
  lastLogSeconds=lastFlushSeconds=0;previousVoltage=0;previousLogFlags=0;
  if (mountBatteryLog() && LittleFS.exists("/battery-v1.bin")) {
    File file=LittleFS.open("/battery-v1.bin","r");
    BatteryLogRecord r;
    for (unsigned i=0;file && i<LOG_CAPACITY && file.read(reinterpret_cast<uint8_t*>(&r),sizeof(r))==sizeof(r);++i) {
      if (r.magic==LOG_MAGIC && r.checksum==logChecksum(r) && r.sequence>=logNext && r.sequence<UINT32_MAX)
        logNext=r.sequence+1;
    }
    file.close();
  }
  logRtcMagic=LOG_MAGIC;
}
bool flushBatteryLog(int64_t now) {
  if (!pendingRecords) return true;
  if (!mountBatteryLog()) return false;
  File file=LittleFS.open("/battery-v1.bin",LittleFS.exists("/battery-v1.bin")?"r+":"w+");
  if (!file) {batteryLogOK=false;return false;}
  unsigned committed=0;
  while (committed<pendingRecords) {
    const BatteryLogRecord &r=pendingLog[committed];
    if (!file.seek(((r.sequence-1)%LOG_CAPACITY)*sizeof(r)) ||
        file.write(reinterpret_cast<const uint8_t*>(&r),sizeof(r))!=sizeof(r)) break;
    ++committed;
  }
  file.flush();file.close();
  if (committed) {
    memmove(pendingLog,pendingLog+committed,(pendingRecords-committed)*sizeof(BatteryLogRecord));
    pendingRecords-=committed;
  }
  batteryLogOK=pendingRecords==0;
  if (batteryLogOK) lastFlushSeconds=now;
  return batteryLogOK;
}
void recordBattery(uint16_t mv, uint8_t percent, bool valid, bool connected, uint32_t awakeMs) {
  timeval now;gettimeofday(&now,nullptr);
  uint8_t flags=(BATTERY_ONLY?1:0)|(valid?2:0)|(connected?4:0);
  bool rise=previousVoltage && mv>previousVoltage+50;
  bool event=coldLogStart || rise || ((flags^previousLogFlags)&7);
  if (!event && now.tv_sec-lastLogSeconds<900) return;
  if (pendingRecords==LOG_BATCH && !flushBatteryLog(now.tv_sec)) return;
  if (logNext==UINT32_MAX) {batteryLogOK=false;return;}
  BatteryLogRecord r{LOG_MAGIC,logNext++,logSession,now.tv_sec,awakeMs,mv,percent,uint8_t(flags|(rise?8:0)),0};
  r.checksum=logChecksum(r);pendingLog[pendingRecords++]=r;
  previousVoltage=mv;previousLogFlags=flags;lastLogSeconds=now.tv_sec;
  if (coldLogStart || pendingRecords>=4 || now.tv_sec-lastFlushSeconds>=3600) flushBatteryLog(now.tv_sec);
  coldLogStart=false;
}
void exportBatteryLog() {
  timeval now;gettimeofday(&now,nullptr);flushBatteryLog(now.tv_sec);
  if (!mountBatteryLog()) {Serial.println("Battery log unavailable; no automatic erase");return;}
  Serial.println("sequence,session,elapsed_seconds,millivolts,percent,awake_ms,flags");
  File file=LittleFS.open("/battery-v1.bin","r");BatteryLogRecord r;
  for (unsigned i=0;file && i<LOG_CAPACITY && file.read(reinterpret_cast<uint8_t*>(&r),sizeof(r))==sizeof(r);++i) {
    if (r.magic==LOG_MAGIC && r.checksum==logChecksum(r))
      Serial.printf("%lu,%lu,%lld,%u,%u,%lu,%u\n",(unsigned long)r.sequence,(unsigned long)r.session,
        (long long)r.seconds,r.millivolts,r.percent,(unsigned long)r.awakeMs,r.flags);
  }
  file.close();
}
