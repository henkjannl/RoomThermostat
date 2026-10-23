#pragma once

#include <M5Stack.h>
#include "Constants.h"
#include <map>
#include <list>

using namespace std;

TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd);

class menuItem_t {
  public: 
    Menus_t menu;            // Menu to which this item belongs
    char label[27];          // Text to show. ToDo: non-fixed size
    icon_t icon;             // Icon to be shown in the menu
    Commands_t command;      // Command to be executed when select button is pressed
    bool operator==(menuItem_t& other) const;
};

bool menuItem_t::operator==(menuItem_t& other) const
{
    return ( (menu == other.menu) && (strcmp(label, other.label)==0) );
}

static const menuItem_t ALL_MENU_ITEMS[] = {  
// Menu group              Label                    Icon                 Command when selected          Context

  {mnHomeScreen,           "None",                  iconNone,            cmMainMenu                    },
  {mnMain,                 "Overrule today",        iconToday,           cmOverruleTodayMenu           },
  {mnMain,                 "Overrule tomorrow",     iconTomorrow,        cmOverruleTomorrowMenu        },
  {mnMain,                 "Overrule multiple days",iconMultipleDays,    cmOverruleMultipleMenu        },
  {mnMain,                 "Settings",              iconSettings,        cmSettingsMenu                },
  {mnMain,                 "Exit menu",             iconExit,            cmHomeScreen                  },
  {mnOverruleDay,          "Work from home",        iconHome,            cmOverruleDayReturn           },
  {mnOverruleDay,          "Work from office",      iconWork,            cmOverruleDayReturn           },
  {mnOverruleDay,          "All day away",          iconAway,            cmOverruleDayReturn           },
  {mnOverruleDay,          "Weekend",               iconWeekend,         cmOverruleDayReturn           },
  {mnOverruleDay,          "Automatic",             iconAutomatic,       cmOverruleDayReturn           },
  {mnOverruleDay,          "Back to main menu",     iconExit,            cmMainMenu                    },
  {mnOverruleMultiple,     "More days",             iconMore,            cmMultipleIncDays             },
  {mnOverruleMultiple,     "Fewer days",            iconFewer,           cmMultipleDecDays             },
  {mnOverruleMultiple,     "Set day schedule",      iconWeekschedule,    cmOverruleMultiple2Menu       },
  {mnOverruleMultiple,     "Reset to automatic",    iconAutomatic,       cmOverruleMultipleAutomatic   },
  {mnOverruleMultiple,     "Back to main menu",     iconExit,            cmMainMenu                    },
  {mnOverruleMultiple2,    "Work from home",        iconHome,            cmOverruleMultipleReturn      },
  {mnOverruleMultiple2,    "Work from office",      iconWork,            cmOverruleMultipleReturn      },
  {mnOverruleMultiple2,    "All day away",          iconAway,            cmOverruleMultipleReturn      },
  {mnOverruleMultiple2,    "Weekend",               iconWeekend,         cmOverruleMultipleReturn      },
  {mnOverruleMultiple2,    "Automatic",             iconAutomatic,       cmOverruleMultipleReturn      },
  {mnOverruleMultiple2,    "Back to previous menu", iconExit,            cmOverruleMultipleMenu        },
  {mnSettings,             "Set low temperature",   iconLowTemperature,  cmLowTempScreen               },
  {mnSettings,             "Set high temperature",  iconHighTemperature, cmHighTempScreen              },
  {mnSettings,             "Set week schedule",     iconWeekschedule,    cmWeekScheduleMenu            },
  {mnSettings,             "Work from home times",  iconHome,            cmWorkFromHomeTimesMenu       },
  {mnSettings,             "Work from office times",iconWork,            cmWorkFromOfficeTimesMenu     },
  {mnSettings,             "Set weekend times",     iconWeekend,         cmWeekendTimesMenu            },
  {mnSettings,             "Back to main menu",     iconExit,            cmMainMenu                    },
  {mnSetWeekSchedule,      "Monday",                iconMonday,          cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Tuesday",               iconTuesday,         cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Wednesday",             iconWednesday,       cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Thursday",              iconThursday,        cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Friday",                iconFriday,          cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Saturday",              iconSaturday,        cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Sunday",                iconSunday,          cmOverruleDaySettingMenu      },
  {mnSetWeekSchedule,      "Back to settings menu", iconExit,            cmSettingsMenu                },
  {mnSetDayScheduleMonday, "Work from home",        iconHome,            cmOverruleMondaySettingHome   },
  {mnSetDayScheduleMonday, "Work from office",      iconWork,            cmOverruleMondaySettingOffice },
  {mnSetDayScheduleMonday, "All day away",          iconAway,            cmOverruleMondaySettingAway   },
  {mnSetDayScheduleMonday, "Weekend",               iconWeekend,         cmOverruleMondaySettingWeekend},
  {mnSetDayScheduleMonday, "Back to week schedule", iconExit,            cmWeekScheduleMenu            },
  {mnSetHomeTimes,         "Wake up time",          iconClock,           cmHomeWakeUpTimeScreen        },
  {mnSetHomeTimes,         "Go to sleep time",      iconClock,           cmHomeSleepTimeScreen         },
  {mnSetHomeTimes,         "Back to settings menu", iconExit,            cmSettingsMenu                },
  {mnSetOfficeTimes,       "Wake up time",          iconClock,           cmOfficeWakeUpTimeScreen      },
  {mnSetOfficeTimes,       "Leave home time",       iconClock,           cmOfficeLeaveTimeScreen       },
  {mnSetOfficeTimes,       "Come home time",        iconClock,           cmOfficeHomeTimeScreen        },
  {mnSetOfficeTimes,       "Go to sleep time",      iconClock,           cmOfficeSleepTimeScreen       },
  {mnSetOfficeTimes,       "Back to settings menu", iconExit,            cmSettingsMenu                },
  {mnSetWeekendTimes,      "Wake up time",          iconClock,           cmHomeWakeUpTimeScreen        },
  {mnSetWeekendTimes,      "Go to sleep time",      iconClock,           cmHomeSleepTimeScreen         },
  {mnSetWeekendTimes,      "Back to settings menu", iconExit,            cmSettingsMenu                },
};

