#include "MyCredentials.h" 
#include <ArduinoJson.h>
#include <detaBaseArduinoESP32.h>
#include <WiFiClientSecure.h>
#include "time.h"

/*
MyCredentials.h is in gitignore, it contains:

const char* WifiAccessPoint = "...";
const char* WifiPassword = "...";
char* apiKey = "...";
char* detaID = "...";
char* detaBaseName = "...";

 */

WiFiClientSecure client;
DetaBaseObject detaObj(client, detaID, detaBaseName, apiKey, true);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("WiFi init");
  WiFi.begin(WifiAccessPoint, WifiPassword);

  Serial.println("Waiting to connect to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("Synching with time server");
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

String timeNow() {
  time_t now;
  struct tm * local;
  char buffer[40];

  time(&now);
  local = localtime(&now);
  snprintf(buffer, 40, "%04d-%02d-%02d_%02d:%02d:%02d", local->tm_year, local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
  return String(buffer);
};

void loop() {
  time_t now;
  static bool initialized = false;
  static StaticJsonDocument<200> outer;
  static StaticJsonDocument<100> internal;
  StaticJsonDocument<120> receiving;
  static String jsonString = "";

  time(&now);
  
  if( !initialized ) {
    // Build the json structure
    JsonArray items = outer.createNestedArray("items");
    internal["time"] =        time(&now);
    internal["temperature"] = (float) 0.01*random( 1500, 2500 );
    internal["boiler"] =      (float) 0.01*random( 1500, 2500 );
    internal["boilerOn"] =    (bool) true;
    items.add(internal);
    initialized=true;
  }
  
  outer["items"][0]["time"] = time(&now);
  outer["items"][0]["temperature"] = (float) 0.01*random( 1500, 2500 );
  outer["items"][0]["boiler"]      = (float) 0.01*random( 1500, 2500 );
  outer["items"][0]["boilerOn"]    = (bool) ( random(10) > 5 );
  outer["items"][0]["showerOn"]    = (bool) ( random(10) > 5 );
  serializeJson(outer, jsonString);
  Serial.printf("jsonString:\t%s\n", jsonString.c_str() );

  result myResult = detaObj.putObject(jsonString.c_str());
  deserializeJson(receiving, myResult.reply);
  Serial.printf("statusCode:\t%d\tresult:\t%s\n", myResult.statusCode, myResult.reply.c_str() );
  //const char* assignedKey = receiving["processed"]["items"][0]["key"];
  //Serial.printf("Status code: %s Reply: %s Key: %s\n", myResult.statusCode, myResult.reply, assignedKey);

  jsonString="";

  delay(5000);
}
