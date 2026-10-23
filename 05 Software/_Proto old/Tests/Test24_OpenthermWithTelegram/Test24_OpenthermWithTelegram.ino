#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <OpenTherm.h>
#invlude "MyCredentials.h"

const int inPin  = 21;  
const int outPin = 22; 
OpenTherm ot(inPin, outPin);

float temperatureSetpoint = 20;

WiFiClientSecure secured_client;
UniversalTelegramBot bot(token, secured_client);

void ICACHE_RAM_ATTR handleInterrupt() {
    ot.handleInterrupt();
}

void handleNewMessages(int numNewMessages)
{
  for (int i = 0; i < numNewMessages; i++)
  {
    bot.sendMessage(bot.messages[i].chat_id, bot.messages[i].text, "");
  }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");

    ot.begin(handleInterrupt);

  OpenThermResponseStatus status = ot.getLastResponseStatus();
  Serial.printf("Status: %s\n",ot.statusToString(status));
}

void loop()
{
    //Set/Get Boiler Status
    bool enableCentralHeating = true;
    bool enableHotWater = true;
    bool enableCooling = false;

    unsigned long response = ot.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
    OpenThermResponseStatus responseStatus = ot.getLastResponseStatus();
    Serial.printf("Status: %s\n",ot.statusToString(responseStatus));

    if (responseStatus == OpenThermResponseStatus::SUCCESS) {
        Serial.print("Central Heating: " + String(ot.isCentralHeatingActive(response) ? "on" : "off"));
        Serial.print(" Hot Water: " + String(ot.isHotWaterActive(response) ? "on" : "off"));
        Serial.print(" Flame: " + String(ot.isFlameOn(response) ? "on" : "off"));
    }
    if (responseStatus == OpenThermResponseStatus::NONE) {
        Serial.print("Error: OpenTherm is not initialized");
    }
    else if (responseStatus == OpenThermResponseStatus::INVALID) {
        Serial.print("Error: Invalid response " + String(response, HEX));
    }
    else if (responseStatus == OpenThermResponseStatus::TIMEOUT) {
        Serial.print("Error: Response timeout");
    }

    //Set Boiler Temperature to 64 degrees C
    ot.setBoilerTemperature(30);

    //Get Boiler Temperature
    float ch_temperature = ot.getBoilerTemperature();
    Serial.print(" CH temperature is " + String(ch_temperature) + " degrees C");

    //Set DHW setpoint to 40 degrees C
    ot.setDHWSetpoint(40);

    //Get DHW Temperature
    float dhw_temperature = ot.getDHWTemperature();
    Serial.println(" DHW temperature is " + String(dhw_temperature) + " degrees C");

    delay(2500);
}
