#pragma once
#include <WiFi.h>
#include <remote_transport.h>
#include <ui_theme.h>
#include "battery_log.h"

RTC_DATA_ATTR uint8_t lastRadioChannel=1;
uint32_t wakeStarted=0, lastActivity=0, discoveryStarted=0, lastSend=0, lastStatusAt=0;
uint32_t wakeSession=0, requestNumber=0;
uint32_t peerConfirmedAt=0;
uint16_t batteryMillivolts=0;
uint8_t batteryPercent=0;
bool batteryValid=false, connected=false, haveStatus=false, radioOK=false;
RemoteProtocol::Packet latestStatus;
uint8_t thermostatMac[6]={};
int lastTouchX=-1,lastTouchY=-1;
bool commandPending=false;
RemoteProtocol::Packet pendingCommand;
uint32_t commandStarted=0,commandSentAt=0;
uint8_t commandAttempts=0;

bool queueRemoteCommand(uint8_t operation,uint8_t index,int16_t value,uint8_t option,uint32_t revision,uint32_t boot,uint32_t lease){
  if(commandPending||!connected||!haveStatus||millis()-lastStatusAt>=5000)return false;
  pendingCommand=RemoteProtocol::Packet{};pendingCommand.kind=RemoteProtocol::Command;
  pendingCommand.session=wakeSession;pendingCommand.request=++requestNumber;
  pendingCommand.operation=operation;pendingCommand.index=index;pendingCommand.value=value;pendingCommand.option=option;
  pendingCommand.revision=revision;pendingCommand.boot=boot;pendingCommand.lease=lease;
  pendingCommand.leaseTime=latestStatus.leaseTime;
  memcpy(pendingCommand.target,thermostatMac,6);
  commandPending=true;commandStarted=millis();commandSentAt=0;commandAttempts=0;return true;
}

void printFirmwareBanner() {
  Serial.printf("CYD remote %s (%s), %dx%d; commands: version, history\n",
    FIRMWARE_VERSION,BATTERY_ONLY?"battery":"usb-test",screen.width(),screen.height());
}

void serviceFirmwareBanner() {
  // USB disappears in deep sleep. Give the host time to reopen the monitor,
  // without waiting in setup or extending the normal awake interval.
  static bool wasConnected=false;
  static uint32_t connectedAt=0;
  static uint8_t reports=0;
  bool connected=bool(Serial);
  if (connected && !wasConnected) {connectedAt=millis();reports=0;}
  wasConnected=connected;
  if (connected && reports<2 && millis()-connectedAt>=(reports?2500U:500U)) {
    printFirmwareBanner();++reports;
  }
}

void paintStatus() {
  bool fresh=haveStatus && millis()-lastStatusAt<5000;
  uint8_t link=!radioOK?4:fresh?1:haveStatus?2:millis()-discoveryStarted<8000?0:3;
  RemoteUi::State state;
  state.link=link;state.battery=batteryPercent;state.batteryValid=batteryValid;
  state.logOK=batteryLogOK;state.snapshot=latestStatus;
  RemoteUi::paint(screen,state,FIRMWARE_VERSION);
}

bool readPortraitTouch(int &x,int &y) {
  uint8_t count, xh,xl,yh,yl;
  if (!readReg(TOUCH_ADDR,0x02,count) || !(count&15)) return false;
  if (!readReg(TOUCH_ADDR,0x03,xh)||!readReg(TOUCH_ADDR,0x04,xl)||
      !readReg(TOUCH_ADDR,0x05,yh)||!readReg(TOUCH_ADDR,0x06,yl)) return false;
  x=((xh&15)<<8)|xl;y=((yh&15)<<8)|yl;
  if (TOUCH_SWAP_XY) {int tmp=x;x=y;y=tmp;}
  if (TOUCH_MIRROR_X) x=239-x;
  if (TOUCH_MIRROR_Y) y=319-y;
  return x>=0 && x<240 && y>=0 && y<320;
}

void beginRemoteRadio() {
  WiFi.persistent(false);WiFi.mode(WIFI_STA);
  // Netherlands: scan the legal 2.4 GHz channels 1..13 without joining an AP.
  wifi_country_t country={"NL",1,13,20,WIFI_COUNTRY_POLICY_MANUAL};
  esp_wifi_set_country(&country);
  if (lastRadioChannel<1 || lastRadioChannel>13) lastRadioChannel=1;
  esp_wifi_set_channel(lastRadioChannel,WIFI_SECOND_CHAN_NONE);
  radioOK=RemoteTransport::begin();
  wakeSession=esp_random();discoveryStarted=millis();lastSend=millis()-400;
}

