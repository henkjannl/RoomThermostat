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

  {mnHomeScreen,          "None",                  iconNone,            cmMainMenu                   },
  {mnMain,                "Overrule today",        iconToday,           cmOverruleTodayMenu          },
  {mnMain,                "Overrule tomorrow",     iconTomorrow,        cmOverruleTomorrowMenu       },
  {mnMain,                "Overrule multiple days",iconMultipleDays,    cmOverruleMultipleMenu       },
  {mnMain,                "Settings",              iconSettings,        cmSettingsMenu               },
  {mnMain,                "Exit menu",             iconExit,            cmHomeScreen                 },
  {mnOverruleDay,         "Work from home",        iconHome,            cmOverruleDayReturn          },
  {mnOverruleDay,         "Work from office",      iconWork,            cmOverruleDayReturn          },
  {mnOverruleDay,         "All day away",          iconAway,            cmOverruleDayReturn          },
  {mnOverruleDay,         "Weekend",               iconWeekend,         cmOverruleDayReturn          },
  {mnOverruleDay,         "Automatic",             iconAutomatic,       cmOverruleDayReturn          },
  {mnOverruleDay,         "Back to main menu",     iconExit,            cmMainMenu                   },
  {mnOverruleMultiple,    "More days",             iconMore,            cmMultipleIncDays            },
  {mnOverruleMultiple,    "Fewer days",            iconFewer,           cmMultipleDecDays            },
  {mnOverruleMultiple,    "Set day schedule",      iconWeekschedule,    cmOverruleMultiple2Menu      },
  {mnOverruleMultiple,    "Reset to automatic",    iconAutomatic,       cmOverruleMultipleAutomatic  },
  {mnOverruleMultiple,    "Back to main menu",     iconExit,            cmMainMenu                   },
  {mnOverruleMultiple2,   "Work from home",        iconHome,            cmOverruleMultipleReturn     },
  {mnOverruleMultiple2,   "Work from office",      iconWork,            cmOverruleMultipleReturn     },
  {mnOverruleMultiple2,   "All day away",          iconAway,            cmOverruleMultipleReturn     },
  {mnOverruleMultiple2,   "Weekend",               iconWeekend,         cmOverruleMultipleReturn     },
  {mnOverruleMultiple2,   "Automatic",             iconAutomatic,       cmOverruleMultipleReturn     },
  {mnOverruleMultiple2,   "Back to previous menu", iconExit,            cmOverruleMultipleMenu       },
  {mnSettings,            "Set temperatures",      iconLowTemperature,  cmTemperatureMenu            },
  {mnSettings,            "Set week schedule",     iconWeekschedule,    cmWeekScheduleMenu           },
  {mnSettings,            "Work from home times",  iconHome,            cmWorkFromHomeTimesMenu      },
  {mnSettings,            "Work from office times",iconWork,            cmWorkFromOfficeTimesMenu    },
  {mnSettings,            "Set weekend times",     iconWeekend,         cmWeekendTimesMenu           },
  {mnSettings,            "Back to main menu",     iconExit,            cmMainMenu                   },
  {mnTemperatureSettings, "High temperature up",   iconHighTemperature, cmHighTempInc                },
  {mnTemperatureSettings, "High temperature down", iconHighTemperature, cmHighTempDec                },
  {mnTemperatureSettings, "Low temperature up",    iconLowTemperature,  cmLowTempInc                 },
  {mnTemperatureSettings, "Low temperature down",  iconLowTemperature,  cmLowTempDec                 },
  {mnTemperatureSettings, "Back to settings menu", iconExit,            cmSettingsMenu               },
  {mnSetWeekSchedule,     "Monday",                iconMonday,          cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Tuesday",               iconTuesday,         cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Wednesday",             iconWednesday,       cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Thursday",              iconThursday,        cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Friday",                iconFriday,          cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Saturday",              iconSaturday,        cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Sunday",                iconSunday,          cmOverruleDaySettingMenu     },
  {mnSetWeekSchedule,     "Back to settings menu", iconExit,            cmSettingsMenu               },
  {mnSetDaySchedule,      "Work from home",        iconHome,            cmOverruleDayScheduleHome    },
  {mnSetDaySchedule,      "Work from office",      iconWork,            cmOverruleDayScheduleOffice  },
  {mnSetDaySchedule,      "All day away",          iconAway,            cmOverruleDayScheduleAway    },
  {mnSetDaySchedule,      "Weekend",               iconWeekend,         cmOverruleDayScheduleWeekend },
  {mnSetDaySchedule,      "Back to week schedule", iconExit,            cmWeekScheduleMenu           },
  {mnSetHomeTimes,        "Wake up later",         iconMore,            cmHomeWakeUpInc              },
  {mnSetHomeTimes,        "Wake up earlier",       iconFewer,           cmHomeWakeUpDec              },
  {mnSetHomeTimes,        "Go to sleep later",     iconMore,            cmHomeSleepInc               },
  {mnSetHomeTimes,        "Go to sleep earlier",   iconFewer,           cmHomeSleepDec               },
  {mnSetHomeTimes,        "Back to settings menu", iconExit,            cmSettingsMenu               },
  {mnSetOfficeTimes,      "Wake up later",         iconMore,            cmOfficeWakeUpInc            },
  {mnSetOfficeTimes,      "Wake up earlier",       iconFewer,           cmOfficeWakeUpDec            },
  {mnSetOfficeTimes,      "Leave later",           iconMore,            cmOfficeLeaveInc             },
  {mnSetOfficeTimes,      "Leave earlier",         iconFewer,           cmOfficeLeaveDec             },
  {mnSetOfficeTimes,      "Come home later",       iconMore,            cmOfficeComeHomeInc          },
  {mnSetOfficeTimes,      "Come home earlier",     iconFewer,           cmOfficeComeHomeDec          },
  {mnSetOfficeTimes,      "Go to sleep later",     iconMore,            cmOfficeSleepInc             },
  {mnSetOfficeTimes,      "Go to sleep earlier",   iconFewer,           cmOfficeSleepDec             },
  {mnSetOfficeTimes,      "Back to settings menu", iconExit,            cmSettingsMenu               },
  {mnSetWeekendTimes,     "Wake up later",         iconMore,            cmWeekendWakeUpInc           },
  {mnSetWeekendTimes,     "Wake up earlier",       iconFewer,           cmWeekendWakeUpDec           },
  {mnSetWeekendTimes,     "Go to sleep later",     iconMore,            cmWeekendSleepInc            },
  {mnSetWeekendTimes,     "Go to sleep earlier",   iconFewer,           cmWeekendSleepDec            },
  {mnSetWeekendTimes,     "Back to settings menu", iconExit,            cmSettingsMenu               }
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
    void drawTemperatureSubTitle();
    void drawHomeTimeSubTitle();
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
  menuStartsAtRow=32;
  
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
      break;

    case mnOverruleDay:
      drawMenuTitle( (context==cxToday) ? "Overrule today" : "Overrule tomorrow");
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
      break;

    case mnSettings:
      drawMenuTitle("Settings");
      break;

    case mnSetWeekSchedule:
      drawMenuTitle("Set weekly schedule");
      break;

    case mnTemperatureSettings:
      drawMenuTitle("Temperature settings");
      drawTemperatureSubTitle();
      menuStartsAtRow=80;
      break;

    case mnSetDaySchedule:
           if(context==cxMonday   ) drawMenuTitle( "Set Monday schedule"   );
      else if(context==cxTuesday  ) drawMenuTitle( "Set Tuesday schedule"  );
      else if(context==cxWednesday) drawMenuTitle( "Set Wednesday schedule");
      else if(context==cxThursday ) drawMenuTitle( "Set Thursday schedule" );
      else if(context==cxFriday   ) drawMenuTitle( "Set Friday schedule"   );
      else if(context==cxSaturday ) drawMenuTitle( "Set Saturday schedule" );
      else if(context==cxSunday   ) drawMenuTitle( "Set Sunday schedule"   );
            
      break;

    case mnSetHomeTimes:
      drawMenuTitle( "Set work from home times");
      drawHomeTimeSubTitle();
      menuStartsAtRow=80;
      break;
    
    case mnSetOfficeTimes:
      drawMenuTitle( "Set office times");
      menuStartsAtRow=80;
      break;
    
    case mnSetWeekendTimes:
      drawMenuTitle( "Set weekend times");
      menuStartsAtRow=80;
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
    
    case cmOverruleMultipleAutomatic:
      data->overrideMultiple=dtAuto;
      data->setIcons();
      selectMenu(mnHomeScreen);
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
    
    case cmTemperatureMenu:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmWeekScheduleMenu:
      selectMenu(mnSetWeekSchedule);
    break;
    
    case cmWorkFromHomeTimesMenu:
      selectMenu(mnSetHomeTimes);
    break;
    
    case cmWorkFromOfficeTimesMenu:
      selectMenu(mnSetOfficeTimes);
    break;
    
    case cmWeekendTimesMenu:
      selectMenu(mnSetWeekendTimes);
    break;
    
    case cmHighTempInc:
      data->highTemp+=0.5;
      drawTemperatureSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmHighTempDec:
      data->highTemp-=0.5;
      drawTemperatureSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmLowTempInc:
      data->lowTemp+=0.5;
      drawTemperatureSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmLowTempDec:
      data->lowTemp-=0.5;
      drawTemperatureSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmOverruleDaySettingMenu:
           if (menuStructure[mnSetWeekSchedule].selectedItem==0) context=cxMonday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==1) context=cxTuesday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==2) context=cxWednesday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==3) context=cxThursday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==4) context=cxFriday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==5) context=cxSaturday;
      else if (menuStructure[mnSetWeekSchedule].selectedItem==6) context=cxSunday;
      selectMenu(mnSetDaySchedule);
    break;
    
    case cmOverruleDayScheduleHome:
           if (context==cxSunday   ) data->regularWeek[0]=dtWorkFromHome;
      else if (context==cxMonday   ) data->regularWeek[1]=dtWorkFromHome;
      else if (context==cxTuesday  ) data->regularWeek[2]=dtWorkFromHome;
      else if (context==cxWednesday) data->regularWeek[3]=dtWorkFromHome;
      else if (context==cxThursday ) data->regularWeek[4]=dtWorkFromHome;
      else if (context==cxFriday   ) data->regularWeek[5]=dtWorkFromHome;
      else if (context==cxSaturday ) data->regularWeek[6]=dtWorkFromHome;
      selectMenu(mnSetWeekSchedule);
    break;
    
    case cmOverruleDayScheduleOffice:
           if (context==cxSunday   ) data->regularWeek[0]=dtWorkAtOffice;
      else if (context==cxMonday   ) data->regularWeek[1]=dtWorkAtOffice;
      else if (context==cxTuesday  ) data->regularWeek[2]=dtWorkAtOffice;
      else if (context==cxWednesday) data->regularWeek[3]=dtWorkAtOffice;
      else if (context==cxThursday ) data->regularWeek[4]=dtWorkAtOffice;
      else if (context==cxFriday   ) data->regularWeek[5]=dtWorkAtOffice;
      else if (context==cxSaturday ) data->regularWeek[6]=dtWorkAtOffice;
      selectMenu(mnSetWeekSchedule);
    break;
    
    case cmOverruleDayScheduleAway:
           if (context==cxSunday   ) data->regularWeek[0]=dtAway;
      else if (context==cxMonday   ) data->regularWeek[1]=dtAway;
      else if (context==cxTuesday  ) data->regularWeek[2]=dtAway;
      else if (context==cxWednesday) data->regularWeek[3]=dtAway;
      else if (context==cxThursday ) data->regularWeek[4]=dtAway;
      else if (context==cxFriday   ) data->regularWeek[5]=dtAway;
      else if (context==cxSaturday ) data->regularWeek[6]=dtAway;
      selectMenu(mnSetWeekSchedule);
    break;
    
    case cmOverruleDayScheduleWeekend:
           if (context==cxSunday   ) data->regularWeek[0]=dtWeekend;
      else if (context==cxMonday   ) data->regularWeek[1]=dtWeekend;
      else if (context==cxTuesday  ) data->regularWeek[2]=dtWeekend;
      else if (context==cxWednesday) data->regularWeek[3]=dtWeekend;
      else if (context==cxThursday ) data->regularWeek[4]=dtWeekend;
      else if (context==cxFriday   ) data->regularWeek[5]=dtWeekend;
      else if (context==cxSaturday ) data->regularWeek[6]=dtWeekend;
      selectMenu(mnSetWeekSchedule);
    break;
    
    case cmHomeWakeUpInc:
      data->workFromHomeWakeUp.inc15();
      drawHomeTimeSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmHomeWakeUpDec:
      data->workFromHomeWakeUp.dec15();
      drawHomeTimeSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmHomeSleepInc:
      data->workFromHomeSleep.inc15();
      drawHomeTimeSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmHomeSleepDec:
      data->workFromHomeSleep.dec15();
      drawHomeTimeSubTitle();
      blockDrawScreen=true;
    break;
    
    case cmOfficeWakeUpInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeWakeUpDec:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeLeaveInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeLeaveDec:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeComeHomeInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeComeHomeDec:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeSleepInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmOfficeSleepDec:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmWeekendWakeUpInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmWeekendWakeUpDec:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmWeekendSleepInc:
      selectMenu(mnTemperatureSettings);
    break;
    
    case cmWeekendSleepDec:
      selectMenu(mnTemperatureSettings);
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

void Menu::drawHomeTimeSubTitle() {
  struct tm* tm;
  char buffer1[80];
  char buffer2[80];
  snprintf(buffer1, 80, "Wake up at %d:%02d",     data->workFromHomeWakeUp.hour, data->workFromHomeWakeUp.minute);
  snprintf(buffer2, 80, "Go to sleep at %d:%02d", data->workFromHomeSleep.hour,  data->workFromHomeSleep.minute );
  drawSubTitle(std::string("Wake up at")+data->workFromHomeWakeUp.str(), buffer2);
}

void Menu::drawTemperatureSubTitle() {
  char buffer1[80];
  char buffer2[80];
  snprintf(buffer1, 80, "High temperature set to %.1f°C", data->highTemp);
  snprintf(buffer2, 80, "Low temperature set to %.1f°C",  data->lowTemp );
  drawSubTitle(buffer1, buffer2);
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
