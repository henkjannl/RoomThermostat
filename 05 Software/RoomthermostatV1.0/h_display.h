#pragma once
#include <TFT_eSPI.h>
#include <SPI.h>
#include <JPEGDecoder.h>
#include <math.h>

/*********************************************************
 *  Display class                                        *
 *********************************************************/
class Display {
  public:
    Display(TFT_eSPI & tft, ControllerData_t & controllerData, screen_t (*selectedScreen)(), command_t (*selectedMenuItem)() );
    void begin();
    void processMessage(userEventMessage_t message);
    void displayHomeScreen(userEventMessage_t message);
    void displayMenuScreen(userEventMessage_t message);

  protected:
    TFT_eSPI *tft;  
    ControllerData_t *controllerData;
    screen_t (*selectedScreen)(); 
    command_t (*selectedMenuItem)();
    screen_t previousScreen = scnSettingsSensorOffset;
};

Display::Display(TFT_eSPI & tft, ControllerData_t & controllerData, screen_t (*selectedScreen)(), command_t (*selectedMenuItem)() ) { 
  // Connect pointers but do not initialize yet
  this->tft              = & tft; 
  this->controllerData   = & controllerData; 
  this->selectedScreen   = selectedScreen;    // This is a pointer to 
  this->selectedMenuItem = selectedMenuItem;
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

void Display::processMessage(userEventMessage_t message) {
  
  Serial.printf("Display::processMessage() screen: %s command: %s\n", screenTitle[selectedScreen()].c_str(), commandLabels[selectedMenuItem()].c_str() );


  // The backlight flipping to the home screen should not wake up the backlight
  // The roiler requesting a transmit icon should also not wake up the backlight
  if( (message.command != cmdMenuHome) and ( message.sender != sndBoiler ) and ( message.sender != sndThermometer ) and ( message.sender!=sndWifi) ) {
    // Request backlight to switch on
    userEventMessage_t msg = userEventMessage_t(sndDisplay, cmdBacklightOn);
    xQueueSend( backlightQueue, &msg, ( TickType_t ) 10 );
  }
  
  if(message.command==cmdBoilerSending) {
    Serial.println("Display: Icon boiler transmitting");
    if( selectedScreen()==scnHome ) drawJpeg(tft, ICONS[iconBoilerTransmit].c_str(), 47, 166);  
    return;
  }

  // Redraw background if screen is changed
  if(selectedScreen() != previousScreen) {
    if( selectedScreen()==scnHome ) {
      drawJpeg(tft, "/HomeScreen.jpg", 0,  0);
    }
    else {
      tft->fillScreen(CLR_BACKGROUND);
      Serial.println("Filled screen for display of menu");
    };
    previousScreen=selectedScreen(); 
  };

  // Redraw foreground
  if( selectedScreen()==scnHome )
    displayHomeScreen(message); 
  else 
    displayMenuScreen(message);

  //controllerData->logBusyTime.finish(btDisplay);  
} 

void Display::displayHomeScreen(userEventMessage_t message) {
  TFT_eSprite sprite = TFT_eSprite(tft);
  const int BUFLEN = 80;
  char buffer[BUFLEN];
    
  if (selectedScreen()==scnHome) {
    Serial.println("Display::displayHomeScreen()");

    time_t now;
    time(&now); // Get current UTC time
    struct tm * localTime;
    localTime = localtime(&now); // Convert UTC to local time

    // Display date on top of screen
    const int W=288;
    const int H=26;
    sprite.createSprite(W, H);
    sprite.fillSprite(CLR_BACKGROUND);    
    sprite.setTextDatum(MC_DATUM);
    sprite.loadFont(FONT_DATE); 
    sprite.setTextColor(CLR_DARK_TEXT, CLR_BACKGROUND);
    strftime(buffer, BUFLEN, "%A %e %B", localTime);
    sprite.drawString(buffer, W/2, H/2);
    sprite.pushSprite(0, 4);
    sprite.unloadFont();
    sprite.deleteSprite();

    // Display time
    sprite.createSprite(80, 32);
    sprite.fillSprite(CLR_CIRCLE_FILL);    
    sprite.setTextDatum(MC_DATUM);
    sprite.loadFont(FONT_TIME);
    sprite.setTextColor(CLR_DARK_TEXT, CLR_CIRCLE_FILL);
    strftime(buffer, BUFLEN, "%R", localTime);
    sprite.drawString(buffer, 40, 16);
    sprite.pushSprite(18, 52);
    sprite.unloadFont();
    sprite.deleteSprite();
    
    // Display temperature setpoint
    sprite.createSprite(56, 32);
    sprite.fillSprite(CLR_CIRCLE_FILL);    
    sprite.loadFont(FONT_TIME);
    sprite.setTextDatum(MR_DATUM);
    sprite.setTextColor(controllerData->overrideTempNow ? CLR_RED_TEXT : CLR_DARK_TEXT, CLR_CIRCLE_FILL);
    snprintf(buffer, BUFLEN, "%.1f", controllerData->temperatureSetpoint );
    sprite.drawString(buffer, 56, 16);
    sprite.pushSprite(38, 109);
    sprite.unloadFont();
    sprite.deleteSprite();
    
    // Display measured temperature
    sprite.createSprite(116, 64);
    sprite.fillSprite(CLR_CIRCLE_FILL);    
    sprite.loadFont(FONT_TEMP_MEASURED);
    sprite.setTextDatum(MR_DATUM);
    sprite.setTextColor(CLR_MEASURED_TEMP, CLR_CIRCLE_FILL);
    snprintf(buffer, BUFLEN, "%.1f", controllerData->measuredTemperature() );      
    sprite.drawString(buffer, 115, 32);
    sprite.pushSprite(130, 88);
    sprite.unloadFont();
    sprite.deleteSprite();

    // Icons
    drawJpeg(tft, ICONS[controllerData->dspWifiIcon  ].c_str(), 15, 166);  
    drawJpeg(tft, ICONS[controllerData->dspBoilerIcon].c_str(), 47, 166);  
    drawJpeg(tft, ICONS[controllerData->dspFlameIcon ].c_str(), 83, 168);  
    drawJpeg(tft, ICONS[controllerData->dspShowerIcon].c_str(),107, 166);  

    // Day icons
    int x=8;
    for(int i=0; i<7; i++) {
      drawJpeg(tft, ICONS[controllerData->dayIcons[i]].c_str(), x, 208);
      x+=40;
    }
  };      
};

void Display::displayMenuScreen(userEventMessage_t message) {
  TFT_eSprite sprite = TFT_eSprite(tft);

  int showSubMenuLines=0;
  char subMenuLine1[MAX_LENGTH_SUBMENU];
  char subMenuLine2[MAX_LENGTH_SUBMENU];
  struct tm * timeinfo;
  time_t now;
  char buffer[32];

  Serial.printf("Display::displayMenuScreen() %s > %s\n", screenTitle[selectedScreen()].c_str(), commandLabels[selectedMenuItem()].c_str() );

  // Determine which subtitles to display
  switch(selectedScreen()) {
    case scnOverruleMultiple: 
      showSubMenuLines = 2; 
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "%s for %d days", DAY_TYPES[controllerData->overrideMultiple].c_str(), controllerData->overrideMultipleCount);

      time(&now);                    // Get current UTC time
      timeinfo = localtime(&now);    // Get local time
      timeinfo->tm_mday += controllerData->overrideMultipleCount; // Add some days
      mktime(timeinfo);            
      strftime(buffer, 32, "%A %e %B", timeinfo);
      snprintf(subMenuLine2, MAX_LENGTH_SUBMENU, "last day is %s", buffer);
      break;

    case scnSettingsTemperature: 
      showSubMenuLines = 2; 
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "High temperature %.1f°C", controllerData->highTemp);
      snprintf(subMenuLine2, MAX_LENGTH_SUBMENU, "Low temperature %.1f°C",  controllerData->lowTemp);

    case scnSettingsSensorOffset: 
      showSubMenuLines = 2; 
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "Sensor offset %.1f°C", controllerData->sensorOffset);
      snprintf(subMenuLine2, MAX_LENGTH_SUBMENU, "Measured temperature %.1f°C",  controllerData->measuredTemperature() );
      break;

    case scnSettingsHomeTimes: 
      showSubMenuLines=1;
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", controllerData->workFromHomeWakeUp.str(), controllerData->workFromHomeSleep.str());
    break;

    case scnSettingsOfficeTimes: 
      showSubMenuLines=2;
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s go out %s", controllerData->workAtOfficeWakeUp.str(), controllerData->workAtOfficeGoOut.str());
      snprintf(subMenuLine2, MAX_LENGTH_SUBMENU, "come home %s sleep %s", controllerData->workAtOfficeComeIn.str(), controllerData->workAtOfficeSleep.str());
      break;

    case scnSettingsWeekendTimes: 
      showSubMenuLines=2;
      snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", controllerData->weekendWakeUp.str(), controllerData->weekendSleep.str());
      break;

    default:
      showSubMenuLines = 0;
  }

  sprite.createSprite(320, 24);

  // Draw the titles to display
  sprite.fillSprite(CLR_BACKGROUND);    
  sprite.setTextColor(CLR_MENU_TITLE, CLR_BACKGROUND);
  sprite.loadFont(FONT_MENU_HEADER); 
  sprite.setTextDatum(TL_DATUM);
  sprite.drawString(screenTitle[selectedScreen()].c_str(), 2, 2);
  sprite.pushSprite(0, 0);

  int y=sprite.height()+4;
  
  if(showSubMenuLines>0) {
    sprite.fillSprite(CLR_BACKGROUND);    
    sprite.loadFont(FONT_SUBMENU); 
    sprite.setTextColor(CLR_MENU_SUBTITLE, CLR_BACKGROUND);
    sprite.drawString(subMenuLine1, 2, 0);
    sprite.pushSprite(0, y);
    y+=sprite.height();    
  };

  if(showSubMenuLines>1) {
    sprite.fillSprite(CLR_BACKGROUND);    
    sprite.drawString(subMenuLine2, 2, 0);
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
  for (auto& menuItem : SCREENS[selectedScreen()] ) {
    y2+=sprite.height();   
    if(y2>240) firstItem++; 
    if(menuItem==selectedMenuItem()) {
      break;
    }
  }

  int itemNr=0;
  for (auto& menuItem : SCREENS[selectedScreen()] ) {

    if(itemNr>=firstItem) {
      sprite.fillSprite(CLR_BACKGROUND);    
      if(menuItem==selectedMenuItem()) {
        sprite.setTextColor(CLR_MENU_SELECT, CLR_BACKGROUND);
        sprite.drawString(">", 0, 2);
      }
      else
        sprite.setTextColor(CLR_MENU_TEXT, CLR_BACKGROUND);
  
      sprite.drawString(commandLabels[menuItem].c_str(), 12, 2);
      sprite.pushSprite(0, y);
      y+=sprite.height();    
    }
    itemNr++;
  }

  sprite.unloadFont();
  sprite.deleteSprite();
};

