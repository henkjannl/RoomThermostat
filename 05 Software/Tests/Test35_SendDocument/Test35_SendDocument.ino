#include "FS.h"
#include "SPIFFS.h"
#include <AsyncTelegram2.h>
#include <map>
#include <DS18B20.h>

#include "MyCredentials.h"
/* The second file in the same folder as this sketch named "MyCredentials.h" contains:
const char* ssid  =  "xxxxxxxxx";     // SSID WiFi network
const char* pass  =  "xxxxxxxxx";     // Password  WiFi network
const char* token =  "xxxxxx:xxxxxxxxxxxxx";  // Telegram token

// Check the userid with the help of bot @JsonDumpBot or @getidsbot (work also with groups)
// https://t.me/JsonDumpBot  or  https://t.me/getidsbot
int64_t userid = 123456789;
*/


// Timezone definition
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"
struct tm sysTime;

#define FORMAT_SPIFFS_IF_FAILED false
#define LOG_FILE "/templog.csv"

#include <WiFiClientSecure.h>
WiFiClientSecure client;
#ifdef ESP8266
#include <ESP8266WiFi.h>
  Session   session;
  X509List  certificate(telegram_cert);
#endif

AsyncTelegram2 myBot(client);
InlineKeyboard inlineKeyboard; 

DS18B20 ds(32);

std::map<time_t, float> TemperatureLog;

#define SEND_LOG_CALLBACK  "sendlog"
#define CLEAR_LOG_CALLBACK "clearlog"

// Callback functions definition for inline keyboard buttons
void onSendLog(const TBMessage &queryMsg){
  // Write logdata to CSV file
  Serial.println("\nCreating csv file");
  File file = SPIFFS.open(LOG_FILE, FILE_WRITE);
  file.print("\"Time\",\"Temperature\"\n");
  
  struct tm * localTime;
  for (auto const& datapoint : TemperatureLog) {
      localTime = localtime(& (datapoint.first) );
      file.printf("%04d-%02d-%02d %02d:%02d:%02d,%.6f\n", 
        localTime->tm_year+1900, localTime->tm_mon+1, localTime->tm_mday,
        localTime->tm_hour, localTime->tm_min, localTime->tm_sec, 
        datapoint.second ); 
  }
  
  file.close();
  
  Serial.println("\nSending csv file");
  file = SPIFFS.open(LOG_FILE, "r");
  if (file) {
    myBot.sendDocument(queryMsg, file, file.size(), AsyncTelegram2::DocumentType::CSV, file.name());
    file.close();
    myBot.sendMessage(queryMsg, "Log file sent", inlineKeyboard);          
  }
  else
    Serial.println("Can't open the file. Upload \"data\" folder to filesystem");
};

// Callback functions definition for inline keyboard buttons
void onClearLog(const TBMessage &queryMsg){
  TemperatureLog.clear();
  Serial.println("Logfile cleared");
  myBot.sendMessage(queryMsg, "Log file cleared", inlineKeyboard);          
}

void setup() {
  // initialize the Serial
  Serial.begin(115200);

  WiFi.setAutoConnect(true);
  WiFi.mode(WIFI_STA);

  // connects to access point
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());

  if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)){
      Serial.println("SPIFFS Mount Failed");
      return;
  }

  listDir("/", 0);

  ds.setResolution(RES_12_BIT);
  
  Serial.printf("Devices: %d\n", ds.getNumberOfDevices());
  Serial.print(ds.getTempC());

#ifdef ESP8266
  // Sync time with NTP, to check properly Telegram certificate
  configTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  //Set certficate, session and some other base client properies
  client.setSession(&session);
  client.setTrustAnchors(&certificate);
  client.setBufferSizes(1024, 1024);
#elif defined(ESP32)
  // Sync time with NTP
  configTzTime(MYTZ, "time.google.com", "time.windows.com", "pool.ntp.org");
  client.setCACert(telegram_cert);
#endif

  // Add inline keyboard
  // add a button that will turn on LED on pin assigned
  inlineKeyboard.addButton("Send logfile",  SEND_LOG_CALLBACK, KeyboardButtonQuery, onSendLog);
  // add a button that will turn off LED on pin assigned
  inlineKeyboard.addButton("Clear logfile", CLEAR_LOG_CALLBACK, KeyboardButtonQuery, onClearLog);
  Serial.printf("Added %d buttons to keyboard\n", inlineKeyboard.getButtonsNumber());

  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");
  myBot.addInlineKeyboard(&inlineKeyboard);

  myBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML /* MARKDOWN */);

  // Send a welcome message to user when ready
  char welcome_msg[128];
  snprintf(welcome_msg, sizeof(welcome_msg),
          "BOT @%s online.\n/help for command list.",
          myBot.getBotName());

  myBot.sendTo(userid, welcome_msg);
}



void loop() {
  printHeapStats();

  static uint32_t lastMeasurement = 0;
  static uint32_t logInterval = 1000*3;
  if ( millis() - lastMeasurement > logInterval ) {
    lastMeasurement = millis();
    time_t now;
    time(&now); // Get current time
    float tempC = ds.getTempC();
    TemperatureLog[now]=tempC;
    Serial.printf("Temperature: %.3f°C\n", tempC);
    if( TemperatureLog.size()>10 ) logInterval = 1000*60*3; // Go to lower frequency after some samples, for debugging.
  }

  // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    MessageType msgType = msg.messageType;

    // Received a text message
    if (msgType == MessageText){
      String msgText = msg.text;
      Serial.print("Text message received: ");
      Serial.println(msgText);

      // Send a csv file passing the stream
      if (msgText.indexOf("/log") > -1) onSendLog(msg);
        
      else if (msgText.indexOf("/clear") > -1) onClearLog(msg);

      else {
        myBot.sendMessage(msg, "Welcome to the temperature logger\nchoose a button", inlineKeyboard);
      }

    }
  }
}


// List all files saved in the selected filesystem
void listDir(const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\r\n", dirname);
  File root = SPIFFS.open(dirname, "r");
  if (!root) {
    Serial.println("- failed to open directory\n");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(" - not a directory\n");
    return;
  }
  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("  DIR : %s\n", file.name());
      if (levels)
        listDir(file.name(), levels - 1);
    }
    else
      Serial.printf("  FILE: %s\tSIZE: %d\n", file.name(), file.size());
    file = root.openNextFile();
  }
}

void printHeapStats() {
  static uint32_t infoTime;
  if (millis() - infoTime > 10000) {
    infoTime = millis();
    time_t now = time(nullptr);
    sysTime = *localtime(&now);
#ifdef ESP32
    //heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
    Serial.printf("%02d:%02d:%02d - Total free: %6d - Max block: %6d\n",
      sysTime.tm_hour, sysTime.tm_min, sysTime.tm_sec,
      heap_caps_get_free_size(0), heap_caps_get_largest_free_block(0) );
#elif defined(ESP8266)
    uint32_t free;
    uint32_t max;
    uint8_t frag;
    ESP.getHeapStats(&free, &max, &frag);
    Serial.printf("%02d:%02d:%02d - Total free: %5lu - Max block: %5lu - Frag: %5d\n",
      sysTime.tm_hour, sysTime.tm_min, sysTime.tm_sec, free, max, frag);
#endif
  }
}
