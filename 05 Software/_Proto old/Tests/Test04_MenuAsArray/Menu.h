#pragma once

#include <M5Stack.h>
#include "Free_Fonts.h"
#include "Constants.h"
#include <map>
#include <list>

using namespace std;

TFT_eSprite tftSprite = TFT_eSprite(&M5.Lcd);

// List of available menus
enum Menus_t {mnHomeScreen, mnMain, mnOverruleDay, mnOverruleMultiple, mnSettings, mnSetWeekSchedule, mnSetDaySchedule,
  mnSetHomeTimes, mnSetOfficeTimes, mnSetWeekendTimes };

// List of commands that result from a menu press
enum Commands_t {cmHighTempScreen, cmHomeScreen, cmHomeSleepTimeScreen, cmHomeWakeUpTimeScreen, cmLowTempScreen, 
  cmMainMenu, cmMultipleDecDays, cmMultipleIncDays, cmOfficeHomeTimeScreen, cmOfficeLeaveTimeScreen, cmOfficeSleepTimeScreen, 
  cmOfficeWakeUpTimeScreen, cmOverruleDayAuto, cmOverruleDayAway, cmOverruleDayHome, cmOverruleDayMenu, cmOverruleDayOffice, 
  cmOverruleDaySettingAway, cmOverruleDaySettingHome, cmOverruleDaySettingMenu, cmOverruleDaySettingOffice, cmOverruleDaySettingWeekend, 
  cmOverruleDayWeekend, cmOverruleMultipleAutomatc, cmOverruleMultipleAway, cmOverruleMultipleHome, cmOverruleMultipleMenu, 
  cmOverruleMultipleOffice, cmOverruleMultipleWeekend, cmSettingsMenu, cmWeekendTimesMenu, cmWeekScheduleMenu, cmWorkFromHomeTimesMenu, 
  cmWorkFromOfficeTimesMenu };

// List of subjects as context for hte handler function
enum Contexts_t { cxNone, cxToday, cxTomorrow, cxMonday, cxTuesday, cxWednesday, cxThursday, cxFriday, cxSaturday, cxSunday };

class menuItem_t {
  public: 
    enum Menus_t menu;            // Menu to which this item belongs
    char label[27];               // Text to show. ToDo: non-fixed size
    enum Commands_t command;      // Command to be executed when select button is pressed
    enum Contexts_t context;      // Optional context for submenu (cxNone if not required)
    bool operator==(menuItem_t& other) const;
};

bool menuItem_t::operator==(menuItem_t& other) const
{
    //return (menu == other.menu);
    return ( (menu == other.menu) && (strcmp(label, other.label)==0) );
    //return (menu == (menuItem_t) other.menu) && (strcmp(label, (menuItem_t) other.label)==0);
}