void serviceRemoteRadio() {
  if (!radioOK) return;
  using namespace RemoteProtocol;
  RemoteTransport::Incoming item;
  for (int budget=0;budget<3 && RemoteTransport::poll(item);++budget) {
    Packet &p=item.packet;
    if (p.session!=wakeSession || p.request!=requestNumber || !RemoteTransport::same(p.target,RemoteTransport::ownMac)) continue;
    if (p.kind==Welcome && p.channel==RemoteTransport::channel()) {
      if (RemoteTransport::setPeer(item.mac)) {
        memcpy(thermostatMac,item.mac,6);connected=true;lastSend=millis()-2000;
        peerConfirmedAt=millis();
        lastRadioChannel=p.channel;
      }
    } else if ((p.kind==Status||p.kind==Ack) && connected && RemoteTransport::same(item.mac,thermostatMac)) {
      if(commandPending){
        if(p.kind!=Ack||p.request!=pendingCommand.request)continue;
        commandPending=false;RemoteUi::completed(p.result);
        Serial.printf("Remote command id=%lu result=%u\n",(unsigned long)p.request,p.result);
      }else if(p.kind!=Status)continue;
      if (!haveStatus || millis()-lastStatusAt>5000)
        Serial.printf("Fresh status after %lu ms, channel %u\n",(unsigned long)(millis()-wakeStarted),p.channel);
      latestStatus=p;haveStatus=true;lastStatusAt=millis();paintStatus();
      if(p.kind==Ack)lastSend=millis()-1500;
    }
  }
  if(commandPending){
    if(millis()-commandStarted>=4500){
      commandPending=false;RemoteUi::completed(RemoteProtocol::Expired,true);
      haveStatus=false;lastSend=millis()-1500;paintStatus();
    }else if(!RemoteTransport::sending.load()&&commandAttempts<5&&(!commandAttempts||millis()-commandSentAt>=700)){
      if(RemoteTransport::send(pendingCommand)){commandSentAt=millis();++commandAttempts;}
    }
    return;
  }
  if (connected && ((haveStatus && millis()-lastStatusAt>5000) ||
                    (!haveStatus && millis()-peerConfirmedAt>4000))) {
    connected=false;discoveryStarted=millis();paintStatus();
  }
  if (RemoteTransport::sending.load()) return;
  if (connected) {
    if (millis()-lastSend<1500) return;
    Packet p;p.kind=GetStatus;p.session=wakeSession;p.request=++requestNumber;
    memcpy(p.target,thermostatMac,6);
    if (RemoteTransport::send(p)) lastSend=millis();
  } else if (millis()-discoveryStarted<8000 && millis()-lastSend>=350) {
    // Try the remembered channel first, then one channel per bounded dwell.
    if (requestNumber) {
      uint8_t next=RemoteTransport::channel()%13+1;
      esp_wifi_set_channel(next,WIFI_SECOND_CHAN_NONE);
    }
    Packet p;p.kind=Discover;p.session=wakeSession;p.request=++requestNumber;
    memcpy(p.target,BROADCAST,6);
    RemoteTransport::send(p,true);lastSend=millis();
  }
}

void setup() {
  wakeStarted=millis();lastActivity=wakeStarted;
  Serial.begin(115200);
  setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);tzset();
  RemoteUi::send=queueRemoteCommand;
  initializeCYD(); // Shows wake screen before touch reset, battery reads or radio.
  batteryMillivolts=measureBatteryMillivolts();
  batteryValid=batteryMillivolts>=2800 && batteryMillivolts<=4350;
  batteryPercent=uint8_t(percentFromVoltage(batteryMillivolts/1000.0f)+0.5f);
  beginBatteryLog();
  beginRemoteRadio();
  lastActivity=millis();paintStatus();
  printFirmwareBanner();
}

void loop() {
  serviceFirmwareBanner();
  static uint32_t lastTouchPoll=0, lastPaint=0;
  static bool released=false,pressed=false;
  static uint32_t releasedAt=millis();
  if (millis()-lastTouchPoll>=50) {
    lastTouchPoll=millis();int x,y;
    if (readPortraitTouch(x,y)) {
      lastActivity=millis();
      if (x!=lastTouchX || y!=lastTouchY) Serial.printf("Touch portrait x=%d y=%d\n",x,y);
      lastTouchX=x;lastTouchY=y;
      if (!connected && millis()-discoveryStarted>=8000) discoveryStarted=millis();
      if(released&&!pressed){RemoteUi::touch(x,y);pressed=true;paintStatus();}
      releasedAt=millis();
    }else if(millis()-releasedAt>=200){released=true;pressed=false;}
  }
  serviceRemoteRadio();
  if (millis()-lastPaint>2000) {lastPaint=millis();paintStatus();}
  static char command[16];static unsigned commandLength=0;
  while (Serial.available()) {
    char c=Serial.read();
    if (c=='\n' || c=='\r') {
      command[commandLength]=0;
      if (!strcmp(command,"version")) {printFirmwareBanner();lastActivity=millis();}
      if (!strcmp(command,"history")) {exportBatteryLog();lastActivity=millis();}
      commandLength=0;
    } else if (commandLength<sizeof(command)-1) command[commandLength++]=c;
  }
  if (!commandPending && millis()-lastActivity>=MESSAGE_MS) {
    // Finish the battery record before radio shutdown; no forced periodic wakes.
    recordBattery(batteryMillivolts,batteryPercent,batteryValid,
      haveStatus && millis()-lastStatusAt<5000,millis()-wakeStarted);
    if (sleepUntilTap()) return;
    lastActivity=millis();paintStatus();
  }
  delay(2);
}
