#pragma once

#include <map>
#include <WiFiClientSecure.h>
#include <AsyncTelegram2.h>
#include <WiFiClientSecure.h>
#include <ArduinoOTA.h>

#define TELEGRAM_INTERVAL 1000

using namespace std;

#define HOME_ICON     EMOTICON_HOUSE
#define WORK_ICON     EMOTICON_OFFICE
#define WEEKEND_ICON  EMOTICON_DANCER
#define AWAY_ICON     EMOTICON_ISLAND
#define AUTO_ICON     EMOTICON_MAGIC_STICK

std::map<dayType_t, String> DAYTYPE_TO_EMOTICON = {
  {dtWorkFromHome, String(HOME_ICON) },
  {dtWorkAtOffice, String(WORK_ICON) },
  {dtWeekend,      String(WEEKEND_ICON) },
  {dtAway,         String(AWAY_ICON) },
  {dtAuto,         String(AUTO_ICON) },
};

// The Telegram bot
WiFiClientSecure client;
AsyncTelegram2 myBot(client);
InlineKeyboard mainKeyboard, settingsKeyboard; 
uint32_t telegramEnabledCounter=0;
bool telegramEnabled() { return (telegramEnabledCounter==0); };
void enableTelegram()  { if( telegramEnabledCounter>0 ) telegramEnabledCounter--; };
void disableTelegram() { telegramEnabledCounter++; };

#define CB_TEMP_UP       "cbTempUp"
#define CB_TEMP_AUTO     "cbTempAuto"
#define CB_TEMP_DOWN     "cbTempDown"
#define CB_SETTINGS_MENU "cbSettingsMenu"
#define CB_MAIN_MENU     "cbMainMenu"

/* =================================================================== *
 *  DATA STRUCTURES THAT MANAGE USERS, MENUS AND COMMANDS IN TELEGRAM  *
 * =================================================================== */

void onTempUp(const TBMessage &queryMsg){
  Serial.println("Temperature higher");
  sendMessage(sndTelegram, cmdSetpointHigher, controllerQueue);
  myBot.editMessage(queryMsg, "Temperature higher", mainKeyboard );    
  myBot.endQuery(queryMsg, "Temperature higher", false);
}

void onTempAuto(const TBMessage &queryMsg){
  Serial.println("Temperature auto");
  sendMessage(sndTelegram, cmdSetpointAutomatic, controllerQueue);
  myBot.editMessage(queryMsg, "Temperature auto", mainKeyboard );    
  myBot.endQuery(queryMsg, "Temperature auto", false);
}

void onTempDown(const TBMessage &queryMsg){
  Serial.println("Temperature lower");
  sendMessage(sndTelegram, cmdSetpointLower, controllerQueue);
  myBot.editMessage(queryMsg, "Temperature lower", mainKeyboard );    
  myBot.endQuery(queryMsg, "Temperature higher", false);
}

void onSettingsMenu(const TBMessage &queryMsg){
  Serial.println("Go to settings menu");
  sendMessage(sndTelegram, cmdMenuSettings, telegramQueue);
  myBot.editMessage(queryMsg, "Go to settings menu", settingsKeyboard);    
  myBot.endQuery(queryMsg, "Go to settings menu", false);
}

void onMainMenu(const TBMessage &queryMsg){
  Serial.println("Main menu");
  sendMessage(sndTelegram, cmdMenuMain, telegramQueue);
  myBot.editMessage(queryMsg, "Main menu", mainKeyboard );    
  myBot.endQuery(queryMsg, "Main menu", false);
}

void startTelegram() {

  client.setCACert(telegram_cert);

  // Add three different keyboards
  mainKeyboard.addButton("-"          , CB_TEMP_DOWN,     KeyboardButtonQuery, onTempDown );
  mainKeyboard.addButton("Auto"       , CB_TEMP_AUTO,     KeyboardButtonQuery, onTempAuto );
  mainKeyboard.addButton("+"          , CB_TEMP_UP,       KeyboardButtonQuery, onTempUp   );
  mainKeyboard.addRow();
  mainKeyboard.addButton("Settings...", CB_SETTINGS_MENU, KeyboardButtonQuery, onSettingsMenu );
  
  settingsKeyboard.addButton("Back to main menu...", CB_MAIN_MENU, KeyboardButtonQuery, onSettingsMenu );

  // Set the Telegram bot properies
  myBot.setUpdateTime( 1000 );
  myBot.setTelegramToken( controllerData.botToken.c_str() );

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  myBot.setFormattingStyle(AsyncTelegram2::FormatStyle::MARKDOWN );

  myBot.addInlineKeyboard(&mainKeyboard);
  myBot.addInlineKeyboard(&settingsKeyboard);

  myBot.sendTo( 46501331, "Welcome!", mainKeyboard.getJSON() );
}

void checkTelegramIfNeeded() {
  TBMessage msg;
  userEventMessage_t message;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {

    if (msg.messageType == MessageText){
        // Received a text message
        myBot.sendMessage(msg, "Welcome!", mainKeyboard);
      } // msgType == MessageText
    } // myBot.getNewMessage(msg)

  // Check if there are messages in the telegramQueue
  // This must still go un if Telegram is disabled, otherwise it cannot be enabled again
  if ( xQueueReceive( telegramQueue, &message, 0) == pdPASS ) {
    Serial.printf("%s > Telegram [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed, telegramEnabled() ? "Y" : "N");

    switch( message.command ) {
      case cmdDisableTelegram:
        disableTelegram();
      break;

      case cmdEnableTelegram:
        enableTelegram();
      break;

      default:
        String chatID = message.chatID;
        if( chatID.length()>1 ) {
          // Serial.printf("Handle queue command for chatID: %s\n", chatID);
          // userConversation[chatID].respondToUser( bot, message );
        }
      }; // switch( message.command )
  }; // xQueueReceive
};