static const menuItem_t ALL_MENU_ITEMS[] = {  

// Menu group          Label                         Command when selected        Context

  {mnHomeScreen,       "None",                       cmMainMenu,                  cxNone      },
  {mnMain,             "Overrule today",             cmOverruleDayMenu,           cxToday     },
  {mnMain,             "Overrule tomorrow",          cmOverruleDayMenu,           cxTomorrow  },
  {mnMain,             "Overrule multiple days",     cmOverruleMultipleMenu,      cxNone      },
  {mnMain,             "Settings",                   cmSettingsMenu,              cxNone      },
  {mnMain,             "Exit menu",                  cmHomeScreen,                cxNone      },
  {mnOverruleDay,      "Work from home",             cmOverruleDayHome,           cxNone      },
  {mnOverruleDay,      "Work from office",           cmOverruleDayOffice,         cxNone      },
  {mnOverruleDay,      "All day away",               cmOverruleDayAway,           cxNone      },
  {mnOverruleDay,      "Weekend",                    cmOverruleDayWeekend,        cxNone      },
  {mnOverruleDay,      "Automatic",                  cmOverruleDayAuto,           cxNone      },
  {mnOverruleDay,      "Back to main menu",          cmMainMenu,                  cxNone      },
  {mnOverruleMultiple, "More days",                  cmMultipleIncDays,           cxNone      },
  {mnOverruleMultiple, "Fewer days",                 cmMultipleDecDays,           cxNone      },
  {mnOverruleMultiple, "Work from home",             cmOverruleMultipleHome,      cxNone      },
  {mnOverruleMultiple, "Work from office",           cmOverruleMultipleOffice,    cxNone      },
  {mnOverruleMultiple, "All day away",               cmOverruleMultipleAway,      cxNone      },
  {mnOverruleMultiple, "Weekend",                    cmOverruleMultipleWeekend,   cxNone      },
  {mnOverruleMultiple, "Automatic",                  cmOverruleMultipleAutomatc,  cxNone      },
  {mnOverruleMultiple, "Back to main menu",          cmMainMenu,                  cxNone      },
  {mnSettings,         "Set low temperature",        cmLowTempScreen,             cxNone      },
  {mnSettings,         "Set high temperature",       cmHighTempScreen,            cxNone      },
  {mnSettings,         "Set week schedule",          cmWeekScheduleMenu,          cxNone      },
  {mnSettings,         "Set work from home times",   cmWorkFromHomeTimesMenu,     cxNone      },
  {mnSettings,         "Set work from office times", cmWorkFromOfficeTimesMenu,   cxNone      },
  {mnSettings,         "Set weekend times",          cmWeekendTimesMenu,          cxNone      },
  {mnSettings,         "Back to main menu",          cmMainMenu,                  cxNone      },
  {mnSetWeekSchedule,  "Monday",                     cmOverruleDaySettingMenu,    cxMonday    },
  {mnSetWeekSchedule,  "Tuesday",                    cmOverruleDaySettingMenu,    cxTuesday   },
  {mnSetWeekSchedule,  "Wednesday",                  cmOverruleDaySettingMenu,    cxWednesday },
  {mnSetWeekSchedule,  "Thursday",                   cmOverruleDaySettingMenu,    cxThursday  },
  {mnSetWeekSchedule,  "Friday",                     cmOverruleDaySettingMenu,    cxFriday    },
  {mnSetWeekSchedule,  "Saturday",                   cmOverruleDaySettingMenu,    cxSaturday  },
  {mnSetWeekSchedule,  "Sunday",                     cmOverruleDaySettingMenu,    cxSunday    },
  {mnSetWeekSchedule,  "Back to settings menu",      cmSettingsMenu,              cxNone      },
  {mnSetDaySchedule,   "Work from home",             cmOverruleDaySettingHome,    cxNone      },
  {mnSetDaySchedule,   "Work from office",           cmOverruleDaySettingOffice,  cxNone      },
  {mnSetDaySchedule,   "All day away",               cmOverruleDaySettingAway,    cxNone      },
  {mnSetDaySchedule,   "Weekend",                    cmOverruleDaySettingWeekend, cxNone      },
  {mnSetDaySchedule,   "Back to week schedule",      cmWeekScheduleMenu,          cxNone      },
  {mnSetHomeTimes,     "Wake up time",               cmHomeWakeUpTimeScreen,      cxNone      },
  {mnSetHomeTimes,     "Go to sleep time",           cmHomeSleepTimeScreen,       cxNone      },
  {mnSetHomeTimes,     "Back to settings menu",      cmSettingsMenu,              cxNone      },
  {mnSetOfficeTimes,   "Wake up time",               cmOfficeWakeUpTimeScreen,    cxNone      },
  {mnSetOfficeTimes,   "Leave home time",            cmOfficeLeaveTimeScreen,     cxNone      },
  {mnSetOfficeTimes,   "Come home time",             cmOfficeHomeTimeScreen,      cxNone      },
  {mnSetOfficeTimes,   "Go to sleep time",           cmOfficeSleepTimeScreen,     cxNone      },
  {mnSetOfficeTimes,   "Back to settings menu",      cmSettingsMenu,              cxNone      },
  {mnSetWeekendTimes,  "Wake up time",               cmHomeWakeUpTimeScreen,      cxNone      },
  {mnSetWeekendTimes,  "Go to sleep time",           cmHomeSleepTimeScreen,       cxNone      },
  {mnSetWeekendTimes,  "Back to settings menu",      cmSettingsMenu,              cxNone      },
};

// All lists of menus together
class MenuGroup_t {
  public:
    bool itemSelected=true;
    uint8_t selectedItem=0;
    
    std::vector<menuItem_t> menuItems;
    
    void drawMenu();
    void prevItem();
    void nextItem();
    enum Commands_t command();
    enum Contexts_t context();
};

