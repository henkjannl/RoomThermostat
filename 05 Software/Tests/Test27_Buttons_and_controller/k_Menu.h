#pragma once

#include <TFT_eSPI.h>
#include <SPI.h>

#include <map>
#include <list>

#include "a_Constants.h"
#include "j_jpeg.h"

using namespace std;

/* ==============================================================================
   menuItem_t
   Item in a menu, including 
    - to which menu the item belongs
    - which icon is displayed
    - which command is executed
   ============================================================================== */
class menuItem_t {
  public: 
    Menus_t menu;            // Menu to which this item belongs
    char label[32];          // Text to show. ToDo: non-fixed size (std::string?)
    icon_t icon;             // Icon to be shown in the menu
    Commands_t command;      // Command to be executed when select button is pressed
    bool operator==(menuItem_t& other) const;
};

bool menuItem_t::operator==(menuItem_t& other) const
{
    return ( (menu == other.menu) && (strcmp(label, other.label)==0) );
}

static const menuItem_t ALL_MENU_ITEMS[] = {  
// Menu group              Label                   Icon                 Command when selected
  {mnHomeScreen,          "None",                  iconNone,            cmMainMenu                  },
  {mnMain,                "Overrule today",        iconToday,           cmOverruleTodayMenu         },
  {mnMain,                "Overrule tomorrow",     iconTomorrow,        cmOverruleTomorrowMenu      },
  {mnMain,                "Overrule multiple days",iconMultipleDays,    cmOverruleMultipleMenu      },
  {mnMain,                "Settings",              iconSettings,        cmSettingsMenu              },
  {mnMain,                "Exit menu",             iconExit,            cmHomeScreen                },
  {mnOverruleDay,         "Work from home",        iconHome,            cmOverruleDayReturn         },
  {mnOverruleDay,         "Work from office",      iconWork,            cmOverruleDayReturn         },
  {mnOverruleDay,         "All day away",          iconAway,            cmOverruleDayReturn         },
  {mnOverruleDay,         "Weekend",               iconWeekend,         cmOverruleDayReturn         },
  {mnOverruleDay,         "Automatic",             iconAutomatic,       cmOverruleDayReturn         },
  {mnOverruleDay,         "Back to main menu",     iconExit,            cmMainMenu                  },
  {mnOverruleMultiple,    "More days",             iconMore,            cmMultipleIncDays           },
  {mnOverruleMultiple,    "Fewer days",            iconFewer,           cmMultipleDecDays           },
  {mnOverruleMultiple,    "Set day schedule",      iconWeekschedule,    cmOverruleMultiple2Menu     },
  {mnOverruleMultiple,    "Reset to automatic",    iconAutomatic,       cmOverruleMultipleAutomatic },
  {mnOverruleMultiple,    "Back to main menu",     iconExit,            cmMainMenu                  },
  {mnOverruleMultiple2,   "Work from home",        iconHome,            cmOverruleMultipleReturn    },
  {mnOverruleMultiple2,   "Work from office",      iconWork,            cmOverruleMultipleReturn    },
  {mnOverruleMultiple2,   "All day away",          iconAway,            cmOverruleMultipleReturn    },
  {mnOverruleMultiple2,   "Weekend",               iconWeekend,         cmOverruleMultipleReturn    },
  {mnOverruleMultiple2,   "Automatic",             iconAutomatic,       cmOverruleMultipleReturn    },
  {mnOverruleMultiple2,   "Back to previous menu", iconExit,            cmOverruleMultipleMenu      },
  {mnSettings,            "Set temperatures",      iconLowTemperature,  cmTemperatureMenu           },
  {mnSettings,            "Set sensor offset",     iconSensorOffset,    cmSensorOffsetMenu          },
  {mnSettings,            "Set week schedule",     iconWeekschedule,    cmWeekScheduleMenu          },
  {mnSettings,            "Work from home times",  iconHome,            cmWorkFromHomeTimesMenu     },
  {mnSettings,            "Work from office times",iconWork,            cmWorkFromOfficeTimesMenu   },
  {mnSettings,            "Set weekend times",     iconWeekend,         cmWeekendTimesMenu          },
  {mnSettings,            "Back to main menu",     iconExit,            cmMainMenu                  },
  {mnTemperatureSettings, "High temperature up",   iconHighTemperature, cmHighTempInc               },
  {mnTemperatureSettings, "High temperature down", iconHighTemperature, cmHighTempDec               },
  {mnTemperatureSettings, "Low temperature up",    iconLowTemperature,  cmLowTempInc                },
  {mnTemperatureSettings, "Low temperature down",  iconLowTemperature,  cmLowTempDec                },
  {mnTemperatureSettings, "Back to home screen",   iconExit,            cmHomeScreen                },
  {mnSetWeekSchedule,     "Monday",                iconMonday,          cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Tuesday",               iconTuesday,         cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Wednesday",             iconWednesday,       cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Thursday",              iconThursday,        cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Friday",                iconFriday,          cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Saturday",              iconSaturday,        cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Sunday",                iconSunday,          cmOverruleDaySettingMenu    },
  {mnSetWeekSchedule,     "Back to settings menu", iconExit,            cmSettingsMenu              },
  {mnSetDaySchedule,      "Work from home",        iconHome,            cmOverruleDayScheduleHome   },
  {mnSetDaySchedule,      "Work from office",      iconWork,            cmOverruleDayScheduleOffice },
  {mnSetDaySchedule,      "All day away",          iconAway,            cmOverruleDayScheduleAway   },
  {mnSetDaySchedule,      "Weekend",               iconWeekend,         cmOverruleDayScheduleWeekend},
  {mnSetDaySchedule,      "Back to week schedule", iconExit,            cmWeekScheduleMenu          },
  {mnSetHomeTimes,        "Wake up later",         iconMore,            cmHomeWakeUpInc             },
  {mnSetHomeTimes,        "Wake up earlier",       iconFewer,           cmHomeWakeUpDec             },
  {mnSetHomeTimes,        "Go to sleep later",     iconMore,            cmHomeSleepInc              },
  {mnSetHomeTimes,        "Go to sleep earlier",   iconFewer,           cmHomeSleepDec              },
  {mnSetHomeTimes,        "Back to settings menu", iconExit,            cmSettingsMenu              },
  {mnSetOfficeTimes,      "Wake up later",         iconMore,            cmOfficeWakeUpInc           },
  {mnSetOfficeTimes,      "Wake up earlier",       iconFewer,           cmOfficeWakeUpDec           },
  {mnSetOfficeTimes,      "Leave later",           iconMore,            cmOfficeLeaveInc            },
  {mnSetOfficeTimes,      "Leave earlier",         iconFewer,           cmOfficeLeaveDec            },
  {mnSetOfficeTimes,      "Come home later",       iconMore,            cmOfficeComeHomeInc         },
  {mnSetOfficeTimes,      "Come home earlier",     iconFewer,           cmOfficeComeHomeDec         },
  {mnSetOfficeTimes,      "Go to sleep later",     iconMore,            cmOfficeSleepInc            },
  {mnSetOfficeTimes,      "Go to sleep earlier",   iconFewer,           cmOfficeSleepDec            },
  {mnSetOfficeTimes,      "Back to settings menu", iconExit,            cmSettingsMenu              },
  {mnSetWeekendTimes,     "Wake up later",         iconMore,            cmWeekendWakeUpInc          },
  {mnSetWeekendTimes,     "Wake up earlier",       iconFewer,           cmWeekendWakeUpDec          },
  {mnSetWeekendTimes,     "Go to sleep later",     iconMore,            cmWeekendSleepInc           },
  {mnSetWeekendTimes,     "Go to sleep earlier",   iconFewer,           cmWeekendSleepDec           },
  {mnSetWeekendTimes,     "Back to settings menu", iconExit,            cmSettingsMenu              },
  {mnAdjustSensorOffset,  "Increase offset",       iconMore,            cmSensorOffsetInc           },
  {mnAdjustSensorOffset,  "Reduce offset",         iconFewer,           cmSensorOffsetDec           },
  {mnAdjustSensorOffset,  "Back to home screen",   iconExit,            cmHomeScreen                }
};