// All lists of menus together
class MenuGroup_t {
  public:
    uint8_t selectedItem=0;

    std::vector<menuItem_t> menuItems;
    
    void drawMenu(int menuStartsAtRow);
    void prevItem(int menuStartsAtRow);
    void nextItem(int menuStartsAtRow);
    Commands_t command();
};

void MenuGroup_t::drawMenu(int menuStartsAtRow) {
  
  const int W=275; // Width of the sprite
  const int H=32;  // Height of the sprite
  const int L=40;  // Left coordinate of sprite on screen

  int y=menuStartsAtRow;

  tftSprite.createSprite(W, H);
  tftSprite.setTextDatum(ML_DATUM);
  tftSprite.loadFont(FONT_MENU); // Noway Regular 24 pixels high
  tftSprite.setTextColor(TFT_BLACK, TFT_WHITE);
  tftSprite.fillSprite(WHITE);    

  uint8_t currentItem=0;
  for(const menuItem_t &menuItem : menuItems) {

    bool displayAsSelected = (currentItem ==selectedItem);

    tftSprite.fillScreen(TFT_WHITE);

    if(displayAsSelected) {
      tftSprite.fillRoundRect(0, 0, W,   H,   H/2,   TFT_RED);
      tftSprite.fillRoundRect(3, 3, W-6, H-6, H/2-3, TFT_WHITE);
    }
    
    tftSprite.drawString(menuItem.label, H/2, H/2);
    tftSprite.pushSprite(L, y);

    if(menuItem.icon != iconNone) {
      int iconID = menuItem.icon;      
      M5.Lcd.drawJpgFile(SPIFFS, ICONS[iconID].c_str(), 2, y);
    }

    y+=H;
    currentItem++;
  }
  tftSprite.unloadFont();
  
  tftSprite.deleteSprite();
}

void MenuGroup_t::prevItem(int menuStartsAtRow) {
  if(selectedItem>0) {
    selectedItem--;
    drawMenu(menuStartsAtRow);
  }
};