void MenuGroup_t::drawMenu() {
  
  int y=2;
  const int W=230;
  const int H=28;
  const int L=(320-W)/2;

  tftSprite.createSprite(280, H);
  tftSprite.setTextDatum(MC_DATUM);
  tftSprite.setFreeFont(FF17);
  tftSprite.fillSprite(WHITE);    

  uint8_t currentItem=0;
  for(const menuItem_t &menuItem : menuItems) {

    bool displayAsSelected = ( itemSelected and (currentItem ==selectedItem ));
    
    tftSprite.fillRoundRect(0, 0, W, H, 12, displayAsSelected ? CLR_BTN_SELECT : CLR_BTN_UNSELECT);
    tftSprite.setTextColor(TFT_BLACK,       displayAsSelected ? CLR_BTN_SELECT : CLR_BTN_UNSELECT);
    tftSprite.drawString(menuItem.label, W/2, 12);
    tftSprite.pushSprite(L, y);

    Serial.printf("%s y=%d w=%d h=%d\n", menuItem.label, y, tftSprite.textWidth(menuItem.label), tftSprite.fontHeight());
    y+=H+1;
    currentItem++;
  }
  Serial.printf("This menu has %d items\n", menuItems.size());
  
  tftSprite.deleteSprite();
}

void MenuGroup_t::prevItem() {
  if(selectedItem>0) {
    selectedItem--;
    drawMenu();
  }
};

void MenuGroup_t::nextItem() {
  if(selectedItem<menuItems.size()-1) {
    selectedItem++;
    drawMenu();
  }
};

enum Commands_t MenuGroup_t::command() {
  /*
  enum Commands_t selectedCommand;
  selectedCommand=menuItems[selectedItem].command;
  return selectedCommand;
  */
  return menuItems[selectedItem].command;
};

enum Contexts_t MenuGroup_t::context() {
  return menuItems[selectedItem].context;
};

// Menu class containing the whole menu structure
class Menu {
  public: 
    Menu(); // constructor
    std::map<Menus_t, MenuGroup_t> menuStructure;

    enum Menus_t currentMenu = mnMain;
    enum Contexts_t currentContext = cxNone;

    void drawMenu();
    void selectMenu(enum Menus_t newMenu);
    void buttonUp();
    void buttonDown();
    void buttonSelect();    
};

// Constructor
Menu::Menu() {

  tftSprite.setFreeFont(FF17);

  // Distribute the flat list of menu items over different menu groups
  for(const menuItem_t &menuItem : ALL_MENU_ITEMS) {

    Serial.printf("%s\t%d\n", menuItem.label, tftSprite.textWidth(menuItem.label));
    
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
  M5.Lcd.fillScreen(WHITE);

  switch(currentMenu) {

    default:
      drawMenu();  
  }
}

void Menu::drawMenu() {
  menuStructure[currentMenu].drawMenu();
};

void Menu::buttonUp() {
  menuStructure[currentMenu].prevItem();
};

void Menu::buttonDown() {
  menuStructure[currentMenu].nextItem();
};

void Menu::buttonSelect() {
  switch (menuStructure[currentMenu].command()) {
    
    case cmMainMenu:
      selectMenu(mnMain);
      break;

    case cmOverruleDayMenu:
      currentContext = menuStructure[currentMenu].context();
      selectMenu(mnOverruleDay);
      break;

    case cmOverruleMultipleMenu:
      selectMenu(mnOverruleMultiple);
      break;

    case cmSettingsMenu:
      selectMenu(mnSettings);
      break;
      
    case cmHomeScreen:
      selectMenu(mnHomeScreen);
      break;
  };
    /*
    cmOverruleDayHome,           
    cmOverruleDayOffice,         
    cmOverruleDayAway,           
    cmOverruleDayWeekend,        
    cmOverruleDayAuto,           
    cmMainMenu,                  
    cmMultipleIncDays,           
    cmMultipleDecDays,           
    cmOverruleMultipleHome,      
    cmOverruleMultipleOffice,    
    cmOverruleMultipleAway,      
    cmOverruleMultipleWeekend,   
    cmOverruleMultipleAutomatc,  
    cmMainMenu,                  
    cmLowTempScreen,             
    cmHighTempScreen,            
    cmWeekScheduleMenu,          
    cmWorkFromHomeTimesMenu,     
    cmWorkFromOfficeTimesMenu,   
    cmWeekendTimesMenu,          
    cmMainMenu,                  
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmOverruleDaySettingMenu,    
    cmSettingsMenu,              
    cmOverruleDaySettingHome,    
    cmOverruleDaySettingOffice,  
    cmOverruleDaySettingAway,    
    cmOverruleDaySettingWeekend, 
    cmWeekScheduleMenu,          
    cmHomeWakeUpTimeScreen,      
    cmHomeSleepTimeScreen,       
    cmSettingsMenu,              
    cmOfficeWakeUpTimeScreen,    
    cmOfficeLeaveTimeScreen,     
    cmOfficeHomeTimeScreen,      
    cmOfficeSleepTimeScreen,     
    cmSettingsMenu,              
    cmHomeWakeUpTimeScreen,      
    cmHomeSleepTimeScreen,       
    cmSettingsMenu,              
    */
};