/******************************************
 * GLOBAL VARIABLES                       * 
 ******************************************/
TFT_eSPI tft = TFT_eSPI();            // Driver for display
Display display(tft, controllerData, &selectedScreen, &selectedMenuItem); // Display

void checkDisplayQueue() {
  userEventMessage_t message;
  static bool backlightOn = true;

  if( xQueueReceive( displayQueue, &message, 0) == pdPASS ) {

    Serial.printf("%s > Display [%s] k=%d keyboardMode: %s\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed, keyboardMode ? "Y" : "N");

    // Wait until the display gets exclusive access to the controller data 
    if( xSemaphoreTake( controllerDataSemaphore, 1000/portTICK_PERIOD_MS ) == pdPASS ) {
      
      disableKeyboard(); // Disable keyboard so display can use SPI
      display.processMessage(message);
      enableKeyboard();  // Enable keyboard again

      xSemaphoreGive( controllerDataSemaphore );
    } else {
      Serial.println("Time out for controllerDataSemaphore in checkDisplayQueue");
      TaskHandle_t mutexHolder = xSemaphoreGetMutexHolder( controllerDataSemaphore );    
      char *pcTaskName = pcTaskGetTaskName(mutexHolder);
      Serial.printf("Mutex held by %s\n", pcTaskName);
    }
  }
}


// Start the the display handler
void startDisplay() {
  display.begin();
}