void MenuGroup_t::nextItem(int menuStartsAtRow) {
  if(selectedItem<menuItems.size()-1) {
    selectedItem++;
    drawMenu(menuStartsAtRow);
  }
};

Commands_t MenuGroup_t::command() {
  return menuItems[selectedItem].command;
};

// Menu class containing the whole menu structure
class Menu {
  public: 
    Menu(Data *data); // constructor

    Data *data;
    
    std::map<Menus_t, MenuGroup_t> menuStructure;

    Menus_t currentMenu = mnHomeScreen;
    Context_t context = cxNone;

    int menuStartsAtRow=0;
    
    void drawScreen();
    void selectMenu(enum Menus_t newMenu);
    void buttonUp();
    void buttonDown();
    void buttonSelect();    

    void drawMenuTitle(std::string title);
    void drawSubTitle(std::string title1, std::string title2);
    void drawMeasuredTemperature();
    void drawTemperatureSetpoint();
    void drawDateAndTime();
    void drawBoiler();
    void drawIcons();
};

// Constructor
Menu::Menu(Data *data) {

  // Store the adress of the real time data and the settings
  this->data=data;

  // Distribute the flat list of menu items over different menu groups
  for(const menuItem_t &menuItem : ALL_MENU_ITEMS) {

    // Create a new key in the menuStructure if the menu does not yet exist
    if(menuStructure.find(menuItem.menu)==menuStructure.end()) 
    {
      // This menu is not yet in the list. Add it to the list
      MenuGroup_t newMenu;
      menuStructure[menuItem.menu]=newMenu;      
    }

    // Store a pointer to the menu item in the appropriate menu
    menuStructure[menuItem.menu].menuItems.push_back(menuItem);
  }
};

void Menu::selectMenu(enum Menus_t newMenu) {
  currentMenu=newMenu;
}

void Menu::drawScreen() {

  struct tm* tm;
  char buffer1[80];
  char buffer2[80];
  
  M5.Lcd.fillScreen(WHITE);
  
  switch(currentMenu) {

    case mnHomeScreen:
      drawMeasuredTemperature();
      drawDateAndTime();
      drawBoiler();
      drawTemperatureSetpoint();
      drawIcons();
      M5.Lcd.drawJpgFile(SPIFFS, ICONS[iconTempHigher].c_str(), 288,   0);
      M5.Lcd.drawJpgFile(SPIFFS, ICONS[iconMenu      ].c_str(), 288,  80);
      M5.Lcd.drawJpgFile(SPIFFS, ICONS[iconTempLower ].c_str(), 288, 160);
      break;
    
    case mnMain:
      drawMenuTitle("Main menu");
      menuStartsAtRow=32;
      break;

    case mnOverruleDay:
      drawMenuTitle( (context==cxToday) ? "Overrule today" : "Overrule tomorrow");
      menuStartsAtRow=32;
      break;

    case mnOverruleMultiple:
      drawMenuTitle("Overrule multiple days");
      tm = localtime(&(data->now));
      tm->tm_mday += data->overrideMultipleCount;
      strftime(buffer1, 80, "%A %e %B", tm);
      snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
      drawSubTitle("Set to '"+DAY_TYPES[data->overrideMultiple]+"' until", buffer2);
      menuStartsAtRow=80;
      break;

    case mnOverruleMultiple2:
      drawMenuTitle("Overrule multiple days");
      menuStartsAtRow=32;
      break;

    case mnSetHomeTimes:
      drawMenuTitle( "Set work from home times");
      menuStartsAtRow=32;
      break;
    
    case mnSetOfficeTimes:
      drawMenuTitle( "Set office times");
      menuStartsAtRow=32;
      break;
    
    case mnSettings:
      drawMenuTitle( "Settings");
      menuStartsAtRow=32;
      break;

    case mnSetWeekendTimes:
      break;

    case mnSetWeekSchedule:
      break;

  };

  // Draw the menu if needed
  if(currentMenu!=mnHomeScreen) {
    menuStructure[currentMenu].drawMenu(menuStartsAtRow);
  };
  
};

