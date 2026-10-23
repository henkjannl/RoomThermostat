#include <AsyncTelegram2.h>
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

#define CB_KEYBOARD1 "cbKb1"
#define CB_KEYBOARD2 "cbKb2"
#define CB_KEYBOARD3 "cbKb3"

// The secure wifi client
WiFiClientSecure client;
#ifdef ESP8266
#include <ESP8266WiFi.h>
  Session   session;
  X509List  certificate(telegram_cert);
#endif

// The Telegram bot
AsyncTelegram2 myBot(client);
InlineKeyboard keyboard1, keyboard2, keyboard3; 

// Callback function definitions for inline keyboard buttons
void onKeyboard1(const TBMessage &queryMsg){
  Serial.println("Keyboard 1 is selected");
  myBot.editMessage(queryMsg, "Keyboard 1 is selected", keyboard1 );    
  myBot.endQuery(queryMsg, "Keyboard 1 is selected", false);
}

void onKeyboard2(const TBMessage &queryMsg){
  Serial.println("Keyboard 2 is selected");
  myBot.editMessage(queryMsg, "Keyboard 2 is selected", keyboard2 );        
  myBot.endQuery(queryMsg, "Keyboard 2 is selected", false);
}

void onKeyboard3(const TBMessage &queryMsg){
  Serial.println("Keyboard 3 is selected");
  myBot.editMessage(queryMsg, "Keyboard 3 is selected", keyboard3 );    
  myBot.endQuery(queryMsg, "Keyboard 3 is selected", false);
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

  // Add three different keyboards
  keyboard1.addButton("Go to keyboard 2", CB_KEYBOARD2, KeyboardButtonQuery, onKeyboard2 );
  keyboard1.addButton("Go to keyboard 3", CB_KEYBOARD3, KeyboardButtonQuery, onKeyboard3 );

  keyboard2.addButton("Go to keyboard 1", CB_KEYBOARD1, KeyboardButtonQuery, onKeyboard1 );
  keyboard2.addButton("Go to keyboard 3", CB_KEYBOARD3, KeyboardButtonQuery, onKeyboard3 );

  keyboard3.addButton("Go to keyboard 1", CB_KEYBOARD1, KeyboardButtonQuery, onKeyboard1 );
  keyboard3.addButton("Go to keyboard 2", CB_KEYBOARD2, KeyboardButtonQuery, onKeyboard2 );
  
  // Set the Telegram bot properies
  myBot.setUpdateTime(1000);
  myBot.setTelegramToken(token);

  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  myBot.begin() ? Serial.println("OK") : Serial.println("NOK");

  myBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML /* MARKDOWN */);

  myBot.sendTo(userid, "Welcome!");
  myBot.addInlineKeyboard(&keyboard1);
  myBot.addInlineKeyboard(&keyboard2);
  myBot.addInlineKeyboard(&keyboard3);
}

// General loop function
void loop() {
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {

    if (msg.messageType == MessageText){
        // Received a text message
        myBot.sendMessage(msg, "Welcome to the dynamic keyboard demo.\nChoose a button.", keyboard1);
      } // msgType == MessageText
    } // myBot.getNewMessage(msg)
}; // loop
