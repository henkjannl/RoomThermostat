#pragma once
#include <TFT_eSPI.h>
#include <SPI.h>
#include <JPEGDecoder.h>
#include <math.h>

// Backlight
#define BACKLIGHT_TIME_OUT 60*1000

// setting PWM properties
static const uint8_t BACKLIGHT_BRIGHTNESS[] = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 11, 13, 15, 18, 22, 26, 31, 37, 45, 53, 63, 75, 90, 107, 127, 151, 180, 214, 255 }; 

class Backlight {
  public:
    Backlight(uint8_t pin, DataBus & databus) { this->pin = pin; this->databus = & databus; };
    void begin(uint8_t backlightLedChannel=0, uint16_t backlightFreq=5000, uint8_t backlightResolution=8);
    bool alreadyOn();
    void loop();

  protected:
    uint8_t pin;
    DataBus *databus;

    uint8_t backlightLedChannel;
    uint16_t backlightFreq;
    uint8_t backlightResolution;

    bool backlightOn;
    unsigned long lastTimeKeyPressed = 0;
};

void Backlight::begin(uint8_t backlightLedChannel, uint16_t backlightFreq, uint8_t backlightResolution) {
  this->backlightLedChannel = backlightLedChannel;
  this->backlightFreq       = backlightFreq;
  this->backlightResolution = backlightResolution;

  // configure LED PWM functionalitites
  ledcSetup(backlightLedChannel, backlightFreq, backlightResolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pin, backlightLedChannel);
}

bool Backlight::alreadyOn() {
  bool result = backlightOn;
  backlightOn=true;
  lastTimeKeyPressed = millis();
  return result;  
}

void Backlight::loop() {
  static unsigned long previousAttempt = 0;
  static uint8_t backlightIndex = 0;
  static int interval = 100;

  // Put backlight in off state if no key was pressed for some time
  if (millis() - lastTimeKeyPressed > BACKLIGHT_TIME_OUT) {
    backlightOn=false;
  }

  if (millis() - previousAttempt > interval){
    
    databus->logBusyTime.start(btBacklight);  
    
    previousAttempt=millis();
    if(backlightOn) {
      // Make the backlight brighter
      interval=10;
      if(BACKLIGHT_BRIGHTNESS[backlightIndex]<255) 
        backlightIndex++;
      else
        interval=1000;
    }
    else {
      // Dim the backlight
      interval=100;
      if(backlightIndex>0) 
        backlightIndex--;
      else
        interval=1000;
    }

    // Delay Telegram unless backlight is fully dark
    databus->delayTelegram = (backlightIndex>0);
    ledcWrite(backlightLedChannel, BACKLIGHT_BRIGHTNESS[backlightIndex]);

    databus->logBusyTime.finish(btBacklight);  
  }
  
}

/*********************************************************
 *  Display class                                        *
 *********************************************************/
class Display {
  public:
    Display(TFT_eSPI & tft, DataBus & databus, MenuStructure & menu, keyboardFunction disableKeyboard, keyboardFunction enableKeyboard);
    void begin();
    void loop();
    void displayHomeScreen();
    void displayMenuScreen();

  protected:
    TFT_eSPI *tft;  
    DataBus *databus;
    MenuStructure *menu;
    keyboardFunction disableKeyboard;  // pointer to the function that disables the keyboard
    keyboardFunction enableKeyboard;   // pointer to the function that disables the keyboard

    umScreenIdentifier_t previousScreen = scAdjustSensorOffset;
};

Display::Display(TFT_eSPI & tft, DataBus & databus, MenuStructure & menu, keyboardFunction disableKeyboard, keyboardFunction enableKeyboard) { 
  // Connect the right pointers but do not initialize yet
  this->tft             = & tft; 
  this->databus         = & databus; 
  this->menu            = & menu;
  this->disableKeyboard = disableKeyboard; 
  this->enableKeyboard  = enableKeyboard; 
};

void Display::begin() {

  // Initialize screen
  disableKeyboard();
  tft->init();
  tft->setRotation(3);
  tft->fillScreen(CLR_LIGHT_TEXT);
  tft->setTextDatum(MC_DATUM);
  tft->loadFont(FONT_TEMP_MEASURED); 
  tft->setTextColor(CLR_DARK_TEXT, CLR_LIGHT_TEXT);
  tft->drawString("Welcome", 160, 120);
  enableKeyboard();
}