void Menu::buttonUp() {
  if(currentMenu==mnHomeScreen) {
    data->overrideTempNow=true;
    data->overrideTemp+=0.5;
    data->setpointTemperature=data->overrideTemp;
    drawTemperatureSetpoint();
  }
  else {
    menuStructure[currentMenu].prevItem(menuStartsAtRow);
  }
};

void Menu::buttonDown() {
  if(currentMenu==mnHomeScreen) {
    data->overrideTempNow=true;
    data->overrideTemp-=0.5;
    data->setpointTemperature=data->overrideTemp;
    drawTemperatureSetpoint();
  }
  else {
    menuStructure[currentMenu].nextItem(menuStartsAtRow);
  }
};

void Menu::buttonSelect() {

  enum dayType_t itemSelected;
  bool blockDrawScreen = false;

  struct tm* tm;
  char buffer1[80];
  char buffer2[80];

  switch ( menuStructure[currentMenu].command() ) {

    case cmHomeScreen:
      selectMenu(mnHomeScreen);
    break;
    
    case cmMainMenu:
      menuStructure[mnMain].selectedItem=0;
      selectMenu(mnMain);
    break;

    // Select the correct item in the today menu
    case cmOverruleTodayMenu:
      context=cxToday;
           if (data->overrideToday==dtWorkFromHome) menuStructure[mnOverruleDay].selectedItem=0;
      else if (data->overrideToday==dtWorkAtOffice) menuStructure[mnOverruleDay].selectedItem=1;
      else if (data->overrideToday==dtAway        ) menuStructure[mnOverruleDay].selectedItem=2;
      else if (data->overrideToday==dtWeekend     ) menuStructure[mnOverruleDay].selectedItem=3;
      else if (data->overrideToday==dtAuto        ) menuStructure[mnOverruleDay].selectedItem=4;
      selectMenu(mnOverruleDay);
    break;

    // Select the correct item in the today menu
    case cmOverruleTomorrowMenu:
      context=cxTomorrow;
           if (data->overrideTomorrow==dtWorkFromHome) menuStructure[mnOverruleDay].selectedItem=0;
      else if (data->overrideTomorrow==dtWorkAtOffice) menuStructure[mnOverruleDay].selectedItem=1;
      else if (data->overrideTomorrow==dtAway        ) menuStructure[mnOverruleDay].selectedItem=2;
      else if (data->overrideTomorrow==dtWeekend     ) menuStructure[mnOverruleDay].selectedItem=3;
      else if (data->overrideTomorrow==dtAuto        ) menuStructure[mnOverruleDay].selectedItem=4;
      selectMenu(mnOverruleDay);
    break;

    // Store the return value of the overrule menu 
    case cmOverruleDayReturn:
           if (menuStructure[mnOverruleDay].selectedItem==0) itemSelected=dtWorkFromHome;
      else if (menuStructure[mnOverruleDay].selectedItem==1) itemSelected=dtWorkAtOffice;
      else if (menuStructure[mnOverruleDay].selectedItem==2) itemSelected=dtAway;
      else if (menuStructure[mnOverruleDay].selectedItem==3) itemSelected=dtWeekend;
      else if (menuStructure[mnOverruleDay].selectedItem==4) itemSelected=dtAuto;
      
      if(context==cxToday) data->overrideToday=itemSelected; else data->overrideTomorrow=itemSelected; 
      
      // Reset multiple days if today or tomorrow are overridden
      if(itemSelected!=dtAuto) data->overrideMultiple = dtAuto;

      data->setIcons();
      selectMenu(mnHomeScreen);
    break;
    
    // Overrule settings for a number of days after tomorrow
    case cmOverruleMultipleMenu:
      selectMenu(mnOverruleMultiple);
    break;

    case cmMultipleDecDays:
      if(data->overrideMultipleCount>1) {
        data->overrideMultipleCount--;
        data->setIcons();
        
        tm = localtime(&(data->now));
        tm->tm_mday += data->overrideMultipleCount;
        strftime(buffer1, 80, "%A %e %B", tm);
        snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
        drawSubTitle("Set to '"+DAY_TYPES[data->overrideMultiple]+"' until", buffer2);
        menuStartsAtRow=80;
      }
      blockDrawScreen=true;
    break;
    
    case cmMultipleIncDays:
      data->overrideMultipleCount++;
      data->setIcons();
        
      tm = localtime(&(data->now));
      tm->tm_mday += data->overrideMultipleCount;
      strftime(buffer1, 80, "%A %e %B", tm);
      snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
      drawSubTitle("Set to '"+DAY_TYPES[data->overrideMultiple]+"' until", buffer2);
      menuStartsAtRow=80;

      blockDrawScreen=true;
    break;
    
    case cmOverruleMultiple2Menu:
           if (data->overrideMultiple==dtWorkFromHome) menuStructure[mnOverruleMultiple2].selectedItem=0;
      else if (data->overrideMultiple==dtWorkAtOffice) menuStructure[mnOverruleMultiple2].selectedItem=1;
      else if (data->overrideMultiple==dtAway        ) menuStructure[mnOverruleMultiple2].selectedItem=2;
      else if (data->overrideMultiple==dtWeekend     ) menuStructure[mnOverruleMultiple2].selectedItem=3;
      else if (data->overrideMultiple==dtAuto        ) menuStructure[mnOverruleMultiple2].selectedItem=4;
      selectMenu(mnOverruleMultiple2);
    break;
    
    case cmOverruleMultipleReturn:
           if (menuStructure[mnOverruleMultiple2].selectedItem==0) data->overrideMultiple=dtWorkFromHome;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==1) data->overrideMultiple=dtWorkAtOffice;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==2) data->overrideMultiple=dtAway;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==3) data->overrideMultiple=dtWeekend;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==4) data->overrideMultiple=dtAuto;

      // Reset today and tomorrow if multiple days are selected
      if(data->overrideMultiple!=dtAuto) {
        if(data->overrideMultipleCount>0) data->overrideToday    = dtAuto;
        if(data->overrideMultipleCount>1) data->overrideTomorrow = dtAuto;
      }
      
      data->setIcons();
      selectMenu(mnOverruleMultiple);
    break;
    
    case cmSettingsMenu:
      selectMenu(mnSettings);
    break;
    
    case cmHighTempScreen:
    break;
    
    case cmHomeSleepTimeScreen:
    break;
    
    case cmHomeWakeUpTimeScreen:
    break;
    
    case cmLowTempScreen:
    break;
    
    case cmOfficeHomeTimeScreen:
    break;
    
    case cmOfficeLeaveTimeScreen:
    break;
    
    case cmOfficeSleepTimeScreen:
    break;
    
    case cmOfficeWakeUpTimeScreen:
    break;
    
  } // switch (menuStructure[currentMenu].command) {

  if(!blockDrawScreen) drawScreen();
};

