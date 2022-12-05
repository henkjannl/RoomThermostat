#pragma once

#include <map>
#include <list>
#include <string>

using namespace std;

/*************************
 * PINOUT OF PERIPHERALS *
 *************************/
 
// OpenTherm Shield pins configuration
#define PIN_OPENTHERM_IN         22 // beware: in and out were flipped due to mistake creating a symbol in KiCad
#define PIN_OPENTHERM_OUT        21

#define PIN_BACKLIGHT 32

// One wire bus and temperature sensor
#define PIN_ONE_WIRE_BUS 17
#define TEMPERATURE_PRECISION 11 // Resolution 0.125°C. Requires 375 ms to fetch temperature

// Pinout of the display is in the TFT_eSPI config file

// Keyboard
#define PIN_BUT_UP        12      // Capacitive pin for up button
#define PIN_BUT_SELECT     4      // Capacitive pin for select button
#define PIN_BUT_DOWN      15      // Capacitive pin for down button

// Display
// Screen colors converted to RGB565
#define CLR_BACKGROUND       0xFFFF   // FF, FF, FF = white
#define CLR_CIRCLE_OUTLINE   0x738E   // 73, 73, 73 = dark grey
#define CLR_CIRCLE_FILL      0xC618   // C0, C0, C0 = light grey
#define CLR_MEASURED_TEMP    0x0000   // 00, 00, 00 = black
#define CLR_DARK_TEXT        0x8410   // 80, 80, 80 = dark grey
#define CLR_LIGHT_TEXT       0xB596   // B0, B0, B0 = light grey
#define CLR_SETPOINT_TEMP    0xF8C3   // FF, 19, 19 = red
#define CLR_MENU_TITLE       0x0000   // 00, 00, 00 = black
#define CLR_MENU_SUBTITLE    0x8410   // 80, 80, 80 = dark grey
#define CLR_MENU_TEXT        0x0000   // 00, 00, 00 = black
#define CLR_MENU_SELECT      0xF8C3   // FF, 19, 19 = red
#define CLR_BLUE_TEXT        0x1A7F   // 19, 4F, FF = blue
#define CLR_RED_TEXT         0xF8C3   // FF, 19, 19 = red

// Screen fonts
#define FONT_MENU_HEADER        "Noway_Medium24"  // Font Noway_Medium24 height 24
#define FONT_MENU               "Noway_Light18"   // Font Noway_Light18 height 17
#define FONT_SUBMENU            "Noway_Light18"   // Font Noway_Light18 height 17
#define FONT_DATE               "Noway_Light24"   // Font Noway_Light24 height 23
#define FONT_TIME               "Noway_Medium24"  // Font Noway_Medium24 height 24
#define FONT_TEMP_MEASURED      "Noway_Medium60"  // Font Noway_Medium60 height 58
#define FONT_TEMP_MEASURED_DEG  "Noway_Light32"   // Font Noway_Light32 height 31
#define FONT_TEMP_SETPOINT      "Noway_Medium32"  // Font Noway_Medium32 height 31
#define FONT_TEMP_SETPOINT_DEG  "Noway_Light18"   // Font Noway_Light18 height 17
#define FONT_NORMAL             "Noway_Regular24" // Font Noway_Regular24 height 23
#define FONT_MEDIUM             "Noway_Medium24"  // Font Noway_Medium24 height 24

/*
    Font Noway_Medium24 height 24
    Font Noway_Light18 height 17
    Font Noway_Light24 height 23
    Font Noway_Medium60 height 58
    Font Noway_Light32 height 31
    Font Noway_Medium32 height 31
    Font Noway_Regular24 height 23
 */
// Configuration file, containing WiFi access point data
#define CONFIG_FILE "/config.jsn"
#define TEMP_CONFIG "/config.tmp"

