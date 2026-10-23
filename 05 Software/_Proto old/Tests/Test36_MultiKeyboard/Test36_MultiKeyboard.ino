#include "FS.h"
#include "SPIFFS.h"
#include <AsyncTelegram2.h>
#include <DS18B20.h>
#include <map>
#include <WiFiClientSecure.h>

#include "MyCredentials.h"
/* The second file in the same folder as this sketch named "MyCredentials.h" contains:
const char* ssid  =  "xxxxxxxxx";     // SSID WiFi network
const char* pass  =  "xxxxxxxxx";     // Password  WiFi network
const char* token =  "xxxxxx:xxxxxxxxxxxxx";  // Telegram token

int64_t userid = 123456789;

I included MyCredentials.h to prevent the information from being uploaded to github (MyCredentials.h is in gitignore)
*/

// Timezone definition
#define MYTZ "CET-1CEST,M3.5.0,M10.5.0/3"

#define FORMAT_SPIFFS_IF_FAILED false
#define LOG_FILE "/templog.csv"
#define SEND_LOG_CALLBACK  "sendlog"
#define STATUS_CALLBACK    "status"
#define CLEAR_LOG_CALLBACK "clearlog"
#define SENSOR_OFFSET -6.6 // 18.1-24.7

#define CB_WARMER            "cbWarmer"
#define CB_AUTO              "cbAuto"
#define CB_COOLER            "cbCooler"
#define CB_OVERRULE_TODAY    "cbOverruleToday"
#define CB_OVERRULE_TOMORROW "cbOverruleTomorrow"
#define CB_OVERRULE_MULTIPLE "cbOverruleMultiple"
#define CB_SETTINGS          "cbSettings"
#define CB_STATUS            "cbStatus"
#define CB_WEEKLY_SCHEDULE   "cbWeeklySchedule"
#define CB_HOME_TIMES        "cbHomeTimes"
#define CB_OFFICE_TIMES      "cbOfficeTimes"
#define CB_WEEKEND_TIMES     "cbWeekendTimes"
#define CB_TEMPERATURES      "cbTemperatures"
#define CB_SENSOR_OFFSET     "cbSensorOffset"
#define CB_DEBUGGING         "cbDebugging"
#define CB_MAIN_MENU         "cbMainMenu"