/* ==============================================================================
   MenuGroup_t
   Holds a list of  menuItem_t and stores which of the items is selected
   ============================================================================== */

class MenuGroup_t {
  public:
    uint8_t selectedItem=0;

    vector<menuItem_t> menuItems;
    
    void prevItem();
    void nextItem();
    Commands_t command();
};

void MenuGroup_t::prevItem() {
  if(selectedItem>0) selectedItem--;
};

void MenuGroup_t::nextItem() {
  if(selectedItem<menuItems.size()-1) selectedItem++;
};

Commands_t MenuGroup_t::command() {
  return menuItems[selectedItem].command;
};

/* ==============================================================================
   Menu
   Holds all MenuGroup_t 's together 
   Takes care of:
      processing change of selection
      executing commands
      all screen drawing
   ============================================================================== */

class Menu {
  public: 
    Menu(TFT_eSPI *tft, TFT_eSprite *sprite, Data *data); // constructor
    
    Data *data;
    TFT_eSPI *tft;       
    TFT_eSprite *sprite;

    std::map<Menus_t, MenuGroup_t> menuStructure;

    Menus_t currentMenu = mnHomeScreen;
    Context_t context = cxNone;

    int menuStartsAtRow=0;
    
    void drawScreen();
    void selectMenu(enum Menus_t newMenu);
    void buttonUp();
    void buttonDown();
    void buttonSelect();    