// Menu items used in the unit menus
enum umMenuItem_t {mnAllDayAway, mnAutomatic, mnBackToHomeScreen, mnBackToMainMenu, mnBackToPreviousMenu, mnBackToSettingsMenu, mnBackToWeekSchedule, 
  mnComeHomeEarlier, mnComeHomeLater, mnExitMenu, mnFewerDays, mnFriday, mnGoToSleepEarlier, mnGoToSleepLater, mnHighTemperatureDown, mnHighTemperatureUp, 
  mnIncreaseOffset, mnLeaveEarlier, mnLeaveLater, mnLowTemperatureDown, mnLowTemperatureUp, mnMonday, mnMoreDays, mnOverruleMultipleDays, mnOverruleToday, 
  mnOverruleTomorrow, mnReduceOffset, mnResetToAutomatic, mnSaturday, mnSetDaySchedule, mnSetSensorOffset, mnSetTemperatures, mnSetWeekSchedule, mnSetWeekendTimes, 
  mnSettings, mnSunday, mnThursday, mnTuesday, mnWakeUpEarlier, mnWakeUpLater, mnWednesday, mnWeekend, mnWorkFromHome, mnWorkFromHomeTimes, mnWorkAtOffice, 
  mnWorkFromOfficeTimes, mnDummyForHome };

typedef std::map<umMenuItem_t, string> umLabels_t;

umLabels_t umLabel = { 
  { mnAllDayAway, "All day away" },
  { mnAutomatic, "Automatic" },
  { mnBackToHomeScreen, "Back to home screen" },
  { mnBackToMainMenu, "Back to main menu" },
  { mnBackToPreviousMenu, "Back to previous menu" },
  { mnBackToSettingsMenu, "Back to settings menu" },
  { mnBackToWeekSchedule, "Back to week schedule" },
  { mnComeHomeEarlier, "Come home earlier" },
  { mnComeHomeLater, "Come home later" },
  { mnExitMenu, "Exit main menu" },
  { mnFewerDays, "Fewer days" },
  { mnFriday, "Friday" },
  { mnGoToSleepEarlier, "Go to sleep earlier" },
  { mnGoToSleepLater, "Go to sleep later" },
  { mnHighTemperatureDown, "High temperature down" },
  { mnHighTemperatureUp, "High temperature up" },
  { mnIncreaseOffset, "Sensor offset up" },
  { mnLeaveEarlier, "Leave earlier" },
  { mnLeaveLater, "Leave later" },
  { mnLowTemperatureDown, "Low temperature down" },
  { mnLowTemperatureUp, "Low temperature up" },
  { mnMonday, "Monday" },
  { mnMoreDays, "More days" },
  { mnOverruleMultipleDays, "Overrule multiple days" },
  { mnOverruleToday, "Overrule today" },
  { mnOverruleTomorrow, "Overrule tomorrow" },
  { mnReduceOffset, "Sensor offset down" },
  { mnResetToAutomatic, "Reset to automatic" },
  { mnSaturday, "Saturday" },
  { mnSetDaySchedule, "Set day schedule" },
  { mnSetSensorOffset, "Set sensor offset" },
  { mnSetTemperatures, "Set day and night temperatures" },
  { mnSetWeekSchedule, "Set week schedule" },
  { mnSetWeekendTimes, "Set weekend times" },
  { mnSettings, "Settings" },
  { mnSunday, "Sunday" },
  { mnThursday, "Thursday" },
  { mnTuesday, "Tuesday" },
  { mnWakeUpEarlier, "Wake up earlier" },
  { mnWakeUpLater, "Wake up later" },
  { mnWednesday, "Wednesday" },
  { mnWeekend, "Weekend" },
  { mnWorkFromHome, "Work from home" },
  { mnWorkFromHomeTimes, "Work from home times" },
  { mnWorkAtOffice, "Work at the office" },
  { mnWorkFromOfficeTimes, "Work from office times" },
  { mnDummyForHome, "Dummy menu item for homescreen" } };

class Screen {
  public:
    Screen() { }; // Never used, but somehow required when class is used in a map
    Screen(string title, std::list<umMenuItem_t> menu);
    
    void firstItem() { selection=std::begin(menu); };
    void prevItem()  { if( selection!=std::begin(menu) ) --selection; };
    void nextItem()  { if( selection!=--std::end(menu) ) ++selection; };
    
