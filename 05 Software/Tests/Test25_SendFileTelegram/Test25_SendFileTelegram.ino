
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"
#include "MyCredentials.h" // Excluded by gitignore, contains wifi credentials and bot token

WiFiClientSecure secured_client;
UniversalTelegramBot bot(token, secured_client);

File myFile;
bool isMoreDataAvailable();
byte getNextByte();

bool isMoreDataAvailable()
{
  static unsigned long i = 0;
  if( (i++ % 100) == 0) Serial.print("D");
  
  return myFile.available();
}

byte getNextByte()
{
  static unsigned long i = 0;
  if( (i++ % 100) == 0) Serial.print("B");
  return myFile.read();
}


void handleNewMessages(int numNewMessages)
{
  String chat_id = bot.messages[0].chat_id;

  String file_name = "/photo.jpg";
  myFile = SPIFFS.open(file_name);

  if (myFile) {
    Serial.print(file_name);
    Serial.print("....");
    Serial.print(myFile.size());

    String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", myFile.size(), isMoreDataAvailable, getNextByte, nullptr, nullptr);

    Serial.println(sent);
    Serial.println(sent ? "was successfully sent" : "was not sent");
    myFile.close();
  }

  else Serial.println("error opening file");

  file_name = "/Logfile.csv";
  myFile = SPIFFS.open(file_name);

  if (myFile) {
    Serial.print(file_name);
    Serial.print("....");
    Serial.print(myFile.size());

    String sent = bot.sendFileByBinary(chat_id, "image/jpeg", myFile.size(), isMoreDataAvailable, getNextByte, nullptr, nullptr);

    Serial.println(sent);
    Serial.println(sent ? "was successfully sent" : "was not sent");
    myFile.close();
  }
  else Serial.println("error opening file");  

  bot.sendMessage(chat_id, "Files should have been sent", "");
  
}


void setup()
{
  Serial.begin(115200);
  Serial.println();

  if(!SPIFFS.begin()){
      Serial.println("SPIFFS Mount Failed");
      return;
  }

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  static unsigned long bot_lasttime;          
   
  if (millis() - bot_lasttime > 1000)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
