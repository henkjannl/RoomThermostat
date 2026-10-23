#pragma once
#include <stdint.h>
#include <stddef.h>

namespace RemoteProtocol {
constexpr uint32_t MAGIC = 0x33544852;
constexpr uint8_t WIRE_VERSION = 2;
enum Kind : uint8_t { Discover=1, Welcome=2, GetStatus=3, Status=4, Command=5, Ack=6 };
enum Flags : uint8_t { RoomValid=1, BoilerOnline=2, HeatingActive=4, ClockValid=8, HotWaterActive=16, DaysValid=32 };
enum Capability:uint16_t {SettingsControl=1,TelegramReports=2};
enum Operation:uint8_t {None=0,SetTarget=1,SetToday=2,SetTomorrow=3,SetMultiple=4,SetDuration=5,SetWeek=6,SetTime=7,SetComfort=8,SetNight=9,SetOffset=10,Automatic=11,Report=12,Restart=13};
enum Result:uint8_t {OK=0,Invalid=1,Conflict=2,Expired=3,WrongBoot=4,Unsupported=5,Busy=6,OldRequest=7};
constexpr uint8_t Auto=4;
#pragma pack(push,1)
struct Settings {
  int16_t comfort=1950,night=1000,offset=-350,target=2000;
  uint16_t times[8]={420,1290,390,435,1035,1290,540,1320};
  uint16_t days=3;
  uint8_t week[7]={2,1,1,1,1,1,2}; // Sunday first.
  uint8_t today=Auto,tomorrow=Auto,multiple=Auto,forever=0,manual=0;
};
// Wire format v2: fixed-width, little-endian fields, no pointers or strings.
// Session and request are echoed; MAC addresses are authenticated as well.
struct Packet {
  uint32_t magic = MAGIC;
  uint8_t version = WIRE_VERSION;
  uint8_t kind = 0;
  uint8_t channel = 0;
  uint8_t flags = 0;
  uint8_t source[6] = {};
  uint8_t target[6] = {};
  uint32_t session = 0;
  uint32_t request = 0;
  uint32_t revision = 0;
  uint32_t epoch = 0;
  int16_t roomCenti = 0;
  int16_t setpointCenti = 0;
  uint32_t boot=0,lease=0,leaseTime=0;
  uint16_t capabilities=0;
  uint8_t operation=None,index=0;
  int16_t value=0;
  uint8_t option=0,result=OK;
  Settings settings;
  uint8_t dayTypes[7]={},overrideMask=0,reason=0;
  uint8_t tag[16] = {};
};
#pragma pack(pop)
static_assert(sizeof(Settings)==38,"Settings layout changed");
static_assert(sizeof(Packet) == 123, "Protocol layout changed");
static_assert(sizeof(Packet) <= 250, "ESP-NOW v1 packet limit");
constexpr uint8_t BROADCAST[6] = {255,255,255,255,255,255};
inline bool sameCommand(const Packet&a,const Packet&b){
  return a.boot==b.boot&&a.lease==b.lease&&a.leaseTime==b.leaseTime&&a.revision==b.revision&&a.operation==b.operation&&a.index==b.index&&a.value==b.value&&a.option==b.option;
}
inline Result apply(Settings &s,const Packet &p){
  const int v=p.value;
  switch(p.operation){
    case SetTarget:if(v<400||v>3000||v%50)return Invalid;s.target=v;s.manual=1;break;
    case SetToday:case SetTomorrow:case SetMultiple:
      if(v<0||v>Auto)return Invalid;
      if(p.operation==SetToday){s.today=v;s.manual=0;}
      if(p.operation==SetTomorrow)s.tomorrow=v;
      if(p.operation==SetMultiple){s.multiple=v;s.manual=0;if(v==Auto)s.forever=0;else{s.today=Auto;s.tomorrow=Auto;if(!s.days)s.days=3;}}
      break;
    case SetDuration:
      if(v<1||v>365||p.option>1)return Invalid;
      s.days=v;s.forever=p.option;s.today=Auto;s.tomorrow=Auto;s.manual=0;break;
    case SetWeek:if(p.index>6||v<0||v>3)return Invalid;s.week[p.index]=v;break;
    case SetTime:{
      if(p.index>7||v<0||v>1425||v%15)return Invalid;
      uint16_t t[8];for(int i=0;i<8;++i)t[i]=s.times[i];t[p.index]=v;
      if(t[0]>=t[1]||t[2]>=t[3]||t[3]>=t[4]||t[4]>=t[5]||t[6]>=t[7])return Invalid;
      for(int i=0;i<8;++i)s.times[i]=t[i];break;
    }
    case SetComfort:if(v<450||v>3000||v%50)return Invalid;s.comfort=v;if(s.night>v-50)s.night=v-50;break;
    case SetNight:if(v<400||v>2950||v%50)return Invalid;s.night=v;if(s.comfort<v+50)s.comfort=v+50;break;
    case SetOffset:if(v<-500||v>500||v%10)return Invalid;s.offset=v;break;
    case Automatic:s.manual=0;break;
    default:return Unsupported;
  }
  return OK;
}
struct CommandGuard {
  uint32_t highest=0;bool cached=false;Packet previous,answer;
  int inspect(const Packet&p,uint32_t boot,uint32_t lease,uint32_t leaseAge,uint32_t revision)const{
    if(p.boot!=boot)return WrongBoot;
    if(cached&&p.request==highest)return sameCommand(p,previous)?-1:Invalid;
    if(!p.request||p.request<=highest)return OldRequest;
    if(!lease||p.lease!=lease||leaseAge>10000)return Expired;
    if(p.revision!=revision)return Conflict;
    return OK;
  }
  void remember(const Packet&p,const Packet&reply){highest=p.request;previous=p;answer=reply;cached=true;}
};
}
