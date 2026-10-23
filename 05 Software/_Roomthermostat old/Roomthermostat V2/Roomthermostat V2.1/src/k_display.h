#pragma once
#include <TFT_eSPI.h>
#include <SPI.h>
#include <JPEGDecoder.h>
#include <math.h>
#include "font_assets.h"

/*********************************************************
 *  Display class                                        *
 *********************************************************/
class Display {
  public:
    Display(TFT_eSPI & tft, ControllerData_t & controllerData, screen_t (*selectedScreen)(), command_t (*selectedMenuItem)() );
    void start();
    void processMessage(const userEventMessage_t message);
    void displayHomeScreen();
    void service();
    bool pending = true;
    int phase = 0;
    int menuPhase = -1;
    int backgroundRow = 240;
    screen_t previousScreen = scnSettingsSensorOffset;
    char shownText[4][80] = {};
    int shownIcons[11] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    bool shownOverride = false;
    bool shownBorders[7] = {};
    void invalidate() {
      memset(shownText, 0, sizeof(shownText));
      for (int &icon : shownIcons) icon = -1;
    }

    void displayMenuScreen();

  protected:
    TFT_eSPI *tft;
    ControllerData_t *controllerData;
    screen_t (*selectedScreen)();
    command_t (*selectedMenuItem)();
};

Display::Display(TFT_eSPI & tft, ControllerData_t & controllerData, screen_t (*selectedScreen)(), command_t (*selectedMenuItem)() ) {
  // Connect pointers but do not initialize yet
  this->tft              = & tft;
  this->controllerData   = & controllerData;
  this->selectedScreen   = selectedScreen;    // This is a pointer to the menu structure to obtain the current screen
  this->selectedMenuItem = selectedMenuItem;  // This is a pointer to the menu structure to obtain the current menu item
};

void Display::start() {
  // Initialize screen
  disableKeyboard();
  tft->init();
  tft->setRotation(3);
  tft->fillScreen(CLR_BACKGROUND);
  tft->setTextDatum(MC_DATUM);
  tft->loadFont(font_Noway_Medium60);

  tft->setTextColor(CLR_LIGHT_TEXT, CLR_BACKGROUND);
  tft->drawString("Welcome", 160, 120);
  tft->unloadFont(); // The welcome screen must not retain a second smooth-font allocation.
  enableKeyboard();
}

void Display::processMessage(const userEventMessage_t message) {
  pending = true; // Coalesce repeated requests; never redraw in queue dispatch.
}

void Display::service() {
  unsigned long started = micros();
  if (selectedScreen() != previousScreen) {
    previousScreen = selectedScreen();
    invalidate();
    phase = 0;
    pending = true;
    backgroundRow = selectedScreen() == scnHome ? 0 : 240;
    if (selectedScreen() != scnHome) tft->fillScreen(CLR_BACKGROUND);
  }
  if (selectedScreen() == scnHome) {
    if (backgroundRow < 240) {
      const FlashImage *background = findFlashImage("/HomeScreen.jpg");
      if (background) drawFlashRows(tft, *background, 0, 0, backgroundRow, 8);
      backgroundRow += 8;
    } else if (pending || phase != 0) {
      if (phase == 0) pending = false;
      displayHomeScreen();
      phase = (phase + 1) % 15;
    }
  } else if (pending || menuPhase >= 0) {
    if (pending) { pending = false; menuPhase = 0; }
    displayMenuScreen();
  }
  displayMaxStepUs = max(displayMaxStepUs, micros() - started);
}

