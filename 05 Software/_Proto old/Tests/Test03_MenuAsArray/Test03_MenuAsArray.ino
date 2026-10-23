#include <map>
#include <list>

using namespace std;

enum Menus_t {mnHomeScreen, mnMain, mnOverruleDay, mnOverruleMultiple, mnSettings, mnSetWeekSchedule, mnSetDaySchedule,
  mnSetHomeTimes, mnSetOfficeTimes, mnSetWeekendTimes };

enum Subjects_t {sbNone, sbToday, sbTomorrow, sbMonday, sbTuesday, sbWednesday, sbThursday, sbFriday, sbSaturday, sbSunday};

// Functions to be called if menu items are selected
void mainMenu() {};
void overruleDayMenu() {};
void overruleMultipleMenu() {};
void settingsMenu() {};
void homeScreen() {};
void overruleDayHome() {};
void overruleDayOffice() {};
void overruleDayAway() {};
void overruleDayWeekend() {};
void overruleDayAuto() {};
void multipleIncDays() {};
void multipleDecDays() {};
void overruleMultipleHome() {};
void overruleMultipleOffice() {};
void overruleMultipleAway() {};
void overruleMultipleWeekend() {};
void overruleMultipleAutomatc() {};
void lowTempScreen() {};
void highTempScreen() {};
void weekScheduleMenu() {};
void workFromHomeTimesMenu() {};
void workFromOfficeTimesMenu() {};
void weekendTimesMenu() {};
void overruleDaySettingMenu() {};
void overruleDaySettingHome() {};
void overruleDaySettingOffice() {};
void overruleDaySettingAway() {};
void overruleDaySettingWeekend() {};
void homeWakeUpTimeScreen() {};
void homeSleepTimeScreen() {};
void officeWakeUpTimeScreen() {};
void officeLeaveTimeScreen() {};
void officeHomeTimeScreen() {};
void officeSleepTimeScreen() {};

typedef struct {
  enum Menus_t menu;       // To which menu does this item belong
  char label[27];        // Which text to show
  enum Subjects_t subject; // Optional subject for submenu
  void (*handler)(void); // Function to call when select button is pressed
} menuItem_t;