    void selectItem(umMenuItem_t menuItem);
    umMenuItem_t selectedItem() { return *selection; };

    string title;
    
  //protected:
    std::list<umMenuItem_t> menu;
    std::list<umMenuItem_t>::iterator selection;
};

Screen::Screen(string title, std::list<umMenuItem_t> menu) {
  this->title=title;
  this->menu=menu;
  firstItem();  
}

void Screen::selectItem(umMenuItem_t menuItem) {
  bool itemfound=false;
  for(std::list<umMenuItem_t>::iterator item = std::begin(menu); item != std::end(menu); item++) {
    if(*item==menuItem) {
      selection=item;
      itemfound=true;
      break;
    }  
  }

  //Serial.println(itemfound ? "item found" : "item not found");
};

enum umScreenIdentifier_t {scHomeScreen, scMain, scOverruleToday, scOverruleTomorrow, scOverruleMultiple, scOverruleMultiple2, scSettings, scTemperatureSettings, scSetWeekSchedule, 
  scSetDaySchedule, scSetHomeTimes, scSetOfficeTimes, scSetWeekendTimes, scAdjustSensorOffset };

typedef std::map<umScreenIdentifier_t, Screen> umScreen_t;

class MenuStructure{
  public:
    MenuStructure(umScreen_t structure);
    void selectScreen(umScreenIdentifier_t screen) { currentScreen=screen; };
    void selectScreen(umScreenIdentifier_t screen, umMenuItem_t menuItem) { currentScreen=screen; screens[screen].selectItem(menuItem); };
    void selectMenuItem(umScreenIdentifier_t screen, umMenuItem_t menuItem) { screens[screen].selectItem(menuItem); };

    void prevMenuItem()  { screens[currentScreen].prevItem(); };
    void nextMenuItem()  { screens[currentScreen].nextItem(); };
    void firstMenuItem() { screens[currentScreen].firstItem(); };
    
    umScreenIdentifier_t selectedScreen() { return currentScreen; };
    string currentScreenTitle() { return screens[currentScreen].title; };
    umMenuItem_t selectedMenuItem();
    std::list<umMenuItem_t> menuItems() { return screens[currentScreen].menu; };   

    void printFirsts();
    
  protected:
    umScreenIdentifier_t currentScreen; 
    std::map<umScreenIdentifier_t, Screen> screens;
};

MenuStructure::MenuStructure(umScreen_t structure) {
  screens=structure;
  currentScreen=(*std::begin(screens)).first; // By default, select the first screen
  for(auto & screen : screens) {
    screen.second.selection=std::begin(screen.second.menu);
  }
};

umMenuItem_t MenuStructure::selectedMenuItem() {   
  return screens[currentScreen].selectedItem(); 
};   