void Display::displayHomeScreen() {
  if (phase < 4) {
    char text[80];
    time_t now = time(nullptr);
    tm local;
    localtime_r(&now, &local);
    bool validTime = controllerData->timeSynched && now > 1609459200;
    switch (phase) {
      case 0: if (validTime) strftime(text, sizeof(text), "%A %e %B", &local);
              else strlcpy(text, "Waiting for time", sizeof(text)); break;
      case 1: if (validTime) strftime(text, sizeof(text), "%R", &local);
              else strlcpy(text, "--:--", sizeof(text)); break;
      case 2: snprintf(text, sizeof(text), "%.1f", controllerData->temperatureSetpoint); break;
      default: if (controllerData->temperatureMeasurementOK)
                 snprintf(text, sizeof(text), "%.1f", controllerData->measuredTemperature());
               else strlcpy(text, "--.-", sizeof(text)); break;
    }
    bool colorChanged = phase == 2 && shownOverride != controllerData->overrideTempNow;
    if (!colorChanged && strcmp(text, shownText[phase]) == 0) return;
    const int widths[] = {292,80,56,116}, heights[] = {30,32,32,64};
    const int xs[] = {0,118,137,100}, ys[] = {0,42,147,79};
    TFT_eSprite sprite(tft);
    if (!sprite.createSprite(widths[phase], heights[phase])) {
      ++displayAllocationFailures;
      pending = true;
      return;
    }
    sprite.fillSprite(CLR_BACKGROUND);
    unsigned long operationStarted = micros();
    if (phase == 0) sprite.loadFont(font_Noway_Light24);
    else if (phase == 3) sprite.loadFont(font_Noway_Medium60);
    else sprite.loadFont(font_Noway_Medium24);
    displayMaxFontLoadUs = max(displayMaxFontLoadUs, micros() - operationStarted);
    serviceDisplayIO();
    sprite.setTextDatum(phase < 2 ? MC_DATUM : MR_DATUM);
    uint16_t color = phase == 3 ? CLR_MEASURED_TEMP :
      phase == 2 && controllerData->overrideTempNow ? CLR_RED_TEXT : CLR_DARK_TEXT;
    sprite.setTextColor(color, CLR_BACKGROUND);
    operationStarted = micros();
    sprite.drawString(text, phase < 2 ? widths[phase]/2 : widths[phase]-1, heights[phase]/2);
    displayMaxTextDrawUs = max(displayMaxTextDrawUs, micros() - operationStarted);
    serviceDisplayIO();
    operationStarted = micros();
    sprite.pushSprite(xs[phase], ys[phase]);
    displayMaxSpritePushUs = max(displayMaxSpritePushUs, micros() - operationStarted);
    sprite.unloadFont();
    sprite.deleteSprite();
    strlcpy(shownText[phase], text, sizeof(shownText[phase]));
    if (phase == 2) shownOverride = controllerData->overrideTempNow;
    return;
  }
  int index = phase - 4;
  int icon;
  bool border = false;
  if (index < 4) {
    const icon_t icons[] = {controllerData->dspWifiIcon, controllerData->dspBoilerIcon,
                           controllerData->dspFlameIcon, controllerData->dspShowerIcon};
    icon = icons[index];
  } else {
    bool valid = controllerData->timeSynched && controllerData->dayIconsValid && time(nullptr) > 1609459200;
    icon = valid ? (int)DAYTYPE_TO_ICON[controllerData->dayTypes[index-4]] : -2;
    border = valid && controllerData->dayTypeOverruled[index-4];
  }
  if (icon == shownIcons[index] && (index < 4 || border == shownBorders[index-4])) return;
  if (index < 4) drawJpeg(tft, ICONS[(icon_t)icon].c_str(), 6, 42+36*index);
  else {
    int x = (index-4)*40;
    // A 40-pixel cell does not erase the adjacent day's border.
    tft->fillRect(x,194,40,42,CLR_BACKGROUND);
    if (border) {
      tft->fillSmoothRoundRect(x,194,40,42,8,CLR_RED_TEXT,CLR_BACKGROUND);
      tft->fillSmoothRoundRect(x+2,196,36,38,6,CLR_BACKGROUND,CLR_RED_TEXT);
    }
    if (icon >= 0) drawJpeg(tft, ICONS[(icon_t)icon].c_str(), x+5,199);
    shownBorders[index-4] = border;
  }
  shownIcons[index] = icon;
}

