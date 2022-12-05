#include <TFT_eSPI.h>
#include <SPI.h>
#include <FS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <OpenTherm.h>

// Thermometer dallas DS18B20 connected to OneWire bus
#define ONE_WIRE_BUS 17
#define TEMPERATURE_PRECISION 11 // Resolution 0.125°C. Requires 375 ms to fetch temperature

// OpenTherm Shield pins configuration
#define OT_IN_PIN         21
#define OT_OUT_PIN        22

// Screen colors converted to RGB565
#define CLR_BACKGROUND       0xFFFF   // FF, FF, FF = white
#define CLR_CIRCLE_OUTLINE   0xCE59   // C8, C8, C8 = dark grey
#define CLR_CIRCLE_FILL      0xEF5D   // E8, E8, E8 = light grey
#define CLR_MEASURED_TEMP    0x0000   // 00, 00, 00 = black
#define CLR_DARK_TEXT        0x8410   // 80, 80, 80 = dark grey
#define CLR_LIGHT_TEXT       0xB596   // B0, B0, B0 = light grey
#define CLR_SETPOINT_TEMP    0xF8C3   // FF, 19, 19 = red
#define CLR_MENU_TITLE       0x0000   // 00, 00, 00 = black
#define CLR_MENU_SUBTITLE    0x8410   // 80, 80, 80 = dark grey
#define CLR_MENU_TEXT        0x0000   // 00, 00, 00 = black
#define CLR_MENU_LINE        0xF8C3   // FF, 19, 19 = red
#define CLR_BLUE_TEXT        0x1A7F   // 19, 4F, FF = blue
#define CLR_RED_TEXT         0xF8C3   // FF, 19, 19 = red

// Fonts
#define AA_FONT_SMALL "NotoSansBold15"
#define AA_FONT_LARGE "NotoSansBold36"

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);
DeviceAddress deviceAddress;
OpenTherm openthermDriver(OT_IN_PIN, OT_OUT_PIN);
TFT_eSPI tft = TFT_eSPI();

void ICACHE_RAM_ATTR openthermInterrupt() {
    openthermDriver.handleInterrupt();
}

void setup(void) {
  Serial.begin(115200);

  delay(500);
  SPIFFS.begin();

  tft.init();  
  tft.fillScreen(CLR_BACKGROUND);
  tft.loadFont(AA_FONT_SMALL);
  
  // Initialize thermometer
  sensor.begin();
  Serial.printf("%d device(s) found on OneWire bus\n", sensor.getDeviceCount());
  Serial.printf("Parasite power on OneWire bus is: %s\n", sensor.isParasitePowerMode() ? "ON": "OFF");
  if (!sensor.getAddress(deviceAddress, 0)) {
    Serial.println("Unable to find address for device 0");
  }
  else {
    Serial.print("Dallas 18B20 found on address:\t");
    for (uint8_t i = 0; i < 8; i++) Serial.printf("%02X ", deviceAddress[i]);
    Serial.println();
  }

  openthermDriver.begin(openthermInterrupt);  
}

void loop() {
  float temperature;
  char buffer[80];
  char status[12];
  bool enableCentralHeating = true;
  bool enableHotWater = true;
  bool enableCooling = false;
  float boilerTemperature = 50;
  bool isCentralHeatingActive = false;
  bool isHotWaterActive = false;
  bool isFlameOn = false;
  float actualBoilerTemperature = false;

  // Retrieve temperature  
  sensor.requestTemperatures();
  temperature = sensor.getTempC(deviceAddress);

  //Set/get Boiler Status
  unsigned long response = openthermDriver.setBoilerStatus(enableCentralHeating, enableHotWater, enableCooling);
  OpenThermResponseStatus responseStatus = openthermDriver.getLastResponseStatus();

  switch(responseStatus) {
    case OpenThermResponseStatus::NONE:
      strncpy(status, "None", 12);
    break;
    
    case OpenThermResponseStatus::SUCCESS:
      strncpy(status, "Success", 12);
      openthermDriver.setBoilerTemperature(boilerTemperature);
    break;
    
    case OpenThermResponseStatus::INVALID:
      strncpy(status, "Invalid", 12);
    break;
    
    case OpenThermResponseStatus::TIMEOUT:
      strncpy(status, "Timeout", 12);
    break;
  };
   
  isCentralHeatingActive   = openthermDriver.isCentralHeatingActive(response);
  isHotWaterActive         = openthermDriver.isHotWaterActive(response);
  isFlameOn                = openthermDriver.isFlameOn(response);
  actualBoilerTemperature  = openthermDriver.getBoilerTemperature();

  // Print everything on screen
  tft.setTextDatum(MC_DATUM);
  //tft.setTextFont(4);
  tft.setTextColor(CLR_DARK_TEXT, CLR_BACKGROUND);
  int x=77;
  int y=20;
  int dy=tft.fontHeight();

  snprintf(buffer, 80, "  T=%.3f  ", temperature);
  tft.drawString(buffer, x,  y); y+=dy;

  tft.drawString(status, x,  y); y+=dy;

  tft.drawString(isCentralHeatingActive ? "CH    ON" : "CH    OFF", x,  y); y+=dy;
  tft.drawString(isHotWaterActive       ? "DHW   ON" : "DHW   OFF", x,  y); y+=dy;
  tft.drawString(isFlameOn              ? "Flame ON" : "Flame OFF", x,  y); y+=dy;

  snprintf(buffer, 80, "  Treq=%.3f  ", boilerTemperature);
  tft.drawString(buffer, x,  y); y+=dy;

  snprintf(buffer, 80, "  Tact=%.3f  ", actualBoilerTemperature);
  tft.drawString(buffer, x,  y); y+=dy;

  delay(1000);
}