// Structure of screens to initialize the menu
const umScreen_t SCREENS = { 
    { scHomeScreen,            {"Home" ,                        {  } } }, 
    { scMain,                  {"Main menu",                    { mnOverruleToday, mnOverruleTomorrow, mnOverruleMultipleDays, mnSettings, mnExitMenu} } },
    { scOverruleToday,         {"Overrule today",               { mnWorkFromHome, mnWorkAtOffice, mnWeekend, mnAllDayAway, mnAutomatic, mnBackToMainMenu } } },
    { scOverruleTomorrow,      {"Overrule tomorrow",            { mnWorkFromHome, mnWorkAtOffice, mnWeekend, mnAllDayAway, mnAutomatic, mnBackToMainMenu } } },
    { scOverruleMultiple,      {"Overrule multiple days",       { mnMoreDays, mnFewerDays, mnSetDaySchedule, mnResetToAutomatic, mnBackToMainMenu } } },
    { scOverruleMultiple2,     {"Overrule multiple - schedule", { mnWorkFromHome, mnWorkAtOffice, mnWeekend, mnAllDayAway, mnAutomatic, mnBackToPreviousMenu } } },
    { scSettings,              {"Settings",                     { mnSetTemperatures, mnSetSensorOffset, mnSetWeekSchedule, mnWorkFromHomeTimes, mnWorkFromOfficeTimes, mnSetWeekendTimes, mnBackToMainMenu } } },
    { scTemperatureSettings,   {"Temperature settings",         { mnHighTemperatureUp, mnHighTemperatureDown, mnLowTemperatureUp, mnLowTemperatureDown, mnBackToSettingsMenu } } },
    { scAdjustSensorOffset,    {"Adjust sensor offset",         { mnIncreaseOffset, mnReduceOffset, mnBackToSettingsMenu } } },
    { scSetWeekSchedule,       {"Set week schedule",            { mnMonday, mnTuesday, mnWednesday, mnThursday, mnFriday, mnSaturday, mnSunday, mnBackToSettingsMenu} } },
    { scSetDaySchedule,        {"Set day schedule",             { mnWorkFromHome, mnWorkAtOffice, mnWeekend, mnAllDayAway, mnBackToWeekSchedule } } },
    { scSetHomeTimes,          {"Set home times",               { mnWakeUpLater, mnWakeUpEarlier, mnGoToSleepLater, mnGoToSleepEarlier, mnBackToSettingsMenu } } },
    { scSetOfficeTimes,        {"Set office times",             { mnWakeUpLater, mnWakeUpEarlier, mnLeaveLater, mnLeaveEarlier, mnComeHomeLater, mnComeHomeEarlier, mnGoToSleepLater, mnGoToSleepEarlier, mnBackToSettingsMenu } } },
    { scSetWeekendTimes,       {"Set weekend times",            { mnWakeUpLater, mnWakeUpEarlier, mnGoToSleepLater, mnGoToSleepEarlier, mnBackToSettingsMenu } } }
  };

/************************************
 * All datatypes for the controller *
 ************************************/
string DAY_NAMES[] ={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };  

enum icon_t {iconNone, iconTempHigher, iconMenu, iconTempLower, iconHome, iconWork, iconWeekend, iconAway, 
  iconAutomatic, iconExit, iconFewer, iconMore, iconSettings, iconHighTemperature, iconLowTemperature, 
  iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays, iconMonday, iconTuesday, iconWednesday, 
  iconThursday, iconFriday, iconSaturday, iconSunday, iconShowerOn, iconShowerOff, iconFlame0, iconFlame1, 
  iconFlame2, iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8, iconWifiConnected, 
  iconWifiLost, iconSetpoint, iconSensorOffset, iconBoilerConnected, iconBoilerLost, iconBoilerTransmit
};

std::map<icon_t, string > ICONS = {
  {iconAutomatic, "/iconAutomatic.jpg"},
  {iconAway, "/iconAway.jpg"},
  {iconBoilerLost, "/iconBoilerLost.jpg"},
  {iconBoilerConnected, "/iconBoilerConn.jpg"},
  {iconBoilerTransmit, "/iconBoilerTransmit.jpg"},
  {iconClock, "/iconClock.jpg"},
  {iconExit, "/iconExit.jpg"},
  {iconFewer, "/iconFewer.jpg"},
  {iconFlame0, "/iconFlame0.jpg"},
  {iconFlame1, "/iconFlame1.jpg"},
  {iconFlame2, "/iconFlame2.jpg"},
  {iconFlame3, "/iconFlame3.jpg"},
  {iconFlame4, "/iconFlame4.jpg"},
  {iconFlame5, "/iconFlame5.jpg"},
  {iconFlame6, "/iconFlame6.jpg"},
  {iconFlame7, "/iconFlame7.jpg"},
  {iconFlame8, "/iconFlame8.jpg"},
  {iconFriday, "/iconFriday.jpg"},
  {iconHighTemperature, "/iconHighTemperature.jpg"},
  {iconHome, "/iconHome.jpg"},
  {iconLowTemperature, "/iconLowTemperature.jpg"},
  {iconMenu, "/iconMenu.jpg"},
  {iconMonday, "/iconMonday.jpg"},
  {iconMore, "/iconMore.jpg"},
  {iconMultipleDays, "/iconMultipleDays.jpg"},
  {iconSaturday, "/iconSaturday.jpg"},
  {iconSensorOffset, "/iconSensorOffset.jpg"},
  {iconSetpoint, "/iconSetpoint.jpg"},
  {iconSettings, "/iconSettings.jpg"},
  {iconShowerOff, "/iconShowerOff.jpg"},
  {iconShowerOn, "/iconShowerOn.jpg"},
  {iconSunday, "/iconSunday.jpg"},
  {iconTempHigher, "/iconTempHigher.jpg"},
  {iconTempLower, "/iconTempLower.jpg"},
  {iconThursday, "/iconThursday.jpg"},
  {iconToday, "/iconToday.jpg"},
  {iconTomorrow, "/iconTomorrow.jpg"},
  {iconTuesday, "/iconTuesday.jpg"},
  {iconWednesday, "/iconWednesday.jpg"},
  {iconWeekend, "/iconWeekend.jpg"},
  {iconWeekschedule, "/iconWeekschedule.jpg"},
  {iconWifiConnected, "/iconWifiConnected.jpg"},
  {iconWifiLost, "/iconWifiLost.jpg"},
  {iconWork, "/iconWork.jpg"}
};

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtWeekend, dtAway, dtAuto};