void Display::loop() {  
  Serial.println("Display::loop()");

  databus->logBusyTime.start(btDisplay);  
  
  // Redraw background if screen is changed
  if(menu->selectedScreen() != previousScreen) {
    if( menu->selectedScreen()==scHomeScreen ) {
      disableKeyboard();
      drawJpeg(tft, "/HomeScreen.jpg", 0,  0);
      enableKeyboard();
      databus->invalidateAll();
    }
    else {
      disableKeyboard();
      tft->fillScreen(CLR_BACKGROUND);
      enableKeyboard();
    };
    previousScreen=menu->selectedScreen(); 
  };

  // Redraw foreground
  if( menu->selectedScreen()==scHomeScreen )
    displayHomeScreen(); 
  else 
    displayMenuScreen();

  databus->logBusyTime.finish(btDisplay);  
} 

void Display::displayHomeScreen() {
  TFT_eSprite sprite = TFT_eSprite(tft);
    
  if (menu->selectedScreen()==scHomeScreen) {
    Serial.println("Display::displayHomeScreen()");

    if(databus->dpTemperatureSetpoint.isChanged()) {
      disableKeyboard();
      sprite.createSprite(56, 32);
      sprite.fillSprite(CLR_CIRCLE_FILL);    
      sprite.loadFont(FONT_TIME);
      sprite.setTextDatum(MR_DATUM);
      sprite.setTextColor(databus->dpOverrideTempNow.getValue() ? CLR_RED_TEXT : CLR_DARK_TEXT, CLR_CIRCLE_FILL);
      sprite.drawString(databus->dpTemperatureSetpoint.getValue(), 56, 16);
      sprite.pushSprite(38, 109);
      sprite.unloadFont();
      sprite.deleteSprite();
      enableKeyboard();
  
      databus->dpTemperatureSetpoint.drawn();
    };
    
    if(databus->dpMeasuredTemperature.isChanged()) {
      disableKeyboard();

      sprite.createSprite(116, 64);
      sprite.fillSprite(CLR_CIRCLE_FILL);    
      sprite.loadFont(FONT_TEMP_MEASURED);
      sprite.setTextDatum(MR_DATUM);
      sprite.setTextColor(CLR_MEASURED_TEMP, CLR_CIRCLE_FILL);
      sprite.drawString(databus->dpMeasuredTemperature.getValue(), 115, 32);
      sprite.pushSprite(130, 88);
      sprite.unloadFont();
      sprite.deleteSprite();

      enableKeyboard();

      databus->dpMeasuredTemperature.drawn();
    };

    if(databus->dpDate.isChanged()) {
      const int W=288;
      const int H=26;
      disableKeyboard();
      sprite.createSprite(W, H);
      sprite.fillSprite(CLR_BACKGROUND);    
      sprite.setTextDatum(MC_DATUM);
      sprite.loadFont(FONT_DATE); 
      sprite.setTextColor(CLR_DARK_TEXT, CLR_BACKGROUND);
      sprite.drawString(databus->dpDate.getValue(), W/2, H/2);
      sprite.pushSprite(0, 4);
      sprite.unloadFont();
      sprite.deleteSprite();
      enableKeyboard();
      databus->dpDate.drawn();
    };
  
    if(databus->dpTime.isChanged()) {
      disableKeyboard();
      sprite.createSprite(80, 32);
      sprite.fillSprite(CLR_CIRCLE_FILL);    
      sprite.setTextDatum(MC_DATUM);
      sprite.loadFont(FONT_TIME);
      sprite.setTextColor(CLR_DARK_TEXT, CLR_CIRCLE_FILL);
      sprite.drawString(databus->dpTime.getValue(), 40, 16);
      sprite.pushSprite(18, 52);
      sprite.unloadFont();
      sprite.deleteSprite();
      enableKeyboard();
    };
      
    if ( databus->dpWifiIcon.isChanged() ) { 
      disableKeyboard(); 
      drawJpeg(tft, ICONS[databus->dpWifiIcon.getValue()].c_str(), 15, 166);  
      enableKeyboard(); 
      databus->dpWifiIcon.drawn();         
    };
      
    if ( databus->dpBoilerConnIcon.isChanged() ) { 
      disableKeyboard(); 
      drawJpeg(tft, ICONS[databus->dpBoilerConnIcon.getValue()].c_str(), 47, 166);  
      enableKeyboard(); 
      databus->dpBoilerConnIcon.drawn();   
    };
    
    if ( databus->dpFlameIcon.isChanged() ) { 
      disableKeyboard(); 
      drawJpeg(tft, ICONS[databus->dpFlameIcon.getValue() ].c_str(), 83, 168);  
      enableKeyboard(); 
      databus->dpFlameIcon.drawn();       
    };
    
    if (databus->dpShowerIcon.isChanged() ) { 
      disableKeyboard(); 
      drawJpeg(tft, ICONS[databus->dpShowerIcon.getValue() ].c_str(),107, 166);  
      enableKeyboard(); 
      databus->dpShowerIcon.drawn();       
    };
  
    int x=8;
    for(int i=0; i<7; i++) {
      if(databus->dpWeekProgramIcons[i].isChanged()) { 
        disableKeyboard(); 
        drawJpeg(tft, ICONS[databus->dpWeekProgramIcons[i].getValue()].c_str(), x, 208);
        enableKeyboard(); 
        databus->dpWeekProgramIcons[i].drawn();
      }
      x+=40;
    };      
  };
};

