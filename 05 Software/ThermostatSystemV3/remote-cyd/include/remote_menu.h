#pragma once
#include <remote_protocol.h>
#include <time.h>

namespace RemoteUi {
using namespace RemoteProtocol;
struct State {
  uint8_t link=0,battery=0;bool batteryValid=false,logOK=false;
  Packet snapshot;
};
enum Page:uint8_t {Home,Main,Period,SettingsPage,Week,Times,HomeTimes,OfficeTimes,WeekendTimes,
  Temperatures,More,StatusPage,Service,Reports,DayToday,DayTomorrow,DayMultiple,DayWeek,Edit,Confirm};
struct Button {int16_t x,y,w,h;uint8_t action;int16_t arg;};
static Button buttons[10];static uint8_t count=0;
static Page page=Home,history[12],returnAfter=Home;static uint8_t depth=0,weekDay=1;
static State state;static bool dirty=true,pending=false;static const char *message="";
static uint32_t editRevision=0,editBoot=0,editLease=0;
static uint8_t editOperation=0,editIndex=0;static int editValue=0,originalValue=0,step=1,minimum=0,maximum=0;
static bool forever=false,originalForever=false;
static const char *editTitle="";
using Send=bool(*)(uint8_t,uint8_t,int16_t,uint8_t,uint32_t,uint32_t,uint32_t);
static Send send=nullptr;
static const char *types[]={"Thuiswerken","Kantoor","Weekend","Afwezig","Automatisch"};
static const char *typeIcons[]={"/iconHome.jpg","/iconWork.jpg","/iconWeekend.jpg","/iconAway.jpg","/iconAutomatic.jpg"};
static const char *days[]={"Zondag","Maandag","Dinsdag","Woensdag","Donderdag","Vrijdag","Zaterdag"};
static const char *shortDays[]={"zo","ma","di","wo","do","vr","za"};
inline const char *typeIcon(int type){return typeIcons[type>=0&&type<=4?type:0];}
inline bool writable(){return state.link==1&&(state.snapshot.capabilities&SettingsControl)&&!pending;}
inline void small(Adafruit_GFX&lcd,const char*value,int x,int y,int maxWidth=176,uint16_t fg=Grey){
  // Exact original light font, scaled only when necessary for the small display.
  const int w=width(font_Noway_Light18,value);
  const float scale=min(.72f,float(maxWidth)/max(1,w));
  float cursor=x-w*scale/2;int baseline=y+int(word(font_Noway_Light18+16)*scale);
  while(*value){uint8_t code=uint8_t(*value++);Glyph g=glyph(font_Noway_Light18,code);
    if(code==0xB0)lcd.drawCircle(cursor+2,y+2,1,fg);
    for(int py=0;py<int(ceilf(g.h*scale));++py)for(int px=0;px<int(ceilf(g.w*scale));++px){
      float left=px/scale,right=min(float(g.w),(px+1)/scale),top=py/scale,bottom=min(float(g.h),(py+1)/scale),sum=0;
      for(int sy=int(top);sy<int(ceilf(bottom));++sy)for(int sx=int(left);sx<int(ceilf(right));++sx){
        float area=(min(right,float(sx+1))-max(left,float(sx)))*(min(bottom,float(sy+1))-max(top,float(sy)));
        sum+=area*pgm_read_byte(g.pixels+sy*g.w+sx);
      }
      uint8_t a=uint8_t(sum*scale*scale+.5f);
      if(a)lcd.drawPixel(int(cursor+g.dx*scale)+px,baseline-int(g.dy*scale)+py,blend(fg,White,a));
    }
    cursor+=g.advance*scale;
  }
}
inline void fit(Adafruit_GFX&lcd,const char*v,int x,int y,int maxWidth=176,uint16_t fg=Ink){
  if(width(font_Noway_Light18,v)>maxWidth)small(lcd,v,x,y,maxWidth,fg);
  else text(lcd,font_Noway_Light18,v,x-width(font_Noway_Light18,v)/2,y,fg);
}
inline void title(Adafruit_GFX&lcd,const char*v){fit(lcd,v,120,22,178,Grey);}
inline void add(int x,int y,int w,int h,uint8_t action,int arg=0){if(count<10)buttons[count++]={int16_t(x),int16_t(y),int16_t(w),int16_t(h),action,int16_t(arg)};}
inline void control(Adafruit_GFX&lcd,const char*image,int x,int y,uint8_t action,int arg=0){
  if(image)icon(lcd,image,x+12,y+12);else text(lcd,font_Noway_Medium24,"OK",x+28-width(font_Noway_Medium24,"OK")/2,y+18,Amber);
  add(x,y,56,56,action,arg);
}
inline void infinity(Adafruit_GFX&lcd,int x,int y,int size=32){
  float oldX=0,oldY=0;
  for(int i=0;i<=80;++i){float t=i*6.2831853f/80,d=1+sinf(t)*sinf(t),px=x+size*(.5f+.40625f*cosf(t)/d),py=y+size*(.5f+.40625f*sinf(t)*cosf(t)/d);
    if(i){lcd.drawLine(oldX,oldY,px,py,Grey);lcd.drawLine(oldX,oldY+1,px,py+1,Grey);}oldX=px;oldY=py;}
}
inline void duration(char*out,size_t len,int n,bool unlimited){
  if(unlimited){snprintf(out,len,"Onbeperkt");return;}
  if(!(state.snapshot.flags&ClockValid)){snprintf(out,len,"%d dagen; klok onbekend",n);return;}
  time_t now=state.snapshot.epoch;tm date;localtime_r(&now,&date);int year=date.tm_year;
  date.tm_hour=12;date.tm_min=0;date.tm_sec=0;date.tm_mday+=max(1,n)-1;date.tm_isdst=-1;mktime(&date);
  if(date.tm_year==year)snprintf(out,len,"t/m %s %02d-%02d",shortDays[date.tm_wday],date.tm_mday,date.tm_mon+1);
  else snprintf(out,len,"t/m %s %02d-%02d-%d",shortDays[date.tm_wday],date.tm_mday,date.tm_mon+1,date.tm_year+1900);
}
inline void valueLabel(char*out,size_t len,int value,int operation,bool unlimited=false){
  if(operation==SetTime)snprintf(out,len,"%02d:%02d",value/60,value%60);
  else if(operation==SetDuration){if(unlimited)snprintf(out,len,"Onbeperkt");else snprintf(out,len,"%d dagen",value);}
  else snprintf(out,len,"%.1f\xB0" "C",value/100.f);
}
inline void go(Page next){if(depth<12)history[depth++]=page;page=next;message="";dirty=true;}
inline void back(){if(pending)return;if(page==Edit||page==Confirm)page=returnAfter;else page=depth?history[--depth]:Home;message="";dirty=true;}
inline void beginEdit(const char*label,uint8_t op,uint8_t index,int value,int delta,int low,int high){
  returnAfter=page;editTitle=label;editOperation=op;editIndex=index;editValue=originalValue=value;
  step=delta;minimum=low;maximum=high;forever=originalForever=op==SetDuration&&state.snapshot.settings.forever;
  editRevision=state.snapshot.revision;editBoot=state.snapshot.boot;editLease=state.snapshot.lease;page=Edit;dirty=true;
}
inline void ask(uint8_t op,uint8_t index,int value,const char*label,Page after){
  returnAfter=after;editTitle=label;editOperation=op;editIndex=index;editValue=value;
  editRevision=state.snapshot.revision;editBoot=state.snapshot.boot;editLease=state.snapshot.lease;page=Confirm;dirty=true;
}
inline void submit(){
  if(!writable()){message="Geen actuele verbinding";dirty=true;return;}
  // Lease is refreshed by status polling; revision/boot remain the edit baseline.
  if(state.snapshot.boot!=editBoot){message="Thermostaat herstart; open opnieuw";dirty=true;return;}
  editLease=state.snapshot.lease;
  if(send&&send(editOperation,editIndex,editValue,editOperation==SetDuration&&forever,editRevision,editBoot,editLease)){
    pending=true;message="Even wachten...";
  }else message="Niet verzonden; probeer opnieuw";
  dirty=true;
}
inline void completed(uint8_t result,bool uncertain=false){
  pending=false;dirty=true;
  if(uncertain){page=returnAfter;message="Onbevestigd; controleer instelling";return;}
  if(result==RemoteProtocol::OK){
    page=returnAfter;
    if(page==DayMultiple){page=Period;if(depth&&history[depth-1]==Period)--depth;}
    if(page==DayToday||page==DayTomorrow){page=Home;depth=0;}
    if(page==DayWeek){page=Week;if(depth&&history[depth-1]==Week)--depth;}
    message=editOperation==Report?"Aangevraagd via thermostaat":editOperation==Restart?"Herstart aangevraagd":"Opgeslagen";
  }else{message=result==Conflict?"Gewijzigd elders; open opnieuw":result==Invalid?"Ongeldige waarde of tijdvolgorde":result==Unsupported?"Niet beschikbaar": "Niet opgeslagen; open opnieuw";}
}
inline void item(Adafruit_GFX&lcd,const char*label,const char*image,int i,int total,uint8_t action,int arg=0,const char*value=nullptr){
  bool grid=total==4;int x=grid?32+(i%2)*96:32,y=grid?100+(i/2)*88:98+i*62;
  if(grid){icon(lcd,image,x+24,y+2);fit(lcd,label,x+40,y+43,82,Grey);if(value)small(lcd,value,x+40,y+63,82,Ink);}
  else{icon(lcd,image,x+8,y+9);fit(lcd,label,x+109,y+(value?4:17),124,Grey);if(value)small(lcd,value,x+109,y+29,124,Ink);}
  add(x,y,grid?80:176,grid?80:54,action,arg);
}
inline void paintMenu(Adafruit_GFX&lcd){
  const auto&s=state.snapshot.settings;char a[64],b[64];count=0;lcd.fillScreen(White);
  if(page==Home){
    if(state.snapshot.flags&ClockValid){time_t now=state.snapshot.epoch;tm d;localtime_r(&now,&d);snprintf(a,sizeof(a),"%s %02d-%02d   %02d:%02d",shortDays[d.tm_wday],d.tm_mday,d.tm_mon+1,d.tm_hour,d.tm_min);}else strcpy(a,"Even verbinden");
    fit(lcd,a,120,16);
    icon(lcd,state.link==1?"/iconWifiConnected.jpg":"/iconWifiLost.jpg",36,44,24);
    bool known=state.link==1;icon(lcd,(state.snapshot.flags&BoilerOnline)?"/iconBoilerConn.jpg":"/iconBoilerLost.jpg",84,44,24,known?255:64);
    icon(lcd,"/iconShowerOn.jpg",132,44,24,known&&(state.snapshot.flags&BoilerOnline)?((state.snapshot.flags&HotWaterActive)?255:64):32);
    snprintf(a,sizeof(a),"/battery-%u.png",unsigned(min(100,25*((state.battery+12)/25))));icon(lcd,a,180,44,28,state.batteryValid?255:64);
    crop(lcd,"/HomeScreen.jpg",64,80,32,60,31,94,26,49);
    if(state.snapshot.flags&RoomValid)snprintf(a,sizeof(a),"%.1f",state.snapshot.roomCenti/100.f);else strcpy(a,"--.-");
    text(lcd,font_Noway_Medium60,a,65,88,known?Ink:Grey);text(lcd,font_Noway_Medium24,"\xB0" "C",187,109);
    crop(lcd,"/HomeScreen.jpg",104,146,32,34,65,143,27,28);
    if(state.snapshot.boot)snprintf(a,sizeof(a),"%.1f",state.snapshot.setpointCenti/100.f);else strcpy(a,"--.-");
    text(lcd,font_Noway_Medium24,a,104,147,s.manual?Amber:Grey);text(lcd,font_Noway_Light18,"\xB0" "C",160,149,Grey);
    icon(lcd,(state.snapshot.flags&HeatingActive)?"/iconFlame1.jpg":"/iconFlame0.jpg",184,145,24,known?255:64);
    a[0]=0;
    if(!known)snprintf(a,sizeof(a),state.link==0?"Verbinden...":"Geen actuele verbinding");
    else if(s.manual)strcpy(a,"Handmatig");
    else if(s.today<Auto)snprintf(a,sizeof(a),"Vandaag: %s",types[s.today]);
    else if(s.multiple<Auto){duration(b,sizeof(b),s.days,s.forever);snprintf(a,sizeof(a),"%s %s",types[s.multiple],b);}
    else if(s.tomorrow<Auto)snprintf(a,sizeof(a),"Morgen: %s",types[s.tomorrow]);
    if(*a)small(lcd,a,120,174,176,Amber);
    tm date={};time_t now=state.snapshot.epoch;localtime_r(&now,&date);
    for(int i=0;i<7;++i){int x=26+i*27;small(lcd,(state.snapshot.flags&ClockValid)?shortDays[(date.tm_wday+i)%7]:"--",x+12,191,24);
      if(state.snapshot.flags&DaysValid){icon(lcd,typeIcon(state.snapshot.dayTypes[i]),x,204,24);if(state.snapshot.overrideMask&(1<<i))lcd.fillRect(x+2,229,20,2,0xC207);else if(!i)lcd.drawFastHLine(x+4,229,16,Amber);}
    }
    control(lcd,"/remote-minus.png",36,232,40,-1);control(lcd,"/remote-menu.png",92,232,1,Main);control(lcd,"/remote-plus.png",148,232,40,1);
  }else if(page==Main){title(lcd,"Menu");item(lcd,"Vandaag","/iconToday.jpg",0,4,1,DayToday);item(lcd,"Morgen","/iconTomorrow.jpg",1,4,1,DayTomorrow);item(lcd,"Meer dagen","/iconMultipleDays.jpg",2,4,1,Period);item(lcd,"Instellen","/iconSettings.jpg",3,4,1,SettingsPage);
  }else if(page==Period){title(lcd,"Meer dagen");duration(a,sizeof(a),s.days,s.forever);small(lcd,a,153,62,104,Amber);
    item(lcd,"Dagtype",typeIcon(s.multiple),0,3,1,DayMultiple,types[min(int(s.multiple),4)]);
    valueLabel(a,sizeof(a),s.days,SetDuration,s.forever);item(lcd,"Aantal dagen","/iconClock.jpg",1,3,2,SetDuration,a);
    item(lcd,"Automatisch","/iconAutomatic.jpg",2,3,3,SetMultiple,s.multiple==Auto?"Geen afwijking":"Afwijking uitschakelen");
  }else if(page==SettingsPage){title(lcd,"Instellingen");item(lcd,"Weekschema","/iconWeekschedule.jpg",0,4,1,Week);item(lcd,"Tijden","/iconClock.jpg",1,4,1,Times);item(lcd,"Temperaturen","/iconSetpoint.jpg",2,4,1,Temperatures);item(lcd,"Meer","/iconSettings.jpg",3,4,1,More);
  }else if(page==Week){title(lcd,"Weekschema");centered(lcd,font_Noway_Medium24,days[weekDay],101,Grey);icon(lcd,typeIcon(s.week[weekDay]),96,144,48);control(lcd,"/remote-previous.png",36,232,4,-1);control(lcd,"/remote-menu.png",92,232,1,DayWeek);control(lcd,"/remote-next.png",148,232,4,1);
  }else if(page>=DayToday&&page<=DayWeek){
    title(lcd,page==DayToday?"Vandaag":page==DayTomorrow?"Morgen":page==DayMultiple?"Meer dagen":days[weekDay]);
    int selected=page==DayToday?s.today:page==DayTomorrow?s.tomorrow:page==DayMultiple?s.multiple:s.week[weekDay];
    for(int i=0;i<5;++i){int x=36+(i%2)*96,y=100+(i/2)*64;bool disabled=page==DayWeek&&i==Auto;icon(lcd,typeIcon(i),x+16,y+8,40,disabled?48:255);if(selected==i)lcd.fillRect(x+18,y+53,36,2,Amber);if(!disabled)add(x,y,72,56,5,i);}
  }else if(page==Times){title(lcd,"Tijden");item(lcd,"Thuiswerken",typeIcon(0),0,3,1,HomeTimes);item(lcd,"Kantoor",typeIcon(1),1,3,1,OfficeTimes);item(lcd,"Weekend",typeIcon(2),2,3,1,WeekendTimes);
  }else if(page==HomeTimes||page==OfficeTimes||page==WeekendTimes){
    int start=page==HomeTimes?0:page==OfficeTimes?2:6,n=page==OfficeTimes?4:2;title(lcd,page==HomeTimes?types[0]:page==OfficeTimes?types[1]:types[2]);
    const char*labels[]={"Opstaan","Vertrekken","Thuiskomen","Slapen"};const char*icons[]={"/remote-wake.png","/remote-leave.png","/remote-arrive.png","/remote-sleep.png"};
    for(int i=0;i<n;++i){int label=n==2?(i?3:0):i;valueLabel(a,sizeof(a),s.times[start+i],SetTime);item(lcd,labels[label],icons[label],i,n,6,start+i,a);}
  }else if(page==Temperatures){title(lcd,"Temperaturen");valueLabel(a,sizeof(a),s.comfort,SetComfort);item(lcd,"Comfort","/iconHighTemperature.jpg",0,2,2,SetComfort,a);valueLabel(a,sizeof(a),s.night,SetNight);item(lcd,"Nacht","/iconLowTemperature.jpg",1,2,2,SetNight,a);
  }else if(page==More){title(lcd,"Meer instellingen");valueLabel(a,sizeof(a),s.offset,SetOffset);item(lcd,"Sensorcorrectie","/iconTemperature.jpg",0,4,2,SetOffset,a);item(lcd,"Automatisch","/iconAutomatic.jpg",1,4,3,Automatic);item(lcd,"Status","/iconWifiConnected.jpg",2,4,1,StatusPage);item(lcd,"Service","/iconSettings.jpg",3,4,1,Service);
  }else if(page==StatusPage){title(lcd,"Verbindingsstatus");bool known=state.link==1,boiler=state.snapshot.flags&BoilerOnline;
    const char*names[]={"Thermostaat","Ketel","Warm water","Accu"};const char*icons[]={"/iconWifiConnected.jpg","/iconBoilerConn.jpg","/iconShowerOn.jpg","/battery-75.png"};
    for(int i=0;i<4;++i){int y=100+i*40;if(i==3)snprintf(a,sizeof(a),"/battery-%u.png",unsigned(min(100,25*((state.battery+12)/25))));icon(lcd,i==3?a:icons[i],32,y,28,i==2&&!(known&&boiler&&(state.snapshot.flags&HotWaterActive))?64:255);small(lcd,names[i],138,y+2,142);
      const char*v=i==0?(known?"Verbonden":"Geen actuele verbinding"):i==1?(!known?"Onbekend":boiler?"Communicatie OK":"Geen ketelantwoord"):i==2?(!known||!boiler?"Onbekend":state.snapshot.flags&HotWaterActive?"In gebruik":"Niet in gebruik"):a;
      if(i==3){if(state.batteryValid)snprintf(a,sizeof(a),"~%u%%",state.battery);else strcpy(a,"Onbekend");}small(lcd,v,138,y+18,142,Ink);
    }footer(lcd,FIRMWARE_VERSION);
  }else if(page==Service){title(lcd,"Service");item(lcd,"Rapporten","/iconBoilerConn.jpg",0,2,1,Reports);item(lcd,"Herstart","/iconExit.jpg",1,2,10,Restart);
  }else if(page==Reports){title(lcd,"Rapporten");const char*labels[]={"Ketel","Log","Timing","Debug"};for(int i=0;i<4;++i)item(lcd,labels[i],i==0?"/iconBoilerConn.jpg":i==2?"/iconClock.jpg":"/iconSettings.jpg",i,4,11,i);
  }else if(page==Edit){title(lcd,editTitle);valueLabel(a,sizeof(a),originalValue,editOperation,originalForever);snprintf(b,sizeof(b),"Huidig: %s",a);small(lcd,b,120,94);
    if(editOperation==SetDuration&&forever)infinity(lcd,88,116,64);else{
      const uint8_t*f=editOperation==SetTime?font_Noway_Medium32:font_Noway_Medium60;
      if(editOperation==SetTime)snprintf(a,sizeof(a),"%02d:%02d",editValue/60,editValue%60);else if(editOperation==SetDuration)snprintf(a,sizeof(a),"%d",editValue);else snprintf(a,sizeof(a),"%.1f",editValue/100.f);
      centered(lcd,f,a,120);
    }
    if(editOperation==SetDuration){infinity(lcd,160,46);add(148,34,56,56,7);if(forever)lcd.drawFastHLine(160,82,32,Amber);fit(lcd,forever?"Onbeperkt":"dagen",120,184);if(!forever){duration(a,sizeof(a),editValue,false);small(lcd,a,120,211,176,Amber);}}
    else if(editOperation!=SetTime)centered(lcd,font_Noway_Medium24,"\xB0" "C",181,Grey);
    control(lcd,"/remote-minus.png",36,232,8,-1);control(lcd,nullptr,92,232,9);control(lcd,"/remote-plus.png",148,232,8,1);
  }else if(page==Confirm){title(lcd,editOperation==Report?"Rapport aanvragen?":editOperation==Restart?"Thermostaat herstarten?":"Instelling wijzigen?");if(editOperation==SetToday||editOperation==SetTomorrow||editOperation==SetMultiple||editOperation==SetWeek)icon(lcd,typeIcon(editValue),96,106,48);else icon(lcd,"/iconAutomatic.jpg",96,106,48);fit(lcd,editTitle,120,173);if(editOperation==Report)small(lcd,"Via thermostaat naar Telegram",120,204);if(editOperation==SetMultiple&&editValue!=Auto){duration(a,sizeof(a),s.days,s.forever);small(lcd,a,120,204,176,Amber);}control(lcd,nullptr,92,232,9);}
  if(page!=Home)control(lcd,"/iconExit.jpg",36,34,0);
  if(*message)small(lcd,message,120,293,212,Amber);
}
inline void touch(int x,int y){
  if(pending)return;
  for(int i=0;i<count;++i){auto b=buttons[i];if(x<b.x||y<b.y||x>=b.x+b.w||y>=b.y+b.h)continue;message="";
    const auto&s=state.snapshot.settings;
    switch(b.action){
      case 0:back();break;
      case 1:if(!state.snapshot.boot&&b.arg!=StatusPage)message="Wacht op actuele verbinding";else go(Page(b.arg));break;
      case 2:if(!writable()){message="Wacht op actuele verbinding";break;}
        if(b.arg==SetDuration)beginEdit("Aantal dagen",SetDuration,0,max(1,int(s.days)),1,1,365);
        if(b.arg==SetComfort)beginEdit("Comfort",SetComfort,0,s.comfort,50,450,3000);
        if(b.arg==SetNight)beginEdit("Nacht",SetNight,0,s.night,50,400,2950);
        if(b.arg==SetOffset)beginEdit("Sensorcorrectie",SetOffset,0,s.offset,10,-500,500);break;
      case 3:if(writable())ask(b.arg,0,Auto,"Automatisch",page);else message="Wacht op actuele verbinding";break;
      case 4:weekDay=(weekDay+b.arg+7)%7;break;
      case 5:if(writable())ask(page==DayToday?SetToday:page==DayTomorrow?SetTomorrow:page==DayMultiple?SetMultiple:SetWeek,weekDay,b.arg,types[b.arg],page);else message="Wacht op actuele verbinding";break;
      case 6:if(writable())beginEdit("Tijd instellen",SetTime,b.arg,s.times[b.arg],15,0,1425);else message="Wacht op actuele verbinding";break;
      case 7:forever=true;break;
      case 8:forever=false;editValue=constrain(editValue+b.arg*step,minimum,maximum);break;
      case 9:submit();break;
      case 10:if(writable())ask(Restart,0,0,"Opnieuw opstarten",Service);break;
      case 11:if(writable()&&(state.snapshot.capabilities&TelegramReports))ask(Report,b.arg,0,"Naar Telegram",Reports);else message="Telegram niet beschikbaar";break;
      case 40:if(writable()){
        returnAfter=Home;editOperation=SetTarget;editIndex=0;editValue=constrain(int(lroundf(state.snapshot.setpointCenti/50.f))*50+b.arg*50,400,3000);editRevision=state.snapshot.revision;editBoot=state.snapshot.boot;submit();
      }else message="Wacht op actuele verbinding";break;
    }dirty=true;return;
  }
}
inline void paint(Adafruit_ILI9341&lcd,const State&s,const char*){
  bool changed=state.link!=s.link||state.battery!=s.battery||state.batteryValid!=s.batteryValid||
    state.snapshot.flags!=s.snapshot.flags||state.snapshot.roomCenti!=s.snapshot.roomCenti||state.snapshot.setpointCenti!=s.snapshot.setpointCenti||
    state.snapshot.epoch/60!=s.snapshot.epoch/60||memcmp(&state.snapshot.settings,&s.snapshot.settings,sizeof(Settings))||
    memcmp(state.snapshot.dayTypes,s.snapshot.dayTypes,7)||state.snapshot.overrideMask!=s.snapshot.overrideMask;
  state=s;if(!dirty&&!changed)return;
  static GFXcanvas16 *frame=new GFXcanvas16(240,320);
  if(frame&&frame->getBuffer()){paintMenu(*frame);lcd.drawRGBBitmap(0,0,frame->getBuffer(),240,320);}else paintMenu(lcd);
  dirty=false;
}
}
