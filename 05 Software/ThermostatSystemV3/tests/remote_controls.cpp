#include <assert.h>
#include <fstream>
#include <string>
#include <time.h>
inline tm*localtime_r(const time_t*t,tm*out){return localtime_s(out,t)?nullptr:out;}
constexpr const char *FIRMWARE_VERSION="3.0.0-alpha.2";
#include "remote_ui.h"
using namespace RemoteProtocol;
static int sends=0;static Packet submitted;
static bool sendTest(uint8_t op,uint8_t index,int16_t value,uint8_t option,uint32_t revision,uint32_t boot,uint32_t lease){
  submitted=Packet{};submitted.operation=op;submitted.index=index;submitted.value=value;submitted.option=option;submitted.revision=revision;submitted.boot=boot;submitted.lease=lease;++sends;return true;
}
void contracts(){
  assert(sizeof(Packet)==123&&offsetof(Packet,tag)==107);
  Settings s;Packet p;p.operation=SetTime;p.index=3;p.value=390;auto before=s;
  assert(apply(s,p)==Invalid&&!memcmp(&s,&before,sizeof(s)));
  p.value=480;assert(apply(s,p)==RemoteProtocol::OK&&s.times[3]==480);
  p.index=8;assert(apply(s,p)==Invalid);p.index=0;p.value=421;assert(apply(s,p)==Invalid);
  p.operation=SetWeek;p.index=6;p.value=Auto;assert(apply(s,p)==Invalid);p.value=3;assert(apply(s,p)==RemoteProtocol::OK&&s.week[6]==3);
  p.operation=SetComfort;p.value=500;assert(apply(s,p)==RemoteProtocol::OK&&s.night==450);p.operation=SetNight;p.value=2950;assert(apply(s,p)==RemoteProtocol::OK&&s.comfort==3000);
  p.operation=SetTarget;p.value=3050;assert(apply(s,p)==Invalid);p.value=2050;assert(apply(s,p)==RemoteProtocol::OK&&s.manual&&s.target==2050);
  p.operation=SetDuration;p.value=3;p.option=1;assert(apply(s,p)==RemoteProtocol::OK&&s.forever&&s.days==3);
  p.operation=SetMultiple;p.value=3;assert(apply(s,p)==RemoteProtocol::OK&&s.multiple==3&&s.forever);
  p.value=Auto;assert(apply(s,p)==RemoteProtocol::OK&&s.multiple==Auto&&!s.forever);
  p.operation=SetOffset;p.value=-501;assert(apply(s,p)==Invalid);p.value=-490;assert(apply(s,p)==RemoteProtocol::OK);
  CommandGuard guard;Packet q;q.request=9;q.boot=17;q.lease=21;q.revision=3;q.operation=SetTarget;q.value=2100;
  assert(guard.inspect(q,17,21,0,3)==RemoteProtocol::OK);
  assert(guard.inspect(q,18,21,0,3)==WrongBoot);assert(guard.inspect(q,17,22,0,3)==Expired);
  assert(guard.inspect(q,17,21,10001,3)==Expired);assert(guard.inspect(q,17,21,0,4)==Conflict);
  Packet ack;ack.result=RemoteProtocol::OK;guard.remember(q,ack);
  assert(guard.inspect(q,17,21,15000,4)==-1); // Lost ACK: don't apply twice.
  q.value=2150;assert(guard.inspect(q,17,21,0,3)==Invalid);q.request=8;assert(guard.inspect(q,17,21,0,3)==OldRequest);
  q.request=10;q.revision=4;assert(guard.inspect(q,17,21,0,4)==RemoteProtocol::OK);
}
int main(int argc,char**argv){
  contracts();
  Adafruit_ILI9341 lcd;RemoteUi::State s;s.link=1;s.battery=76;s.batteryValid=true;
  s.snapshot.boot=17;s.snapshot.revision=3;s.snapshot.lease=21;s.snapshot.capabilities=SettingsControl|TelegramReports;
  s.snapshot.flags=RoomValid|ClockValid|BoilerOnline|DaysValid;s.snapshot.roomCenti=2040;s.snapshot.setpointCenti=1950;s.snapshot.epoch=1792771200;
  for(int i=0;i<7;++i)s.snapshot.dayTypes[i]=s.snapshot.settings.week[(i+5)%7];
  RemoteUi::send=sendTest;
  auto draw=[&](){RemoteUi::paint(lcd,s,FIRMWARE_VERSION);
    for(int i=0;i<RemoteUi::count;++i){auto a=RemoteUi::buttons[i];assert(a.x>=26&&a.y>=26&&a.x+a.w<=214&&a.y+a.h<=298);for(int j=i+1;j<RemoteUi::count;++j){auto b=RemoteUi::buttons[j];assert(a.x+a.w<=b.x||b.x+b.w<=a.x||a.y+a.h<=b.y||b.y+b.h<=a.y);}}
    int backs=0;for(int i=0;i<RemoteUi::count;++i)if(!RemoteUi::buttons[i].action){auto b=RemoteUi::buttons[i];assert(b.x==36&&b.y==34&&b.w==56&&b.h==56);++backs;}
    assert(backs==(RemoteUi::page==RemoteUi::Home?0:1));
  };
  auto screenshot=[&](const char*name){if(argc<2)return;std::ofstream f(std::string(argv[1])+"/firmware-"+name+".ppm",std::ios::binary);f<<"P6\n240 320\n255\n";for(auto pixel:lcd.pixels){char rgb[]={char(((pixel>>11)&31)*255/31),char(((pixel>>5)&63)*255/63),char((pixel&31)*255/31)};f.write(rgb,3);}};
  auto tap=[&](int x,int y){RemoteUi::touch(x,y);draw();};
  auto ack=[&](){assert(RemoteUi::pending);assert(apply(s.snapshot.settings,submitted)==RemoteProtocol::OK);++s.snapshot.revision;RemoteUi::completed(RemoteProtocol::OK);draw();};
  draw();assert(RemoteUi::count==3);screenshot("home");tap(120,260);assert(RemoteUi::page==RemoteUi::Main);screenshot("menu");tap(72,220);assert(RemoteUi::page==RemoteUi::Period);screenshot("period");
  tap(160,118);assert(RemoteUi::page==RemoteUi::DayMultiple);screenshot("daytypes");tap(72,184);assert(RemoteUi::page==RemoteUi::Confirm);tap(120,260);ack();assert(RemoteUi::page==RemoteUi::Period&&s.snapshot.settings.multiple==2);
  tap(160,180);assert(RemoteUi::page==RemoteUi::Edit);tap(176,60);assert(RemoteUi::forever);tap(120,260);ack();assert(RemoteUi::page==RemoteUi::Period&&s.snapshot.settings.forever);
  tap(160,180);tap(176,260);int previousSends=sends;tap(64,60);assert(RemoteUi::page==RemoteUi::Period&&sends==previousSends&&s.snapshot.settings.forever);
  tap(160,248);tap(120,260);ack();assert(s.snapshot.settings.multiple==Auto&&!s.snapshot.settings.forever&&RemoteUi::page==RemoteUi::Period);
  tap(64,60);assert(RemoteUi::page==RemoteUi::Main); // No duplicate Period in stack.
  for(int page=RemoteUi::SettingsPage;page<=RemoteUi::DayWeek;++page){RemoteUi::page=RemoteUi::Page(page);RemoteUi::dirty=true;draw();screenshot(("screen-"+std::to_string(page)).c_str());}
  RemoteUi::page=RemoteUi::OfficeTimes;RemoteUi::dirty=true;draw();tap(72,130);assert(RemoteUi::originalValue==390);tap(176,260);assert(RemoteUi::originalValue==390&&RemoteUi::editValue==405);screenshot("time-editor");tap(120,260);ack();assert(s.snapshot.settings.times[2]==405);
  RemoteUi::page=RemoteUi::Home;RemoteUi::dirty=true;s.link=3;draw();previousSends=sends;tap(176,260);assert(sends==previousSends);
  s.link=1;draw();tap(176,260);assert(RemoteUi::pending);RemoteUi::completed(Expired,true);draw();assert(!RemoteUi::pending);screenshot("uncertain");
  puts("PASS: actual C++ settings validation, command deduplication/conflicts/expiry/reboot, firmware navigation, period return, drafts, offline controls, every screen's touch geometry and fixed back.");
}