void Display::displayMenuScreen() {
  TFT_eSprite sprite = TFT_eSprite(tft);

  Serial.println("Display::displayMenuScreen()");
  disableKeyboard(); 

  sprite.createSprite(320, 24);

  sprite.fillSprite(CLR_BACKGROUND);    
  sprite.setTextColor(CLR_MENU_TITLE, CLR_BACKGROUND);
  sprite.loadFont(FONT_MENU_HEADER); 
  sprite.setTextDatum(TL_DATUM);
  sprite.drawString(menu->currentScreenTitle().c_str(), 2, 2);
  sprite.pushSprite(0, 0);

  int y=sprite.height()+4;
  
  if(databus->showSubMenuLines>0) {
    sprite.fillSprite(CLR_BACKGROUND);    
    sprite.loadFont(FONT_SUBMENU); 
    sprite.setTextColor(CLR_MENU_SUBTITLE, CLR_BACKGROUND);
    sprite.drawString(databus->subMenuLine1, 2, 0);
    sprite.pushSprite(0, y);
    y+=sprite.height();    
  };

  if(databus->showSubMenuLines>1) {
    sprite.fillSprite(CLR_BACKGROUND);    
    sprite.drawString(databus->subMenuLine2, 2, 0);
    sprite.pushSprite(0, y);
    y+=sprite.height();
  };

  sprite.loadFont(FONT_SUBMENU); 
  int newH = sprite.fontHeight()+4;

  sprite.unloadFont();
  sprite.deleteSprite();

  sprite.createSprite(320, newH);
  sprite.loadFont(FONT_SUBMENU); 

  // Determine if the menu needs to scroll up
  int y2 = y;
  int firstItem=0;
  for (auto& menuItem : menu->menuItems() ) {
    y2+=sprite.height();   
    if(y2>240) firstItem++; 
    if(menuItem==menu->selectedMenuItem()) {
      break;
    }
  }

  int itemNr=0;
  for (auto& menuItem : menu->menuItems() ) {
    if(itemNr>=firstItem) {
      sprite.fillSprite(CLR_BACKGROUND);    
      if(menuItem==menu->selectedMenuItem()) {
        sprite.setTextColor(CLR_MENU_SELECT, CLR_BACKGROUND);
        sprite.drawString(">", 0, 2);
      }
      else
        sprite.setTextColor(CLR_MENU_TEXT, CLR_BACKGROUND);
  
      sprite.drawString(umLabel[menuItem].c_str(), 12, 2);
      sprite.pushSprite(0, y);
      y+=sprite.height();    
    }
    itemNr++;
  }

  sprite.unloadFont();
  sprite.deleteSprite();

  enableKeyboard(); 
};
