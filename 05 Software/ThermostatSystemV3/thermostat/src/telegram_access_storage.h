#pragma once
#include <Preferences.h>
#include "telegram_access.h"

// Core 0 owns the live ACL. Core 1 receives immutable drafts and writes NVS
// only in the same OpenTherm idle window used for thermostat settings.
static QueueHandle_t telegramAccessWrites=nullptr,telegramAccessSaved=nullptr;
static TelegramAccess::Record telegramUsers=TelegramAccess::empty();
static bool telegramAccessReady=false;

inline bool initializeTelegramAccess(){
  telegramUsers=TelegramAccess::empty();
  Preferences pref;
  if(pref.begin("telegram_acl",true)){
    TelegramAccess::Record record={};
    if(pref.getBytesLength("users")==sizeof(record)&&pref.getBytes("users",&record,sizeof(record))==sizeof(record)&&TelegramAccess::valid(record))telegramUsers=record;
    else if(pref.isKey("users"))Serial.println("Telegram ACL invalid; only owner is allowed");
    pref.end();
  }
  telegramAccessWrites=xQueueCreate(1,sizeof(TelegramAccess::Record));
  telegramAccessSaved=xQueueCreate(1,sizeof(bool));
  telegramAccessReady=telegramAccessWrites&&telegramAccessSaved;
  return telegramAccessReady;
}
inline void serviceTelegramAccessStorage(){
  if(!telegramAccessReady)return;
  static TelegramAccess::Record draft;
  if(xQueueReceive(telegramAccessWrites,&draft,0)!=pdPASS)return;
  bool saved=false;
  if(TelegramAccess::valid(draft)){
    Preferences pref;
    if(pref.begin("telegram_acl",false)){
      saved=pref.putBytes("users",&draft,sizeof(draft))==sizeof(draft);
      pref.end();
    }
  }
  xQueueOverwrite(telegramAccessSaved,&saved);
}