const char EMOTICON_ALARM_CLOCK[]         = { 0xe2, 0x8f, 0xb0, 0x00 };
const char EMOTICON_BACK[]                = { 0xf0, 0x9f, 0x94, 0x99, 0x00 };
const char EMOTICON_BATH[]                = { 0xf0, 0x9f, 0x9b, 0x81, 0x00 };
const char EMOTICON_BED[]                 = { 0xf0, 0x9f, 0x9b, 0x8f, 0x00 };
const char EMOTICON_BEETLE[]              = { 0xf0, 0x9f, 0x90, 0x9e, 0x00 };
const char EMOTICON_BLACK_DOT[]           = { 0xe2, 0x9a, 0xab, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_BLUE_DOT[]            = { 0xf0, 0x9f, 0x94, 0xb5, 0x00 };
const char EMOTICON_BULLSEYE[]            = { 0xf0, 0x9f, 0x8e, 0xaf, 0x00 };
const char EMOTICON_CALENDAR[]            = { 0xf0, 0x9f, 0x93, 0x85, 0x00 };
const char EMOTICON_CAMERA[]              = { 0xf0, 0x9f, 0x93, 0xb7, 0x00 };
const char EMOTICON_CHECKERED_FLAG[]      = { 0xf0, 0x9f, 0x8f, 0x81, 0x00 };
const char EMOTICON_CHECKMARK[]           = { 0xe2, 0x9c, 0x85, 0x00 };
const char EMOTICON_CLIPBOARD[]           = { 0xf0, 0x9f, 0x93, 0x8b, 0x00 };
const char EMOTICON_CLOCK[]               = { 0xe2, 0x8f, 0xb0, 0x00 };
const char EMOTICON_CROSSMARK[]           = { 0xe2, 0x9d, 0x8c, 0x00 };
const char EMOTICON_DANCER[]              = { 0xf0, 0x9f, 0x92, 0x83, 0xf0, 0x9f, 0x8f, 0xbb, 0x00 };
const char EMOTICON_DOWN_ARROW[]          = { 0xe2, 0xac, 0x87, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_EARTH[]               = { 0xf0, 0x9f, 0x8c, 0x8d, 0x00 };
const char EMOTICON_FLAME[]               = { 0xf0, 0x9f, 0x94, 0xa5, 0x00 };
const char EMOTICON_FOOTSTEPS[]           = { 0xf0, 0x9f, 0x91, 0xa3, 0x00 };
const char EMOTICON_GEAR[]                = { 0xe2, 0x9a, 0x99, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_GLASSES[]             = { 0xf0, 0x9f, 0x91, 0x93, 0x00 };
const char EMOTICON_GREEN_DOT[]           = { 0xf0, 0x9f, 0x94, 0xb5, 0x00 };
const char EMOTICON_HAMMER_WRENCH[]       = { 0xf0, 0x9f, 0x9b, 0xa0, 0x00 };
const char EMOTICON_HOURGLASS[]           = { 0xe2, 0x8c, 0x9b, 0xef, 0xb8, 0x8f, 0x00 };
const char EMOTICON_HOUSE[]               = { 0xf0, 0x9f, 0x8f, 0xa0, 0x00 };
const char EMOTICON_ISLAND[]              = { 0xf0, 0x9f, 0x8f, 0x96, 0x00 };
const char EMOTICON_LAMP[]                = { 0xf0, 0x9f, 0x92, 0xa1, 0x00 };
const char EMOTICON_LEVEL_SLIDER[]        = { 0xf0, 0x9f, 0x8e, 0x9a, 0x00 };
const char EMOTICON_LIFEBUOY[]            = { 0xf0, 0x9f, 0x9b, 0x9f, 0x00 };
const char EMOTICON_MAGIC_STICK[]         = { 0xf0, 0x9f, 0xaa, 0x84, 0x00 };
const char EMOTICON_MINUS[]               = { 0xe2, 0x9e, 0x96, 0x00 };
const char EMOTICON_OFFICE[]              = { 0xf0, 0x9f, 0x8f, 0xa2, 0x00 };
const char EMOTICON_PIN[]                 = { 0xf0, 0x9f, 0x93, 0x8d, 0x00 };
const char EMOTICON_PLUS[]                = { 0xe2, 0x9e, 0x95, 0x00 };
const char EMOTICON_POINTING_FINGER[]     = { 0xf0, 0x9f, 0x91, 0x89, 0x00 };
const char EMOTICON_RED_DOT[]             = { 0xf0, 0x9f, 0x94, 0xb4, 0x00 };
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

// The secure wifi client
WiFiClientSecure client;
#ifdef ESP8266
#include <ESP8266WiFi.h>
  Session   session;
  X509List  certificate(telegram_cert);
#endif

// The Telegram bot
AsyncTelegram2 myBot(client);
InlineKeyboard mainKeyboard, settingsKeyboard; 

// The dallas sensor
DS18B20 ds(32);

// Logdata, with times and measured temperatures
std::map<time_t, float> TemperatureLog;

// Callback functions definition for inline keyboard buttons
void onWarmer(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Temperature up", mainKeyboard);    
}

void onAuto(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Temperature auto", mainKeyboard);    
}

void onCooler(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Temperature down", mainKeyboard);    
}

void onOverruleToday(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Overrule today menu", mainKeyboard);    
}

void onOverruleTomorrow(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Overrule totomorrow menu", mainKeyboard);    
}

void onOverruleMultiple(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Overrule multiple days menu", mainKeyboard);    
}

void onSettings(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Settings menu", settingsKeyboard);    
}

void onStatus(const TBMessage &queryMsg){
  float tempC = ds.getTempC()+SENSOR_OFFSET;
  String message = "Current temperature " + String(tempC, 3) + "°C";
  myBot.editMessage(queryMsg, message.c_str(), mainKeyboard);    
}

void onWeeklySchedule(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Weekly schedule menu", settingsKeyboard);    
}

void onHomeTimes(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Home times", settingsKeyboard);    
}

void onOfficeTimes(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Office times", settingsKeyboard);    
}

void onWeekendTimes(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Weekend times", settingsKeyboard);    
}

void onTemperatures(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Set temperatures menu", settingsKeyboard);    
}

void onSensorOffset(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Sensor offset", settingsKeyboard);    
}

void onDebugging(const TBMessage &queryMsg){
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
    myBot.sendMessage(queryMsg, "Log file sent", mainKeyboard);          
  }
  else
    Serial.println("Can't open the file. Upload \"data\" folder to filesystem");

  myBot.editMessage(queryMsg, "Log file sent", settingsKeyboard);    
}

void onMainMenu(const TBMessage &queryMsg){
  myBot.editMessage(queryMsg, "Back to the main menu", mainKeyboard);    
}

void onClearLog(const TBMessage &queryMsg){
  TemperatureLog.clear();
  Serial.println("Logfile cleared");
  myBot.editMessage(queryMsg, "Log file cleared", mainKeyboard);          
}

// General setup function
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
  Serial.print(ds.getTempC()+SENSOR_OFFSET);

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

  // Add main keyboard
  char buffer [100];
  snprintf( buffer, 100, "%s %s", EMOTICON_PLUS,         "Warmer"               ); mainKeyboard.addButton( buffer, CB_WARMER,            KeyboardButtonQuery, onWarmer           );
  snprintf( buffer, 100, "%s %s", EMOTICON_MAGIC_STICK,  "Auto"                 ); mainKeyboard.addButton( buffer, CB_AUTO,              KeyboardButtonQuery, onAuto             );
  snprintf( buffer, 100, "%s %s", EMOTICON_MINUS,        "Cooler"               ); mainKeyboard.addButton( buffer, CB_COOLER,            KeyboardButtonQuery, onCooler           );
  mainKeyboard.addRow();
  mainKeyboard.addButton("Overrule today...",          CB_OVERRULE_TODAY,    KeyboardButtonQuery, onOverruleToday    );
  mainKeyboard.addRow();
  mainKeyboard.addButton("Overrule tomorrow...",       CB_OVERRULE_TOMORROW, KeyboardButtonQuery, onOverruleTomorrow );
  mainKeyboard.addRow();
  mainKeyboard.addButton("Overrule multiple days...",  CB_OVERRULE_MULTIPLE, KeyboardButtonQuery, onOverruleMultiple );
  mainKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_GEAR,         "Settings..."          ); mainKeyboard.addButton( buffer, CB_SETTINGS,          KeyboardButtonQuery, onSettings         );
  snprintf( buffer, 100, "%s %s", EMOTICON_STHETOSCOPE,  "Status"               ); mainKeyboard.addButton( buffer, CB_STATUS,            KeyboardButtonQuery, onStatus           );

  // Add settings keyboard
  snprintf( buffer, 100, "%s %s", EMOTICON_CLIPBOARD,    "Weekly schedule..."   ); settingsKeyboard.addButton(buffer, CB_WEEKLY_SCHEDULE,  KeyboardButtonQuery, onWeeklySchedule    );
  snprintf( buffer, 100, "%s %s", EMOTICON_HOUSE,        "Home times..."        ); settingsKeyboard.addButton(buffer, CB_HOME_TIMES,       KeyboardButtonQuery, onHomeTimes         );
  settingsKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_OFFICE,       "Office times..."      ); settingsKeyboard.addButton(buffer, CB_OFFICE_TIMES,     KeyboardButtonQuery, onOfficeTimes       );
  snprintf( buffer, 100, "%s %s", EMOTICON_DANCER,       "Weekend times..."     ); settingsKeyboard.addButton(buffer, CB_WEEKEND_TIMES,    KeyboardButtonQuery, onWeekendTimes      );
  settingsKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_BULLSEYE,     "Temperatures..."      ); settingsKeyboard.addButton(buffer, CB_TEMPERATURES,     KeyboardButtonQuery, onTemperatures      );
  snprintf( buffer, 100, "%s %s", EMOTICON_LEVEL_SLIDER, "Sensor offset..."     ); settingsKeyboard.addButton(buffer, CB_SENSOR_OFFSET,    KeyboardButtonQuery, onSensorOffset      );
  settingsKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_LIFEBUOY,     "Debugging..."         ); settingsKeyboard.addButton(buffer, CB_DEBUGGING,        KeyboardButtonQuery, onDebugging         );
  settingsKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_BACK,         "Back to main menu..." ); settingsKeyboard.addButton(buffer, CB_MAIN_MENU,        KeyboardButtonQuery, onMainMenu          );

  // Add weekly schedule
  weekScheduleKeyboard.addButton("Monday",    CB_WSCH_MONDAY,    KeyboardButtonQuery, onWeeklySchedule    );
  weekScheduleKeyboard.addButton("Tuesday",   CB_WSCH_TUESDAY,   KeyboardButtonQuery, onWeeklySchedule    );
  settingsKeyboard.addRow();
  weekScheduleKeyboard.addButton("Wednesday", CB_WSCH_WDNESDAY,  KeyboardButtonQuery, onWeeklySchedule    );
  weekScheduleKeyboard.addButton("Thursday",  CB_WSCH_THURSDAY,  KeyboardButtonQuery, onWeeklySchedule    );
  settingsKeyboard.addRow();
  weekScheduleKeyboard.addButton("Friday",    CB_WSCH_FRIDAY,    KeyboardButtonQuery, onWeeklySchedule    );
  weekScheduleKeyboard.addButton("Saturday",  CB_WSCH_SATURDAY,  KeyboardButtonQuery, onWeeklySchedule    );
  settingsKeyboard.addRow();
  weekScheduleKeyboard.addButton("Sunday",    CB_WSCH_SUNDAY,    KeyboardButtonQuery, onWeeklySchedule    );
  settingsKeyboard.addRow();
  snprintf( buffer, 100, "%s %s", EMOTICON_GEAR, "Back to settings..." ); weekScheduleKeyboard.addButton(buffer, CB_SETTINGS, KeyboardButtonQuery, onDebugging         );
  
  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");
  myBot.addInlineKeyboard(&mainKeyboard);
  myBot.addInlineKeyboard(&settingsKeyboard);

  myBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML /* MARKDOWN */);

  // Send a welcome message to user when ready
  char welcome_msg[128];
  snprintf(welcome_msg, sizeof(welcome_msg),
          "BOT @%s online.\n/help for command list.",
          myBot.getBotName());

  myBot.sendTo(userid, welcome_msg);
}

// General loop function
void loop() {
  printHeapStats();

  static uint32_t lastMeasurement = 0;
  static uint32_t logInterval = 1000*3;
  if ( millis() - lastMeasurement > logInterval ) {
    lastMeasurement = millis();
    time_t now;
    time(&now); // Get current time
    float tempC = ds.getTempC()+SENSOR_OFFSET;
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
      if (msgText.indexOf("/log") > -1) onDebugging(msg);
        
      else if (msgText.indexOf("/clear") > -1) onClearLog(msg);

      else {
        myBot.sendMessage(msg, "Welcome to the temperature logger\nchoose a button", mainKeyboard);
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
  struct tm sysTime;
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