void Menu::drawMenuTitle(std::string title) {
  M5.Lcd.loadFont(FONT_MENU_HEADER);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.drawString(title.c_str(), 160, 4);
};

void Menu::drawSubTitle(std::string title1, std::string title2) {
  M5.Lcd.fillRect(0, 32, 320, 79-32, TFT_WHITE);
  M5.Lcd.loadFont(FONT_SUBMENU);
  M5.Lcd.setTextDatum(TC_DATUM);
  M5.Lcd.setTextColor(TFT_BLACK, TFT_WHITE);
  M5.Lcd.drawString(title1.c_str(), 160, 32);
  M5.Lcd.drawString(title2.c_str(), 160, 55);
}

void Menu::drawDateAndTime() {

  const int W=288; // Width of the sprite
  const int H=26;  // Height of the sprite

  //time_t now;
  //float measuredTemperature=21.3;
  //bool showerRunning=true;  
  //float boilerPower = 0.75;

  Serial.printf("The current local time is: %s\n", ctime(&(data->now)));

  char buffer[32]; 
  struct tm * lctm;
  lctm = localtime(&(data->now));
  strftime(buffer, 32, "%A %e %B", lctm);
  //sprintf(buffer,"Wednesday 30 September"); // Longest date possible to check if it fits the screen

  tftSprite.createSprite(W, H);
  tftSprite.setTextDatum(MC_DATUM);
  tftSprite.loadFont(FONT_DATE); 
  tftSprite.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString(buffer, W/2, H/2);
  tftSprite.pushSprite(0, 0);
  tftSprite.unloadFont();

  strftime(buffer, 32, "%H:%M", lctm);
  //sprintf(buffer,"23:59"); // Local time possible to check if it fits the screen
  tftSprite.setTextDatum(MC_DATUM);
  tftSprite.loadFont(FONT_TIME);
  tftSprite.setTextColor(TFT_DARKGREY, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString(buffer, W/2, H/2);
  tftSprite.pushSprite(0, 40);
  tftSprite.unloadFont();
  tftSprite.deleteSprite();
};

void Menu::drawMeasuredTemperature() {

  const int W=200; // Width of the sprite
  const int H=62;  // Height of the sprite

  char temperature[12]; 
  sprintf(temperature, "%.1f",data->measuredTemperature);

  tftSprite.createSprite(W, H);
  tftSprite.setTextDatum(MR_DATUM);
  tftSprite.loadFont(FONT_TEMP_MEASURED); 
  tftSprite.setTextColor(TFT_BLACK, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString(temperature, W, H/2);
  tftSprite.pushSprite(0, 84);
  tftSprite.unloadFont();
  tftSprite.deleteSprite();

  tftSprite.createSprite(34, H);
  tftSprite.setTextDatum(TL_DATUM);
  tftSprite.loadFont(FONT_TEMP_MEASURED_DEG);
  tftSprite.setTextColor(TFT_LIGHTGREY, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString("°C", 0, 0);
  tftSprite.pushSprite(W+3, 85);
  tftSprite.unloadFont();
  tftSprite.deleteSprite();
};

void Menu::drawTemperatureSetpoint() {

  const int W=98;  // Width of the sprite
  const int H=34;  // Height of the sprite

  char temperature[12]; 
  sprintf(temperature, "%.1f",data->setpointTemperature);

  tftSprite.createSprite(W, H);
  tftSprite.setTextDatum(TR_DATUM);
  tftSprite.loadFont(FONT_TEMP_SETPOINT); 
  tftSprite.setTextColor(TFT_RED, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString(temperature, W, 0);
  tftSprite.pushSprite(121, 157);
  tftSprite.unloadFont();
  tftSprite.deleteSprite();

  tftSprite.createSprite(34, H);
  tftSprite.setTextDatum(TL_DATUM);
  tftSprite.loadFont(FONT_TEMP_SETPOINT_DEG);
  tftSprite.setTextColor(TFT_LIGHTGREY, TFT_WHITE);
  tftSprite.fillSprite(TFT_WHITE);    
  tftSprite.drawString("°C", 0, 0);
  tftSprite.pushSprite(223, 157);
  tftSprite.unloadFont();
  tftSprite.deleteSprite();
};

void Menu::drawBoiler() {
  
  M5.Lcd.drawJpgFile(SPIFFS, ICONS[data->showerRunning ? iconShowerOn : iconShowerOff].c_str(), 56, 158);

  enum icon_t flame;
  if      (data->boilerPower < 0.11) flame = iconFlame0; 
  else if (data->boilerPower < 0.22) flame = iconFlame1; 
  else if (data->boilerPower < 0.33) flame = iconFlame2; 
  else if (data->boilerPower < 0.44) flame = iconFlame3; 
  else if (data->boilerPower < 0.55) flame = iconFlame4; 
  else if (data->boilerPower < 0.66) flame = iconFlame5; 
  else if (data->boilerPower < 0.77) flame = iconFlame6; 
  else if (data->boilerPower < 0.88) flame = iconFlame7; 
  else                               flame = iconFlame8; 

  M5.Lcd.drawJpgFile(SPIFFS, ICONS[flame].c_str(), 100, 159);
  
};

void Menu::drawIcons() {
  int x=8;
  for(int i=0; i<7; i++) {
    M5.Lcd.drawJpgFile(SPIFFS, ICONS[data->icons[i]].c_str(), x, 200);
    x+=40;
  }
};