    void drawHomeScreenBackground();
    void drawMenuTitle(string title);
    void drawSubTitle(string title1, std::string title2);
    void drawMenu();
    void drawTemperatureSubTitle();
    
    void drawHomeTimeSubTitle();
    void drawOfficeTimeSubTitle();
    void drawWeekendTimeSubTitle();
    void drawSensorOffsetSubTitle();
    
    void periodicScreenUpdate();
    void drawMeasuredTemperature();
    void drawTemperatureSetpoint();
    void drawDateAndTime();
    void drawBoiler();
    void drawIcons();
};

// Constructor
Menu::Menu(TFT_eSPI *tft, TFT_eSprite *sprite, Data *data) {

  // Store the adress of the real time data and the settings
  this->tft=tft;
  this->sprite=sprite;
  this->data=data;

  // Distribute the flat list of menu items over different menu groups
  for(const menuItem_t &menuItem : ALL_MENU_ITEMS) {

    Serial.println(menuItem.label); 

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
  drawScreen();
}

void Menu::drawScreen() {

  static Menus_t previousMenu = mnSetWeekendTimes;
  time_t now;
  struct tm* tm;
  char buffer1[80];
  char buffer2[80];

  if(currentMenu != previousMenu) {
    tft->fillScreen(CLR_BACKGROUND);
    if(currentMenu==mnHomeScreen) drawHomeScreenBackground();
    previousMenu=currentMenu;
  }
  
  menuStartsAtRow=4;
  
  switch(currentMenu) {

    case mnHomeScreen:
      drawMeasuredTemperature();
      drawDateAndTime();
      drawBoiler();
      drawTemperatureSetpoint();
      drawIcons();
      break;
    
    case mnMain:
      drawMenuTitle("Main menu");
      break;

    case mnOverruleDay:
      drawMenuTitle( (context==cxToday) ? "Overrule today" : "Overrule tomorrow");
      break;

    case mnOverruleMultiple:
      drawMenuTitle("Overrule multiple days");
      time(&now); // Get current time
      tm = localtime(&now);
      tm->tm_mday += data->overrideMultipleCount;
      mktime(tm);
      strftime(buffer1, 80, "%A %e %B", tm);
      snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
      drawSubTitle(string("Set to '")+string(DAY_TYPES[data->overrideMultiple])+"' until", string(buffer2) );
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
      drawMenuTitle("Set work from home times");
      drawHomeTimeSubTitle();
      break;
    
    case mnSetOfficeTimes:
      drawMenuTitle("Set office times");
      drawOfficeTimeSubTitle();
      break;
    
    case mnSetWeekendTimes:
      drawMenuTitle("Set weekend times");
      drawWeekendTimeSubTitle();
      break;

    case mnAdjustSensorOffset:
      drawMenuTitle("Set thermometer offset");
      drawSensorOffsetSubTitle();
      break;
  };

  // Draw the menu if needed
  if(currentMenu!=mnHomeScreen) drawMenu();
  
};

void Menu::buttonUp() {
  if(currentMenu==mnHomeScreen) {
    data->setpointLower();
    drawTemperatureSetpoint();
  }
  else {
    menuStructure[currentMenu].prevItem();
    drawMenu();
  }
};

void Menu::buttonDown() {
  if(currentMenu==mnHomeScreen) {
    data->setpointLower();
    drawTemperatureSetpoint();
  }
  else {
    menuStructure[currentMenu].nextItem();
    drawMenu();
  }
};

void Menu::buttonSelect() {
  dayType_t itemSelected;

  time_t now;
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
      
      if(context==cxToday) {
        data->overrideToday=itemSelected; 
        data->overrideTempNow=false;
      }
      else 
        data->overrideTomorrow=itemSelected; 
      
      selectMenu(mnHomeScreen);
    break;
    
    // Overrule settings for a number of days after tomorrow
    case cmOverruleMultipleMenu:
      selectMenu(mnOverruleMultiple);
    break;

    case cmMultipleDecDays:
      data->overruleMultipleLessDays();
        
      time(&now); // Get current time
      tm = localtime(&now);
      tm->tm_mday += data->overrideMultipleCount;
      mktime(tm);
      strftime(buffer1, 80, "%A %e %B", tm);
      snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
      drawSubTitle(string("Set to '")+DAY_TYPES[data->overrideMultiple]+"' until", string(buffer2) );
      menuStartsAtRow=80;
    break;
    
    case cmMultipleIncDays:
      data->overruleMultipleMoreDays();
        
      time(&now); // Get current time
      tm = localtime(&now);
      tm->tm_mday += data->overrideMultipleCount;
      mktime(tm);
      strftime(buffer1, 80, "%A %e %B", tm);
      snprintf(buffer2, 80, "%s (%d)", buffer1, data->overrideMultipleCount);
      drawSubTitle(string("Set to '"+DAY_TYPES[data->overrideMultiple]+"' until"), string(buffer2) );
      menuStartsAtRow=80;
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
      data->overruleMultipleAutomatic();
      selectMenu(mnHomeScreen);
    break;
    
    case cmOverruleMultipleReturn:
           if (menuStructure[mnOverruleMultiple2].selectedItem==0) data->overrideMultiple=dtWorkFromHome;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==1) data->overrideMultiple=dtWorkAtOffice;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==2) data->overrideMultiple=dtAway;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==3) data->overrideMultiple=dtWeekend;
      else if (menuStructure[mnOverruleMultiple2].selectedItem==4) data->overrideMultiple=dtAuto;

      // Reset today and tomorrow if multiple days are selected (can be overruled by the user lateron)
      if(data->overrideMultiple!=dtAuto) {
        if(data->overrideMultipleCount>0) data->overrideToday    = dtAuto;
        if(data->overrideMultipleCount>1) data->overrideTomorrow = dtAuto;
        data->overrideTempNow=false;
      }
      
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
      drawScreen();
    break;
    
    case cmWorkFromOfficeTimesMenu:
      selectMenu(mnSetOfficeTimes);
    break;
    
    case cmWeekendTimesMenu:
      selectMenu(mnSetWeekendTimes);
    break;

    case cmSensorOffsetMenu:
      selectMenu(mnAdjustSensorOffset);
    break;
    
    case cmHighTempInc:
      data->highTempHigher();
      drawTemperatureSubTitle();
    break;
    
    case cmHighTempDec:
      data->highTempLower();
      drawTemperatureSubTitle();
    break;
    
    case cmLowTempInc:
      data->lowTempHigher();
      drawTemperatureSubTitle();
    break;
    
    case cmLowTempDec:
      data->lowTempLower();
      drawTemperatureSubTitle();
    break;
    
    case cmOverruleDaySettingMenu:
      // Determine which day is displayed in the menu
           if (menuStructure[mnSetWeekSchedule].selectedItem==0) { context=cxMonday;    itemSelected=data->regularWeek[1]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==1) { context=cxTuesday;   itemSelected=data->regularWeek[2]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==2) { context=cxWednesday; itemSelected=data->regularWeek[3]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==3) { context=cxThursday;  itemSelected=data->regularWeek[4]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==4) { context=cxFriday;    itemSelected=data->regularWeek[5]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==5) { context=cxSaturday;  itemSelected=data->regularWeek[6]; }
      else if (menuStructure[mnSetWeekSchedule].selectedItem==6) { context=cxSunday;    itemSelected=data->regularWeek[0]; }

      // Preselect the current setting in the day menu
           if (itemSelected==dtWorkFromHome) menuStructure[mnSetDaySchedule].selectedItem=0;
      else if (itemSelected==dtWorkAtOffice) menuStructure[mnSetDaySchedule].selectedItem=1;
      else if (itemSelected==dtAway        ) menuStructure[mnSetDaySchedule].selectedItem=2;
      else if (itemSelected==dtWeekend     ) menuStructure[mnSetDaySchedule].selectedItem=3;

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
      if( data->workFromHomeSleep - data->workFromHomeWakeUp >= timeValue_t(0,30) ) {
        data->workFromHomeWakeUp+=timeValue_t(0,15);
        drawHomeTimeSubTitle();
      }
    break;
    
    case cmHomeWakeUpDec:
      if(data->workFromHomeWakeUp >= timeValue_t(0,15)) {
        data->workFromHomeWakeUp-=timeValue_t(0,15);
        drawHomeTimeSubTitle();
      }
    break;
    
    case cmHomeSleepInc:
      if(data->workFromHomeSleep < timeValue_t(23,45)) {
        data->workFromHomeSleep+=timeValue_t(0,15);
        drawHomeTimeSubTitle();
      }
    break;
    
    case cmHomeSleepDec:
      if( data->workFromHomeSleep - data->workFromHomeWakeUp > timeValue_t(0,30) ) {
        data->workFromHomeSleep-=timeValue_t(0,15);
        drawHomeTimeSubTitle();
      }
    break;
    
    case cmOfficeWakeUpInc:
      if( data->workAtOfficeGoOut - data->workAtOfficeWakeUp > timeValue_t(0,30)) {
        data->workAtOfficeWakeUp+=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeWakeUpDec:
      if( data->workAtOfficeWakeUp >= timeValue_t(0,15 )) {
        data->workAtOfficeWakeUp-=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeLeaveInc:
      if( data->workAtOfficeComeIn - data->workAtOfficeGoOut > timeValue_t(0,30) ) {
        data->workAtOfficeGoOut+=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeLeaveDec:
      if( data->workAtOfficeGoOut - data->workAtOfficeWakeUp > timeValue_t(0,30) ) {
        data->workAtOfficeGoOut-=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeComeHomeInc:
      if( data->workAtOfficeSleep - data->workAtOfficeComeIn > timeValue_t(0,30) ) {
        data->workAtOfficeComeIn+=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeComeHomeDec:
      if( data->workAtOfficeComeIn - data->workAtOfficeGoOut > timeValue_t(0,30) ) {
        data->workAtOfficeComeIn-=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeSleepInc:
      if( data->workAtOfficeSleep < timeValue_t(23,45) ) {
        data->workAtOfficeSleep+=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmOfficeSleepDec:
      if( data->workAtOfficeSleep - data->workAtOfficeComeIn > timeValue_t(0,30) ) {
        data->workAtOfficeSleep-=timeValue_t(0,15);
        drawOfficeTimeSubTitle();
      }
    break;
    
    case cmWeekendWakeUpInc:
      if( data->weekendSleep - data->weekendWakeUp > timeValue_t(0,30) ) {
        data->weekendWakeUp+=timeValue_t(0,15);
        drawWeekendTimeSubTitle();
      }
    break;
    
    case cmWeekendWakeUpDec:
      if( data->weekendWakeUp >= timeValue_t(0,15) ) {
        data->weekendWakeUp-=timeValue_t(0,15);
        drawWeekendTimeSubTitle();
      }
    break;
    
    case cmWeekendSleepInc:
      if( data->weekendSleep < timeValue_t(23,45) ) {
        data->weekendSleep+=timeValue_t(0,15);
        drawWeekendTimeSubTitle();
      }
    break;
    
    case cmWeekendSleepDec:
      if( data->weekendSleep - data->weekendWakeUp> timeValue_t(0,30) ) {
        data->weekendSleep-=timeValue_t(0,15);
        drawWeekendTimeSubTitle();
      }
    break;

    case cmSensorOffsetInc:
      data->measuredTemperatureOffset+=0.1;
      //data->forceMessageToController=true;
      drawSensorOffsetSubTitle();
    break;
    
    case cmSensorOffsetDec:
      data->measuredTemperatureOffset-=0.1;
      //data->forceMessageToController=true;
      drawSensorOffsetSubTitle();
    break;
  } // switch (menuStructure[currentMenu].command)
};

void Menu::drawHomeScreenBackground() {
  
  // Draw the background for the homescreen
  tft->fillRoundRect(4,  45, 200, 46, 23, CLR_CIRCLE_OUTLINE);
  tft->fillRoundRect(6,  47, 196, 42, 21, CLR_CIRCLE_FILL   );

  tft->fillRoundRect(4,  97, 200, 46, 23, CLR_CIRCLE_OUTLINE);
  tft->fillRoundRect(6,  99, 196, 42, 21, CLR_CIRCLE_FILL   );

  tft->fillRoundRect(4, 149, 200, 46, 23, CLR_CIRCLE_OUTLINE);
  tft->fillRoundRect(6, 151, 196, 42, 21, CLR_CIRCLE_FILL   );

  drawJpeg(tft, ICONS[iconSetpoint].c_str(), 17,  154);

  tft->fillCircle(197, 120, 84, CLR_CIRCLE_OUTLINE);
  tft->fillCircle(197, 120, 82, CLR_CIRCLE_FILL   );

  // Centigrade symbol for measured temperature
  tft->setTextDatum(TL_DATUM);
  tft->loadFont(FONT_TEMP_MEASURED_DEG);
  tft->setTextColor(CLR_MEASURED_TEMP, CLR_CIRCLE_FILL);
  tft->drawString("°C", 239, 96);

  drawJpeg(tft, ICONS[iconTempHigher].c_str(), 288,   0);
  drawJpeg(tft, ICONS[iconMenu      ].c_str(), 288,  80);
  drawJpeg(tft, ICONS[iconTempLower ].c_str(), 288, 160);
};

void Menu::periodicScreenUpdate() {
  // Redraw date and time several tims a minute
  const int INTERVAL = 10000; 
  static unsigned long previousAttempt = 0;

  if (data->forceScreenUpdate or (millis() - previousAttempt > INTERVAL) ) {
    previousAttempt=millis();
    data->forceScreenUpdate=false;

    drawDateAndTime();
    drawBoiler();
    drawTemperatureSetpoint();
    drawMeasuredTemperature();
    drawIcons();
  }; 
};

void Menu::drawMeasuredTemperature() {

  char temperature[12]; 

  if(currentMenu==mnHomeScreen) {  
    sprintf(temperature, "%.1f",data->measuredTemperature);
    sprite->createSprite(116, 48);
    sprite->fillSprite(CLR_CIRCLE_FILL);    
    sprite->setTextDatum(TR_DATUM);
    sprite->loadFont(FONT_TEMP_MEASURED); 
    sprite->setTextColor(CLR_MEASURED_TEMP, CLR_CIRCLE_FILL);
    sprite->drawString(temperature, 116, 0);
    sprite->pushSprite(120, 96);
    sprite->unloadFont();
    sprite->deleteSprite();
  };
};

void Menu::drawDateAndTime() {

  const int W=288; // Width of the sprite
  const int H=26;  // Height of the sprite
  char buffer[32]; 
  time_t now;
  struct tm * local;

  if(currentMenu==mnHomeScreen) {  
    if(data->timeSynched) {
      time(&now); // Get current time
      local = localtime(&now);
      strftime(buffer, 32, "%A %e %B", local);
      //sprintf(buffer,"Wednesday 30 September"); // Longest date possible to check if it fits the screen
    
      sprite->createSprite(W, H);
      sprite->fillSprite(CLR_BACKGROUND);    
      sprite->setTextDatum(MC_DATUM);
      sprite->loadFont(FONT_DATE); 
      sprite->setTextColor(CLR_DARK_TEXT, CLR_BACKGROUND);
      sprite->drawString(buffer, W/2, H/2);
      sprite->pushSprite(0, 4);
      sprite->unloadFont();
      sprite->deleteSprite();
    };
    
    sprite->createSprite(80, 32);
    sprite->fillSprite(CLR_CIRCLE_FILL);    
    if(data->timeSynched) {
      strftime(buffer, 32, "%H:%M", local);
    }
    else {
      sprintf(buffer,"--:--"); 
    }
    sprite->setTextDatum(MC_DATUM);
    sprite->loadFont(FONT_TIME);
    sprite->setTextColor(CLR_DARK_TEXT, CLR_CIRCLE_FILL);
    sprite->drawString(buffer, 40, 16);
    sprite->pushSprite(32, 52);
    sprite->unloadFont();
    sprite->deleteSprite();
  };
};

void Menu::drawTemperatureSetpoint() {

  char temperature[12]; 

  if(currentMenu==mnHomeScreen) {  
    sprintf(temperature, "%.1f",data->temperatureSetpoint);
  
    sprite->createSprite(67, 25);
    sprite->fillSprite(CLR_CIRCLE_FILL);    
    sprite->setTextDatum(TL_DATUM);
    sprite->loadFont(FONT_TIME);
    sprite->setTextColor(data->overrideTempNow ? CLR_RED_TEXT : CLR_DARK_TEXT, CLR_CIRCLE_FILL);
    sprite->drawString(temperature, 0, 0);
    int x=sprite->textWidth(temperature)+1;
  
    // Centigrade symbol for setpoint temperature
    sprite->loadFont(FONT_TEMP_SETPOINT_DEG);
    sprite->drawString("°C", x, 0);
  
    sprite->pushSprite(53, 161);
    sprite->unloadFont();
    sprite->deleteSprite();
  };
};

void Menu::drawBoiler() {
  enum icon_t flame;
  char buffer[120];

  if(currentMenu==mnHomeScreen) {  
    if      (data->boilerPercentage < 0.05) flame = iconFlame0; 
    else if (data->boilerPercentage < 0.10) flame = iconFlame1; 
    else if (data->boilerPercentage < 0.15) flame = iconFlame2; 
    else if (data->boilerPercentage < 0.20) flame = iconFlame3; 
    else if (data->boilerPercentage < 0.35) flame = iconFlame4; 
    else if (data->boilerPercentage < 0.50) flame = iconFlame5; 
    else if (data->boilerPercentage < 0.65) flame = iconFlame6; 
    else if (data->boilerPercentage < 0.80) flame = iconFlame7; 
    else                                    flame = iconFlame8; 
  
    drawJpeg(tft, ICONS[flame].c_str(), 17, 107);  
    drawJpeg(tft, ICONS[data->isHotWaterActive ? iconShowerOn      : iconShowerOff].c_str(), 43, 104);
    drawJpeg(tft, ICONS[data->wifiConnected    ? iconWifiConnected : iconWifiLost ].c_str(), 78, 104);

    if(true) {
        // Show PID parameters on screen for debugging purposes

        tft->loadFont(FONT_SUBMENU);
        tft->setTextDatum(MC_DATUM);
        tft->setTextColor(CLR_BLUE_TEXT, CLR_CIRCLE_FILL);

        //snprintf(buffer,120, "  %s  ", data->responseStatus);
        //tft->drawString(buffer, 197,  46);

        snprintf(buffer,120, "  Treq=%.2f°C  ", data->requestedBoilerTemperature);
        tft->drawString(buffer, 197,  65);

        snprintf(buffer,120, "  P=%.2f°C I=%.2f°C  ", data->P, data->I);
        tft->drawString(buffer, 197,   84);

        snprintf(buffer,120, "  HEATING %s  ", data->isCentralHeatingActive ? "ON" : "OFF" );
        tft->drawString(buffer, 197,  154);

        snprintf(buffer,120, "  FLAME %s  ", data->isFlameOn ? "ON" : "OFF" );
        tft->drawString(buffer, 197,  172);

        snprintf(buffer,120, "  Tact=%.2f°C  ", data->actualBoilerTemperature);
        tft->drawString(buffer, 197,  190);
    };
  };
};

void Menu::drawIcons() {
  int x=8;
  if(currentMenu==mnHomeScreen) {  
    data->setIcons();
    for(int i=0; i<7; i++) {
      drawJpeg(tft, ICONS[data->icons[i]].c_str(), x, 208);
      x+=40;
    };
  };  
};

void Menu::drawMenuTitle(string title) {
  tft->loadFont(FONT_MENU_HEADER);
  tft->setTextDatum(TC_DATUM);
  tft->setTextColor(CLR_MENU_TITLE, CLR_BACKGROUND);
  tft->drawString(title.c_str(), 160, 4);
  menuStartsAtRow=32;
};

void Menu::drawSubTitle(string title1, string title2) {
  tft->fillRect(0, 32, 320, 79-32, CLR_BACKGROUND);
  tft->loadFont(FONT_SUBMENU);
  tft->setTextDatum(TC_DATUM);
  tft->setTextColor(CLR_MENU_SUBTITLE, CLR_BACKGROUND);
  tft->drawString(title1.c_str(), 160, 32);
  tft->drawString(title2.c_str(), 160, 55);
  menuStartsAtRow=80;
}

void Menu::drawHomeTimeSubTitle() {
  drawSubTitle(string("Wake up at ")+data->workFromHomeWakeUp.str(), string("Go to sleep at ")+data->workFromHomeSleep.str());
}

void Menu::drawOfficeTimeSubTitle() {
  drawSubTitle(string("Wake up at ")  +data->workAtOfficeWakeUp.str()+" go out at "     +data->workAtOfficeGoOut.str(), 
               string("come home at ")+data->workAtOfficeComeIn.str()+" go to sleep at "+data->workAtOfficeSleep.str() );
}

void Menu::drawSensorOffsetSubTitle() {
  char buffer[80];
  snprintf(buffer, 80, "set to %.1f°C", data->measuredTemperatureOffset);
  drawSubTitle("Temperature sensor offset", buffer);
}

void Menu::drawWeekendTimeSubTitle() {
  drawSubTitle(string("Wake up at ")+data->weekendWakeUp.str(), string("Go to sleep at ")+data->weekendSleep.str());
}

void Menu::drawTemperatureSubTitle() {
  char buffer1[80];
  char buffer2[80];
  snprintf(buffer1, 80, "High temperature set to %.1f°C", data->highTemp);
  snprintf(buffer2, 80, "Low temperature set to %.1f°C",  data->lowTemp );
  drawSubTitle(buffer1, buffer2);
}

void Menu::drawMenu() {
  const int L=40;             // Left coordinate of sprite on screen
  const int W=tft->width()-L; // Width of the sprite
  const int H=32;             // Height of the sprite

  int numRows=(int) (tft->height()-menuStartsAtRow)/H;
  
  // The offset is needed to display rows that are off the grid
  int iOffset=min(max(menuStructure[currentMenu].selectedItem-numRows+2,0),menuStructure[currentMenu].menuItems.size()-numRows);

  sprite->createSprite(W, H);
  sprite->setTextDatum(ML_DATUM);
  sprite->loadFont(FONT_MENU); // Noway Regular 24 pixels high
  sprite->setTextColor(CLR_MENU_TEXT, CLR_BACKGROUND);
  sprite->fillSprite(CLR_BACKGROUND);    

  int y=menuStartsAtRow-iOffset*H;
  uint8_t currentItem=0;
  for(const menuItem_t &menuItem : menuStructure[currentMenu].menuItems) {

    if(y>=menuStartsAtRow) {
      sprite->fillSprite(CLR_BACKGROUND);
  
      if(currentItem == menuStructure[currentMenu].selectedItem) {
        sprite->fillRoundRect(0, 0, W,   H,   H/2,   CLR_MENU_LINE);
        sprite->fillRoundRect(3, 3, W-6, H-6, H/2-3, CLR_BACKGROUND);
      }
      
      sprite->drawString(menuItem.label, H/2, H/2);
      sprite->pushSprite(L, y);
  
      if(menuItem.icon != iconNone) {
        int iconID = menuItem.icon;      
        drawJpeg(tft, ICONS[iconID].c_str(), 2, y);
      }
    }
    y+=H;
    currentItem++;
  }
  sprite->fillSprite(CLR_BACKGROUND);
  sprite->pushSprite(0, y);
  sprite->pushSprite(L, y);
  sprite->unloadFont();
  sprite->deleteSprite();
};
