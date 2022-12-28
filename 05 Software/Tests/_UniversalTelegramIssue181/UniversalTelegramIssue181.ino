#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SPIFFS.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Wifi network station credentials
#define WIFI_SSID "..."
#define WIFI_PASSWORD "..."
#define BOT_TOKEN "..."

#define TELEGRAM_DEBUG true

#define FORMAT_SPIFFS_IF_FAILED false

unsigned long bytesRead = 0;
const unsigned long BOT_MTBS = 1000; // mean time between scan messages

unsigned long bot_lasttime;          // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

File myFile;
bool isMoreDataAvailable();
byte getNextByte();

bool isMoreDataAvailable() { return myFile.available(); }
byte getNextByte() { bytesRead++; return myFile.read(); }

void handleNewMessages(int numNewMessages)
{
  String chat_id = bot.messages[0].chat_id;
  String file_name = "/box.jpg";

  String welcome = "Test message\n";
  bot.sendMessage(chat_id, welcome);

  myFile = SPIFFS.open(file_name);
  listDir(SPIFFS, "/", 0);

  if (myFile)
  {
    Serial.print(file_name);
    Serial.print("....");

    //Content type for PNG image/png
    String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", myFile.size(),
                                        isMoreDataAvailable,
                                        getNextByte, nullptr, nullptr);

    if (sent) {
      Serial.println("was successfully sent");
    }
    else {
      Serial.println("was not sent");
    }

    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening photo");
  }

  Serial.printf("Bytes read: %d\n", bytesRead);

  bytesRead=0;
  
  file_name = "/Logfile.csv";

  myFile = SPIFFS.open(file_name);

  if (myFile)
  {
    Serial.print(file_name);
    Serial.print("....");

    //Content type for PNG image/png
    String sent = bot.sendTextByBinary(chat_id, "text/html", myFile.size(), isMoreDataAvailable, getNextByte, nullptr, nullptr);

    if (sent) {
      Serial.println("was successfully sent");
    }
    else {
      Serial.println("was not sent");
    }

    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening photo");
  }
  Serial.printf("Bytes read: %d\n", bytesRead);

}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
  }

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
  if (millis() - bot_lasttime > BOT_MTBS)
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
