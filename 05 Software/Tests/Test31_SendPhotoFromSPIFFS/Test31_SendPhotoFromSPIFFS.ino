/*******************************************************************
    A telegram bot for your ESP32 that demonstrates sending an image
    from SD.

    Parts:
    ESP32 D1 Mini stlye Dev board* - http://s.click.aliexpress.com/e/C6ds4my
    (or any ESP32 board)

      = Affilate

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Example originally written by Vadim Sinitski 

    Library written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "FS.h"
#include "SPIFFS.h"
#include "MyCredentials.h"

#define FORMAT_SPIFFS_IF_FAILED false
#define PHOTOJPG "/Photo.jpg"
#define PHOTOPNG "/Photo.png"
#define SMALLJPG "/Small.jpg"
#define LOGFILE "/Logfile.csv"


const unsigned long BOT_MTBS = 1000; // mean time between scan messages

unsigned long bot_lasttime;          // last time messages' scan has been done
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

File myFile;
bool isMoreDataAvailable();
byte getNextByte();

bool isMoreDataAvailable()
{
  return myFile.available();
}

byte getNextByte()
{
  return myFile.read();
}

void sendPhoto( String chat_id, const char * photo ) {

  myFile = SPIFFS.open(photo);

  if (myFile)
  {
    Serial.printf("Sending %s to chat %s\n", photo , chat_id.c_str() );

    //Content type for PNG image/png
    String sent = bot.sendPhotoByBinary(chat_id, "image/jpeg", myFile.size(),
                                        isMoreDataAvailable,
                                        getNextByte, nullptr, nullptr);

    Serial.println(sent ? "was successfully sent" : "was not sent");
    myFile.close();
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening photo");
  }
}
void handleNewMessages(int numNewMessages)
{
  String chat_id = bot.messages[0].chat_id;
  bot.sendMessage(chat_id, "Small JPG photo coming up", "");
  sendPhoto( chat_id, SMALLJPG );
/*     
  bot.sendMessage(chat_id, "JPG photo coming up", "");
  sendPhoto( chat_id, PHOTOJPG );
  bot.sendMessage(chat_id, "PNG photo coming up", "");
  sendPhoto( chat_id, PHOTOPNG );
*/
  bot.sendMessage(chat_id, "Sending of photos ready", "");

  myFile = SPIFFS.open(SMALLJPG);
  for(int i = 0; i<20; i++ ) {
    Serial.printf("%02d) %02X > %s\n", i, myFile.read(), isMoreDataAvailable() ? "OK" : "X" );
  }
  myFile.close();
  
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

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  bot.maxMessageLength = 10000;

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
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

  listDir(SPIFFS, "/", 0);
  readFile(SPIFFS, LOGFILE);
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