void Display::displayMenuScreen() {
  TFT_eSprite sprite = TFT_eSprite(tft);

  int showSubMenuLines=0;
  char subMenuLine1[MAX_LENGTH_SUBMENU];
  char subMenuLine2[MAX_LENGTH_SUBMENU];
  struct tm timeinfoStorage;
  struct tm *timeinfo = &timeinfoStorage;
  time_t now;
  char buffer[32];

  Serial.printf("Display::displayMenuScreen() %s > %s\n", screenTitle[selectedScreen()].c_str(), commandLabels[selectedMenuItem()].c_str() );

  // Determine which subtitles to display
  switch( selectedScreen() ) {

    case scnOverruleMultiple:
      if( controllerData->multipleForever) {
        showSubMenuLines = 1;
        snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "%s forever", DAY_TYPES[controllerData->overrideMultiple].c_str() );
      }
      else {
        showSubMenuLines = 2;

        snprintf(subMenuLine1, MAX_LENGTH_SUBMENU, "%s for %d days", DAY_TYPES[controllerData->overrideMultiple].c_str(), controllerData->overrideMultipleCount);

        time(&now);                    // Get current UTC time
        localtime_r(&now, timeinfo);    // Get local time into caller-owned storage
        timeinfo->tm_mday += controllerData->overrideMultipleCount-1; // controllerData->overrideMultipleCount = 1 means today
        mktime(timeinfo);
        strftime(buffer, 32, "%A %e %B", timeinfo);
        snprintf( subMenuLine2, MAX_LENGTH_SUBMENU, "last day is %s", buffer);
      }
      break;

    case scnSettingsTemperature:
      showSubMenuLines = 2;
      snprintf( subMenuLine1, MAX_LENGTH_SUBMENU, "High temperature %.1f°C", controllerData->highTemp);
      snprintf( subMenuLine2, MAX_LENGTH_SUBMENU, "Low temperature %.1f°C",  controllerData->lowTemp);
      break;

    case scnSettingsSensorOffset:
      showSubMenuLines = 2;
      snprintf( subMenuLine1, MAX_LENGTH_SUBMENU, "Sensor offset %.1f°C", controllerData->sensorOffset);
      snprintf( subMenuLine2, MAX_LENGTH_SUBMENU, "Measured temperature %.1f°C",  controllerData->measuredTemperature() );
      break;

    case scnSettingsHomeTimes:
      showSubMenuLines=1;
      snprintf( subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", controllerData->workFromHomeWakeUp.str().c_str(), controllerData->workFromHomeSleep.str().c_str() );
      break;

    case scnSettingsOfficeTimes:
      showSubMenuLines=2;
      snprintf( subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s go out %s",  controllerData->workAtOfficeWakeUp.str().c_str(), controllerData->workAtOfficeGoOut.str().c_str() );
      snprintf( subMenuLine2, MAX_LENGTH_SUBMENU, "come home %s sleep %s", controllerData->workAtOfficeComeIn.str().c_str(), controllerData->workAtOfficeSleep.str().c_str() );
      break;

    case scnSettingsWeekendTimes:
      showSubMenuLines=1;
      snprintf( subMenuLine1, MAX_LENGTH_SUBMENU, "Wake up %s sleep %s", controllerData->weekendWakeUp.str().c_str(), controllerData->weekendSleep.str().c_str() );
      break;

    default:
      showSubMenuLines = 0;
  }

  // Render one title/subtitle/menu row per main-loop iteration.
  bool title = menuPhase == 0;
  unsigned long operationStarted = micros();
  if (title) sprite.loadFont(font_Noway_Medium24);
  else sprite.loadFont(font_Noway_Light18);
  displayMaxFontLoadUs = max(displayMaxFontLoadUs, micros() - operationStarted);
  serviceDisplayIO();
  int itemHeight = sprite.fontHeight()+4;
  int height = menuPhase <= showSubMenuLines ? 24 : itemHeight;
  int y = 0;
  const char *text = nullptr;
  uint16_t color = CLR_MENU_TITLE;
  bool selected = false;
  if (title) text = screenTitle[selectedScreen()].c_str();
  else if (menuPhase <= showSubMenuLines) {
    y = 28 + (menuPhase-1)*24;
    text = menuPhase == 1 ? subMenuLine1 : subMenuLine2;
    color = CLR_MENU_SUBTITLE;
  } else {
    int startY = 28 + showSubMenuLines*24;
    int firstItem = 0, y2 = startY;
    for (auto item : SCREENS[selectedScreen()]) {
      y2 += itemHeight;
      if (y2 > 240) ++firstItem;
      if (item == selectedMenuItem()) break;
    }
    int row = menuPhase - showSubMenuLines - 1;
    y = startY + row*itemHeight;
    int n = 0;
    for (auto item : SCREENS[selectedScreen()]) {
      if (n++ == firstItem+row) {
        text = commandLabels[item].c_str();
        selected = item == selectedMenuItem();
        break;
      }
    }
    color = selected ? CLR_MENU_SELECT : CLR_MENU_TEXT;
    if (!text || y+height > 240) {
      if (y < 240) tft->fillRect(0,y,320,240-y,CLR_BACKGROUND);
      sprite.unloadFont();
      menuPhase = -1;
      return;
    }
  }
  if (!sprite.createSprite(320,height)) {
    ++displayAllocationFailures;
    sprite.unloadFont();
    return; // Retry this row without dereferencing an absent sprite buffer.
  }
  sprite.fillSprite(CLR_BACKGROUND);
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(color,CLR_BACKGROUND);
  operationStarted = micros();
  if (selected) sprite.drawString(">",0,2);
  sprite.drawString(text,menuPhase > showSubMenuLines ? 12 : 2,2);
  displayMaxTextDrawUs = max(displayMaxTextDrawUs, micros() - operationStarted);
  serviceDisplayIO();
  operationStarted = micros();
  sprite.pushSprite(0,y);
  displayMaxSpritePushUs = max(displayMaxSpritePushUs, micros() - operationStarted);
  sprite.unloadFont();
  sprite.deleteSprite();
  ++menuPhase;

};

/******************************************
 * GLOBAL VARIABLES                       *
 ******************************************/
TFT_eSPI tft = TFT_eSPI();  // Driver for display
Display display(tft, controllerData, &selectedScreen, &selectedMenuItem); // Display

void checkDisplayQueue() {
  userEventMessage_t message;
  userEventMessage_t messageOut;

  if( xQueueReceive( displayQueue, &message, 0) == pdPASS ) {

    Serial.printf("%s > Display [%s] k=%d\n", senderLabels[message.sender].c_str(), commandLabels[message.command].c_str(), keyboardEnabed );

    if (message.command != cmdSilentUpdate || selectedScreen() == scnHome ||
        selectedScreen() == scnSettingsSensorOffset) display.processMessage(message);

    switch( message.command ) {

      case cmdBoilerSending:
        // Boiler scheduling is independent of the display.
        break;

      case cmdUpdateScreen:
        // Switch on the backlight
        sendMessage(sndDisplay, cmdBacklightOn, backlightQueue); // Switch on backlight
        break;

    } // switch
  } // xQueueReceive
  display.service();
}


// Start the the display handler
void startDisplay() {
  display.start();
}
