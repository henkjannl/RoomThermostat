#define ARDUINOJSON_DECODE_UNICODE 1

#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <list>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

using namespace std;

#define CONFIG_FILE "/config_test.jsn"

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
 };


WiFiMulti wifiMulti;
WiFiClientSecure secured_client;
UniversalTelegramBot bot("", secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done

const char EMOTICON_ALARM_CLOCK[]         = { 0xe2, 0x8f, 0xb0, 0x00 };
const char EMOTICON_BACK[]                = { 0xf0, 0x9f, 0x94, 0x99, 0x00 };
const char EMOTICON_BATH[]                = { 0xf0, 0x9f, 0x9b, 0x81, 0x00 };
const char EMOTICON_BED[]                 = { 0xf0, 0x9f, 0x9b, 0x8f, 0x00 };
const char EMOTICON_BEETLE[]              = { 0xf0, 0x9f, 0x90, 0x9e, 0x00 };
const char EMOTICON_BULLSEYE[]            = { 0xf0, 0x9f, 0x8e, 0xaf, 0x00 };
const char EMOTICON_CALENDAR[]            = { 0xf0, 0x9f, 0x93, 0x85, 0x00 };
const char EMOTICON_CAMERA[]              = { 0xf0, 0x9f, 0x93, 0xb7, 0x00 };
const char EMOTICON_CHECKERED_FLAG[]      = { 0xf0, 0x9f, 0x8f, 0x81, 0x00 };
const char EMOTICON_CHECKMARK[]           = { 0xe2, 0x9c, 0x85, 0x00 };
const char EMOTICON_CLIPBOARD[]           = { 0xf0, 0x9f, 0x93, 0x8b, 0x00 };
const char EMOTICON_CLOCK[]               = { 0xe2, 0x8f, 0xb0, 0x00 };
const char EMOTICON_CROSSMARK[]           = { 0xe2, 0x9d, 0x8c, 0x00 };
const char EMOTICON_DOWN_ARROW[]          = { 0xe2, 0xac, 0x87, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_EARTH[]               = { 0xf0, 0x9f, 0x8c, 0x8d, 0x00 };
const char EMOTICON_FLAME[]               = { 0xf0, 0x9f, 0x94, 0xa5, 0x00 };
const char EMOTICON_FOOTSTEPS[]           = { 0xf0, 0x9f, 0x91, 0xa3, 0x00 };
const char EMOTICON_GEAR[]                = { 0xe2, 0x9a, 0x99, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_GLASSES[]             = { 0xf0, 0x9f, 0x91, 0x93, 0x00 };
const char EMOTICON_HAMMER_WRENCH[]       = { 0xf0, 0x9f, 0x9b, 0xa0, 0x00 };
const char EMOTICON_HOURGLASS[]           = { 0xe2, 0x8c, 0x9b, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_HOUSE[]               = { 0xf0, 0x9f, 0x8f, 0xa0, 0x00 };
const char EMOTICON_ISLAND[]              = { 0xf0, 0x9f, 0x8f, 0x96, 0x00 };
const char EMOTICON_LAMP[]                = { 0xf0, 0x9f, 0x92, 0xa1, 0x00 };
const char EMOTICON_LIFEBUOY[]            = { 0xf0, 0x9f, 0x9b, 0x9f  };
const char EMOTICON_MAGIC_STICK[]         = { 0xf0, 0x9f, 0xaa, 0x84, 0x00 };
const char EMOTICON_OFFICE[]              = { 0xf0, 0x9f, 0x8f, 0xa2, 0x00 };
const char EMOTICON_PIN[]                 = { 0xf0, 0x9f, 0x93, 0x8d, 0x00 };
const char EMOTICON_POINTING_FINGER[]     = { 0xf0, 0x9f, 0x91, 0x89, 0x00 };
const char EMOTICON_RED_QUESTION_MARK[]   = { 0xe2, 0x9d, 0x93, 0x00 };
const char EMOTICON_RUNNER[]              = { 0xf0, 0x9f, 0x8f, 0x83, 0x00 };
const char EMOTICON_SATTELITE_DISH[]      = { 0xf0, 0x9f, 0x93, 0xa1, 0x00 };
const char EMOTICON_SHOWER[]              = { 0xf0, 0x9f, 0x9a, 0xbf, 0x00 };
const char EMOTICON_SMILEY[]              = { 0xf0, 0x9f, 0x98, 0x80, 0x00 };
const char EMOTICON_SNOWFLAKE[]           = { 0xe2, 0x9d, 0x84, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_SPIDERWEB[]           = { 0xf0, 0x9f, 0x95, 0xb8, 0x00 };
const char EMOTICON_SPEECH_BALLOON[]      = { 0xf0, 0x9f, 0x92, 0xac, 0x00 };
const char EMOTICON_STHETOSCOPE[]         = { 0xf0, 0x9f, 0xa9, 0xba, 0x00 };
const char EMOTICON_STOPWATCH[]           = { 0xe2, 0x8f, 0xb1, 0x00 };
const char EMOTICON_SUN[]                 = { 0xf0, 0x9f, 0x8c, 0x9e, 0x00 };
const char EMOTICON_THERMOMETER[]         = { 0xf0, 0x9f, 0x8c, 0xa1, 0x00 };
const char EMOTICON_TRIANGLE_DOWN[]       = { 0xf0, 0x9f, 0x94, 0xbb, 0x00 };
const char EMOTICON_TRIANGLE_UP[]         = { 0xf0, 0x9f, 0x94, 0xba, 0x00 };
const char EMOTICON_UP_ARROW[]            = { 0xe2, 0xac, 0x86, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_WARNING[]             = { 0xe2, 0x9a, 0xa0, 0xef, 0xb8, 0x8f, 0x00 };

String convertToHexString( String input ) {
  String result = "{ ";
  for( int i=0; i<input.length(); i++ ) {
    result += String("0x") + String( (int) input[i], HEX ) + ", ";
  }
  result+="0x00 };";
  result.toLowerCase();
  Serial.println(input + ": " + result);
  return result;
}
void handleNewMessages(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {    
    bot.sendMessage(bot.messages[i].chat_id, convertToHexString( bot.messages[i].text ), "");
  }
}

void setup() {
  // WiFi
  list<tAccessPoint> AccessPoints;
  
  // TELEGRAM
  // Telegram bot settings
  String botName;
  String botUserName;
  String botToken;
  String botChatID;
  bool locationKnown;
  char timezone[64];

  Serial.begin(115200);
  SPIFFS.begin();
  delay(500);

  Serial.println();

  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  File input = SPIFFS.open(CONFIG_FILE);
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    Serial.print(F("deserializeJson() of configfile failed: "));
    Serial.println(error.f_str());
    return;
  }
      
  for (JsonObject elem : doc["AccessPoints"].as<JsonArray>()) {
    tAccessPoint AccessPoint;
    strlcpy(AccessPoint.ssid     ,elem["SSID"],    sizeof(AccessPoint.ssid    ));
    strlcpy(AccessPoint.password ,elem["password"],sizeof(AccessPoint.password));
    strlcpy(AccessPoint.timezone ,elem["timezone"],sizeof(AccessPoint.timezone));          
    AccessPoints.push_back(AccessPoint);  
  }
  
  botName=    doc["BotName"    ].as<String>();
  botUserName=doc["BotUsername"].as<String>();
  botToken=   doc["BotToken"   ].as<String>();
  botChatID=  doc["ChatID"     ].as<String>();

  // Connect to wifi
  for(auto accessPoint : AccessPoints) {
    wifiMulti.addAP(accessPoint.ssid, accessPoint.password);
    Serial.printf("Adding accesspoint:  %s\n", accessPoint.ssid );
  }

  Serial.print("Waiting for a WiFi connection");

  while(wifiMulti.run()!=WL_CONNECTED) {
    Serial.print(".");
    delay(350);
  }
  Serial.println("");
  
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  bot.updateToken(botToken);
  
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

  bot.sendMessage(botChatID, String(EMOTICON_POINTING_FINGER) + " welcome to the emotiocon echo", "");
}

void loop() {

  if (millis() - bot_lasttime > BOT_MTBS) {
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