string DAY_TYPES[] = { "Work from home", "Work from office", "Weekend day", "All day away", "Automatic" };

const icon_t DAYTYPE_TO_ICON[] = { iconHome, iconWork, iconWeekend, iconAway, iconAutomatic };

enum setpointReason_t { spWeekSchedule,  spMultipleDays, spOverrideToday, spOverrideNow };

// Stores hour and minute of changes in the program
class timeValue_t {
  public:
    // Constructors
    timeValue_t() { minutesSinceMidnight = 0; };
    timeValue_t(int hour, int minute) { minutesSinceMidnight=60*hour+minute; };
    timeValue_t(const time_t& source) {
      struct tm * otherTime;
      otherTime = localtime(&source);
      minutesSinceMidnight=60*otherTime->tm_hour+otherTime->tm_min;
    };
    
    int minutesSinceMidnight;
    
    timeValue_t& operator+=(const timeValue_t& other){ minutesSinceMidnight+=other.minutesSinceMidnight; return *this; };
    timeValue_t& operator-=(const timeValue_t& other){ minutesSinceMidnight-=other.minutesSinceMidnight; return *this; };
  
    bool operator> (const time_t& other);
    bool operator> (const timeValue_t& other) const { return minutesSinceMidnight >  other.minutesSinceMidnight; }
    bool operator< (const timeValue_t& other) const { return minutesSinceMidnight <  other.minutesSinceMidnight; }
    bool operator>=(const timeValue_t& other) const { return minutesSinceMidnight >= other.minutesSinceMidnight; }
    bool operator<=(const timeValue_t& other) const { return minutesSinceMidnight <= other.minutesSinceMidnight; }

    string str() {
      char res[16];
      int hour   = minutesSinceMidnight / 60;
      int minute = minutesSinceMidnight % 60;
      snprintf(res, 16, "%d:%02d", hour, minute);
      return string(res);
    };
};

timeValue_t operator+(timeValue_t lhs, const timeValue_t & rhs) { lhs += rhs;  return lhs; };    
timeValue_t operator-(timeValue_t lhs, const timeValue_t & rhs) { lhs -= rhs;  return lhs; };    

/*********************************
 * All datatypes for the display *
 *********************************/
// Display parameter
template <class T> class DisplayParameter_t {
  public:
    void setValue(T newValue) { currentValue = newValue; }  
    T getValue()              { return currentValue; }
    bool isChanged()          { return ( (currentValue!=previousValue) or invalid ); }
    void drawn()              { previousValue = currentValue; invalid=false; }
    void invalidate()         { invalid=true; } // Useful when screen needs to be redrawn

  protected:
    T currentValue;
    T previousValue;
    bool invalid=false;
};

/**************************
 * All datatypes for WiFi *
 **************************/
// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
};
