/*******************************************************************
 *  An example of how to use a custom reply keyboard markup.       *
 *                                                                 *
 *                                                                 *
 *  written by Brian Lough                                         *
 *******************************************************************/
#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define TELEGRAM_DEBUG 1

#include <UniversalTelegramBot.h>
#include "SPIFFS.h"

#include "a_Constants.h"
#include "f_Wifi.h"
#include "g_Time.h"


Data data;

WiFiClientSecure secured_client;
UniversalTelegramBot bot("", secured_client);
int ledStatus = 0;
TFT_eSPI tft = TFT_eSPI();

void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    Serial.printf("ChatID: %s\n", chat_id);
    
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon")
    {
      tft.fillScreen(TFT_RED);
      ledStatus = 1;
      bot.sendMessage(chat_id, "Led is ON", "");
    }

    if (text == "/ledoff")
    {
      ledStatus = 0;
      tft.fillScreen(TFT_BLACK);
      bot.sendMessage(chat_id, "Led is OFF", "");
    }

    if (text == "/status")
    {
      if (ledStatus)
      {
        bot.sendMessage(chat_id, "Led is ON", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Led is OFF", "");
      }
    }

    if (text == "/options")
    {
      String keyboardJson = "[[\"Setpoint higher\", \"Setpoint lower\"],[\"Setpoint auto\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }

    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Reply Keyboard Markup example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      welcome += "/options : returns the reply keyboard\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup()
{
  // Initilize serial port
  Serial.begin(115200);

  // Initialize SPIFFS
  SPIFFS.begin();
  delay(500);

  // Load config data from SPIFFS
  data.loadConfig(SPIFFS, CONFIG_FILE);

  Serial.printf("Bot token: %s\n", data.botToken.c_str());
  bot.updateToken(data.botToken);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  // Init screen
  tft.init();  
  tft.fillScreen(CLR_BACKGROUND);

  // Initialize wifi
  setupWifi(data);

  data.wifiConnected = (WiFi.status() == WL_CONNECTED);

  // Check current daylight saving time for our timezone
  requestTimeZoneIfNeeded(data);

  // Sync with atomic clock
  syncTimeIfNeeded(data);

  
}

void loop() {
  static bool prevConn = false;
  static bool tryFirstTime = true;
  
  data.wifiConnected = (WiFi.status() == WL_CONNECTED);

  // Check if there is a change in the wireless connection
  if(data.wifiConnected != prevConn) {
    Serial.printf("WiFi connected: %s\n", data.wifiConnected ? "Y" : "N");
    prevConn=data.wifiConnected;

    if(data.wifiConnected) {
      Serial.printf("Get me result positive: %s\n", bot.getMe() ? "Y" : "N");
      Serial.printf("- name    : %s\n",  bot.name.c_str());
      Serial.printf("- userName: %s\n",  bot.userName.c_str());

      if(tryFirstTime) {
        Serial.printf("sendGetToTelegram: %s\n", bot.sendGetToTelegram("/test").c_str());
        tryFirstTime=false;
      }
    }
  }

  // If connected, make a one time request
  if(data.wifiConnected and tryFirstTime) {
    Serial.printf("sendGetToTelegram: %s\n", bot.sendGetToTelegram("/test").c_str());
    tryFirstTime=false;
  }
    
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  while (numNewMessages)
  {
    Serial.println("got response");
    handleNewMessages(numNewMessages);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }

  reconnectIfNeeded(data);
  requestTimeZoneIfNeeded(data);
  syncTimeIfNeeded(data);

  delay(800);
}
