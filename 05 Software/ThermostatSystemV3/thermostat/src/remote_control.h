#pragma once
// All access to controllerData and settings is on the control/UI task (Core 1).
RemoteProtocol::Settings captureRemoteSettings(){
  RemoteProtocol::Settings s;
  s.comfort=lroundf(controllerData.highTemp*100);s.night=lroundf(controllerData.lowTemp*100);
  s.offset=lroundf(controllerData.sensorOffset*100);s.target=lroundf(controllerData.temperatureSetpoint*100);
  const timeValue_t times[]={controllerData.workFromHomeWakeUp,controllerData.workFromHomeSleep,
    controllerData.workAtOfficeWakeUp,controllerData.workAtOfficeGoOut,controllerData.workAtOfficeComeIn,
    controllerData.workAtOfficeSleep,controllerData.weekendWakeUp,controllerData.weekendSleep};
  for(int i=0;i<8;++i)s.times[i]=times[i].minutesSinceMidnight;
  for(int i=0;i<7;++i)s.week[i]=controllerData.regularWeek[i];
  s.days=controllerData.overrideMultipleCount;s.today=controllerData.overrideToday;
  s.tomorrow=controllerData.overrideTomorrow;s.multiple=controllerData.overrideMultiple;
  s.forever=controllerData.multipleForever;s.manual=controllerData.overrideTempNow;return s;
}
void applyRemoteSettings(const RemoteProtocol::Settings&s){
  controllerData.highTemp=s.comfort/100.f;controllerData.lowTemp=s.night/100.f;
  controllerData.sensorOffset=s.offset/100.f;
  controllerData.overrideTempNow=s.manual;if(s.manual)controllerData.overrideSetpoint=s.target/100.f;
  timeValue_t *times[]={&controllerData.workFromHomeWakeUp,&controllerData.workFromHomeSleep,
    &controllerData.workAtOfficeWakeUp,&controllerData.workAtOfficeGoOut,&controllerData.workAtOfficeComeIn,
    &controllerData.workAtOfficeSleep,&controllerData.weekendWakeUp,&controllerData.weekendSleep};
  for(int i=0;i<8;++i)*times[i]=timeValue_t(int(s.times[i]));
  for(int i=0;i<7;++i)controllerData.regularWeek[i]=dayType_t(s.week[i]);
  controllerData.overrideMultipleCount=s.days;controllerData.overrideToday=dayType_t(s.today);
  controllerData.overrideTomorrow=dayType_t(s.tomorrow);controllerData.overrideMultiple=dayType_t(s.multiple);
  controllerData.multipleForever=s.forever;controllerData.settingsChanged=true;
  calculateNewSetpoint();sendMessage(sndController,cmdUpdateScreen,displayQueue);
}
