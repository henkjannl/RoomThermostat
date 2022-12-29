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
StaticJsonDocument<200> outer;

String jsonString;

int val = 1;

void setup() {
  Serial.begin(115200);

  Serial.println("Reached before WiFi init");
  WiFi.begin(WifiAccessPoint, WifiPassword);

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

  Serial.println("Waiting to connect to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  jsonString = "";

}

String timeNow() {
  time_t now;
  time(&now);
  struct tm * local;
  local = localtime(&now);
  char buffer[20];
  snprintf(buffer, 20, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
  return String(buffer);
};

void loop() {
  static bool initialized = false;
  String tn;
  float temperature;
  StaticJsonDocument<50> internal;
  
  if( !initialized ) {
    Serial.println("Let's begin initialization");
    JsonArray items = outer.createNestedArray("items");
    tn = timeNow();
    internal["time"] = tn;
    temperature = random( 15.0, 25.0 );
    internal["temperature"] = temperature;
    items.add(internal);
    serializeJson(outer, jsonString);
    Serial.printf("jsonString: %s\n", jsonString);
    initialized=true;
  }
  
  StaticJsonDocument<120> receiving;

  temperature = random( 15.0, 25.0 );
  tn = timeNow();
  Serial.println(tn);
  outer["items"][0]["time"] = tn;
  outer["items"][0]["temperature"] = temperature;
  serializeJson(outer, jsonString);
  Serial.printf("jsonString: %s\n", jsonString);

  result myResult = detaObj.putObject(jsonString.c_str());
  Serial.println();
  Serial.println(myResult.statusCode);
  Serial.println(myResult.reply);
  deserializeJson(receiving, myResult.reply);
  const char* assignedKey = receiving["processed"]["items"][0]["key"];
  val = receiving["processed"]["items"][0]["time"];

  Serial.print("Key assigned by Deta:\t");
  Serial.println(assignedKey);
  Serial.print("Value received:\t");
  Serial.println(val);

  jsonString = "";
  delay(10000);
}
