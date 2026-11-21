// Executes the production capture, overflow, timestamp, formatting and retry code.
// Network/time/FreeRTOS are deterministic fakes, not ESP32 timing simulations.
#include <cassert>
#include <cstring>
#include <deque>
#include <vector>
#include <string>
#include <stdint.h>
#include <cstdarg>
int64_t fakeUs=0;
long fakeUtc=0;
int httpCode=-1;
std::vector<std::string> requests;
struct SerialFake {void println(const char *){} void printf(const char *,...){} } Serial;
uint32_t millis() {return uint32_t(fakeUs/1000);}
uint32_t esp_random() {return 1234;}
struct FakeQueue {size_t capacity,itemSize; std::deque<std::vector<unsigned char>> items;};
using QueueHandle_t=FakeQueue*;
enum {pdPASS=1};
QueueHandle_t xQueueCreate(size_t count,size_t size) {return new FakeQueue{count,size,{}};}
int xQueueSend(QueueHandle_t q,const void *data,int wait) {
  assert(wait==0);
  if(q->items.size()==q->capacity)return 0;
  auto bytes=static_cast<const unsigned char*>(data);
  q->items.emplace_back(bytes,bytes+q->itemSize);return pdPASS;
}
int xQueueReceive(QueueHandle_t q,void *data,int wait) {
  assert(wait==0);
  if(q->items.empty())return 0;
  memcpy(data,q->items.front().data(),q->itemSize);q->items.pop_front();return pdPASS;
}
size_t uxQueueMessagesWaiting(QueueHandle_t q) {return q->items.size();}
struct Control {
  float temperatureSetpoint=20,boilerPercentage=.5f,requestedBoilerTemperature=32.5f;
  bool enableHotWater=true,enableCentralHeating=true;
  float measuredTemperature(){return 19.5f;}
} controllerData;
bool roomTemperatureValid(){return true;}
#define THERMOSTAT_INFLUX_ENABLED 1
#define THERMOSTAT_INFLUX_TOKEN "test-only"
#define THERMOSTAT_INFLUX_URL "http://test.invalid/api/v2/write"
#include "telemetry.h"

int main() {
  using namespace Telemetry;
  begin(); assert(records);
  telemetryObserveStatus(true,true);
  assert(uxQueueMessagesWaiting(records)==1);
  telemetryObserveStatus(true,true);
  assert(uxQueueMessagesWaiting(records)==1); // Repeat status is not an edge.
  telemetryObserveStatus(false,false);
  Record r{};
  xQueueReceive(records,&r,0); assert(r.flags&HotWaterActive);
  xQueueReceive(records,&r,0); assert(!(r.flags&StatusValid));
  char line[512];
  int count=format(line,sizeof(line),r,1234,1704067200000LL);
  assert(count>0 && size_t(count)<sizeof(line));
  assert(strstr(line,"status_valid=false"));
  assert(!strstr(line,"dhw_active=")); // Failure must never imply hot water off.
  telemetryObserveStatus(true,false);
  xQueueReceive(records,&r,0); assert(r.flags&StatusValid);assert(!(r.flags&HotWaterActive));
  serviceControl(); // Enable state and periodic sample.
  fakeUs=20000000; serviceControl(); // Stale status emits unknown.
  assert(status==-1);
  while(xQueueReceive(records,&r,0)==pdPASS) {}
  for(unsigned i=0;i<Capacity+5;++i)capture(Sample);
  assert(dropped==5 && uxQueueMessagesWaiting(records)==Capacity);
  xQueueReceive(records,&r,0);assert(r.sequence==13); // Oldest five evicted; earlier seven consumed.
  fakeUs=30000000;
  serviceWorker(false);assert(requests.empty());
  serviceWorker(true);assert(requests.empty()); // Clock not set.
  fakeUtc=1793500000;
  serviceWorker(true);assert(requests.size()==1); // Server unavailable.
  std::string original=requests.back();
  fakeUs+=4000000;serviceWorker(true);assert(requests.size()==1);
  fakeUs+=1000000;fakeUtc+=3600;httpCode=401;
  serviceWorker(true);assert(requests.size()==2 && requests.back()==original);
  fakeUs+=10000000;httpCode=204;serviceWorker(true);
  assert(requests.size()==3 && requests.back()==original); // Exact retry even after time correction.
  fakeUs+=1000000;serviceWorker(true);
  assert(requests.size()==4 && requests.back()!=original); // Only ACK advances pending batch.
  Clock clock;
  r.uptimeUs=1000000;
  auto first=clock.stamp(r,1704067200000LL,2000000);
  assert(first==1704067199000LL);
  assert(clock.stamp(r,1704060000000LL,2000000)==first+1); // Backward clock never overwrites a point.
  r.flags=StatusValid; r.room=NAN;
  count=format(line,sizeof(line),r,1234,first);
  assert(count>0 && !strstr(line,"room_temperature_c="));
  assert(strstr(line,"dhw_active=false"));
  r.boilerSetpoint=NAN;assert(format(line,sizeof(line),r,1234,first)==-1);
  delete records;
}
