#pragma once
#include <remote_transport.h>
#include "remote_control.h"
uint32_t remoteLastRequestMs=0,remoteRequestCount=0;

// Core 1 only. ESP-NOW callbacks only copy into RemoteTransport's bounded queue.
void serviceRemoteLink() {
  using namespace RemoteProtocol;
  static uint32_t revision=1,lastReply=0,boot=esp_random()|1U,lease=0,session=0;
  static Settings previous;static bool initialized=false;
  static CommandGuard guard;
  static uint8_t sessionMac[6]={};
  static uint32_t rebootAt=0;
  if(rebootAt&&int32_t(millis()-rebootAt)>=0&&boilerStep==BoilerStep::Idle&&opentherm.isReady()){
    if(!controllerData.settingsChanged||controllerData.saveSettingsNVS())ESP.restart();
    rebootAt=millis()+1000;
  }
  Settings current=captureRemoteSettings();
  if(!initialized||memcmp(&current,&previous,sizeof(current))){previous=current;++revision;initialized=true;}
  RemoteTransport::Incoming incoming;
  for (int budget=0;budget<2 && RemoteTransport::poll(incoming);++budget) {
    const Packet &request=incoming.packet;
    if (request.kind!=Discover && request.kind!=GetStatus && request.kind!=Command) continue;
    remoteLastRequestMs=millis();++remoteRequestCount;
    if(request.kind!=Discover&&!RemoteTransport::same(request.target,RemoteTransport::ownMac))continue;
    if (millis()-lastReply<100 || RemoteTransport::sending.load()) continue;
    if (!RemoteTransport::setPeer(incoming.mac)) continue;
    if(request.kind==Discover){
      if(session!=request.session||!RemoteTransport::same(sessionMac,incoming.mac)){
        session=request.session;memcpy(sessionMac,incoming.mac,6);guard=CommandGuard{};lease=esp_random()|1U;
      }
    }else if(session!=request.session||!RemoteTransport::same(sessionMac,incoming.mac))continue;
    Packet reply;
    memcpy(reply.target,incoming.mac,6);
    reply.session=request.session; reply.request=request.request;
    reply.boot=boot;reply.capabilities=SettingsControl;
    if(controllerData.botChatID.length())reply.capabilities|=TelegramReports;
    if (request.kind==Discover) reply.kind=Welcome;
    else {
      reply.kind=request.kind==Command?Ack:Status;
      if(request.kind==Command){
        const int check=guard.inspect(request,boot,lease,millis()-request.leaseTime,revision);
        if(check==-1){RemoteTransport::send(guard.answer);lastReply=millis();continue;}
        reply.result=check;
        if(check==RemoteProtocol::OK){
          if(request.operation==Report){
            if(request.index>3||!controllerData.botChatID.length())reply.result=Unsupported;
            else{
              const command_t commands[]={cmdReportBoiler,cmdReportLog,cmdReportTiming,cmdReportDebug};
              userEventMessage_t message(controllerData.botChatID,scnMain,commands[request.index]);
              reply.result=tryEnqueueNetworkReply(message)?RemoteProtocol::OK:Busy;
            }
          }else if(request.operation==Restart){rebootAt=millis()+800;}
          else{
            Settings draft=current;reply.result=apply(draft,request);
            if(reply.result==RemoteProtocol::OK){applyRemoteSettings(draft);current=captureRemoteSettings();previous=current;++revision;}
          }
        }
      }
      reply.revision=revision;reply.lease=lease;reply.leaseTime=millis();reply.settings=current;
      if (roomTemperatureValid()) {
        reply.flags|=RoomValid;
        reply.roomCenti=lroundf(controllerData.measuredTemperature()*100);
      }
      reply.setpointCenti=lroundf(controllerData.temperatureSetpoint*100);
      if (controllerData.boilerOnline) reply.flags|=BoilerOnline;
      if (controllerData.centralHeatingActive) reply.flags|=HeatingActive;
      static_assert(iconFlame8-iconFlame0==8,"Flame icon enum must be contiguous");
      int selectedFlame=int(controllerData.dspFlameIcon)-int(iconFlame0);
      reply.flameLevel=selectedFlame>=0&&selectedFlame<=8?uint8_t(selectedFlame):0;
      if (controllerData.hotWaterActive) reply.flags|=HotWaterActive;
      if (controllerData.dayIconsValid) reply.flags|=DaysValid;
      for(int i=0;i<7;++i){reply.dayTypes[i]=controllerData.dayTypes[i];if(controllerData.dayTypeOverruled[i])reply.overrideMask|=1<<i;}
      reply.reason=controllerData.reasonForSetpoint;
      if (controllerData.timeSynched) {reply.flags|=ClockValid;reply.epoch=time(nullptr);}
    }
    if(request.kind==Command && request.request>guard.highest)guard.remember(request,reply);
    if (RemoteTransport::send(reply,reply.kind==Welcome)) lastReply=millis();
  }
}
