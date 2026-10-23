#include "FS.h"
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "time.h"
#include <HTTPClient.h>

#define CONFIG_FILE "/config.jsn"

WiFiMulti wifiMulti;
const int led = 2;
  
void setup() {
  Serial.begin(115200);
  delay(300);

  SPIFFS.begin();

  // Retrieve configuration data from the configuration file stored in SPIFFS 
  StaticJsonDocument<1024> doc;

  File input = SPIFFS.open( CONFIG_FILE );
  
  DeserializationError error = deserializeJson(doc, input);
  
  if (error) {
    Serial.print(F("deserializeJson() of configfile failed: "));
    Serial.println(error.f_str());
    return;
  }
      
  for (JsonObject elem : doc["AccessPoints"].as<JsonArray>()) {
    char ssid[64];
    char password[64];
    strlcpy(ssid     ,elem["SSID"],    sizeof(ssid    ) );
    strlcpy(password ,elem["password"],sizeof(password) );
    wifiMulti.addAP(ssid, password);
  }
  
  Serial.println("Config loaded");  

  wifiMulti.run();

  if(wifiMulti.run() == WL_CONNECTED) {
    Serial.println("WiFi connected IP address: ");
    Serial.println( WiFi.localIP() );
  }
      
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(led, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();

  static unsigned long previousMillis;
  static bool ledState;
  
  if (millis() - previousMillis >= 200) {
    // save the last time you blinked the LED
    previousMillis = millis();

    // if the LED is off turn it on and vice-versa:
    ledState = not(ledState);
    // set the LED with the ledState of the variable:
    digitalWrite(led,  ledState);
  }  
}