static const menuItem_t ALL_MENU_ITEMS[] = {  
  { mnHomeScreen,       "None",                      sbNone,     mainMenu                 }, 
  { mnMain,             "Overrule today",            sbToday,    overruleDayMenu          }, 
  { mnMain,             "Overrule tomorrow",         sbTomorrow, overruleDayMenu          }, 
  { mnMain,             "Overrule multiple days",    sbNone,     overruleMultipleMenu     }, 
  { mnMain,             "Settings",                  sbNone,     settingsMenu             }, 
  { mnMain,             "Exit menu",                 sbNone,     homeScreen               }, 
  { mnOverruleDay,      "Work from home",            sbNone,     overruleDayHome          }, 
  { mnOverruleDay,      "Work from office",          sbNone,     overruleDayOffice        }, 
  { mnOverruleDay,      "All day away",              sbNone,     overruleDayAway          }, 
  { mnOverruleDay,      "Weekend",                   sbNone,     overruleDayWeekend       }, 
  { mnOverruleDay,      "Automatic",                 sbNone,     overruleDayAuto          }, 
  { mnOverruleDay,      "Back to main menu",         sbNone,     mainMenu                 }, 
  { mnOverruleMultiple, "More days",                 sbNone,     multipleIncDays          }, 
  { mnOverruleMultiple, "Fewer days",                sbNone,     multipleDecDays          }, 
  { mnOverruleMultiple, "Work from home",            sbNone,     overruleMultipleHome     }, 
  { mnOverruleMultiple, "Work from office",          sbNone,     overruleMultipleOffice   }, 
  { mnOverruleMultiple, "All day away",              sbNone,     overruleMultipleAway     }, 
  { mnOverruleMultiple, "Weekend",                   sbNone,     overruleMultipleWeekend  }, 
  { mnOverruleMultiple, "Automatic",                 sbNone,     overruleMultipleAutomatc }, 
  { mnOverruleMultiple, "Back to main menu",         sbNone,     mainMenu                 }, 
  { mnSettings,         "Set low temperature",       sbNone,     lowTempScreen            }, 
  { mnSettings,         "Set high temperature",      sbNone,     highTempScreen           }, 
  { mnSettings,         "Set week schedule",         sbNone,     weekScheduleMenu         }, 
  { mnSettings,         "Set work from home times",  sbNone,     workFromHomeTimesMenu    }, 
  { mnSettings,         "Set work from office times",sbNone,     workFromOfficeTimesMenu  }, 
  { mnSettings,         "Set weekend times",         sbNone,     weekendTimesMenu         }, 
  { mnSettings,         "Back to main menu",         sbNone,     mainMenu                 }, 
  { mnSetWeekSchedule,  "Monday",                    sbMonday,   overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Tuesday",                   sbTuesday,  overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Wednesday",                 sbWednesday,overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Thursday",                  sbThursday, overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Friday",                    sbFriday,   overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Saturday",                  sbSaturday, overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Sunday",                    sbSunday,   overruleDaySettingMenu   }, 
  { mnSetWeekSchedule,  "Back to settings menu",     sbNone,     settingsMenu             }, 
  { mnSetDaySchedule,   "Work from home",            sbNone,     overruleDaySettingHome   }, 
  { mnSetDaySchedule,   "Work from office",          sbNone,     overruleDaySettingOffice }, 
  { mnSetDaySchedule,   "All day away",              sbNone,     overruleDaySettingAway   }, 
  { mnSetDaySchedule,   "Weekend",                   sbNone,     overruleDaySettingWeekend}, 
  { mnSetDaySchedule,   "Back to week schedule",     sbNone,     weekScheduleMenu         }, 
  { mnSetHomeTimes,     "Wake up time",              sbNone,     homeWakeUpTimeScreen     }, 
  { mnSetHomeTimes,     "Go to sleep time",          sbNone,     homeSleepTimeScreen      }, 
  { mnSetHomeTimes,     "Back to settings menu",     sbNone,     settingsMenu             }, 
  { mnSetOfficeTimes,   "Wake up time",              sbNone,     officeWakeUpTimeScreen   }, 
  { mnSetOfficeTimes,   "Leave home time",           sbNone,     officeLeaveTimeScreen    }, 
  { mnSetOfficeTimes,   "Come home time",            sbNone,     officeHomeTimeScreen     }, 
  { mnSetOfficeTimes,   "Go to sleep time",          sbNone,     officeSleepTimeScreen    }, 
  { mnSetOfficeTimes,   "Back to settings menu",     sbNone,     settingsMenu             }, 
  { mnSetWeekendTimes,  "Wake up time",              sbNone,     homeWakeUpTimeScreen     }, 
  { mnSetWeekendTimes,  "Go to sleep time",          sbNone,     homeSleepTimeScreen      }, 
  { mnSetWeekendTimes,  "Back to settings menu",     sbNone,     settingsMenu             }
};

// 
class MenuGroup_t {
  public:
    bool itemSelected;
    menuItem_t* selectedItem;
    std::list<menuItem_t> menuItems;
};

std::map<Menus_t, MenuGroup_t> menuStructure;
  

void setup() {
  Serial.begin(115200);
  delay(500);

  // Differentiate the flat list of menu items in different menu groups
  for(const menuItem_t &menuItem : ALL_MENU_ITEMS) {
    
    // Create a new key in the menuStructure if the menu does not yet exist
    if(menuStructure.find(menuItem.menu)==menuStructure.end()) {
      MenuGroup_t newMenu;
      menuStructure[menuItem.menu]=newMenu;
    }

    // Store a pointer to the menu item in the appropriate menu
    menuStructure[menuItem.menu].menuItems.push_back(menuItem);
  }

  Serial.println("\nMain menu:");
  for(const menuItem_t &menuItem : menuStructure[mnMain].menuItems) Serial.printf("  - %s\n",menuItem.label);

  Serial.println("\nSet week schedule:");
  for(const menuItem_t &menuItem : menuStructure[mnSetWeekSchedule].menuItems) Serial.printf("  - %s\n",menuItem.label);
}

void loop() {
  // put your main code here, to run repeatedly:

}
