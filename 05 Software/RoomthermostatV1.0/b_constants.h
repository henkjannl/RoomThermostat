#pragma once

#include <map>
#include <list>
#include <string>

using namespace std;

/*************************
 * PINOUT OF PERIPHERALS *
 *************************/
 
// OpenTherm Shield pins configuration
#define PIN_OPENTHERM_IN         22   // beware: in and out were flipped due to mistake creating a symbol in KiCad
#define PIN_OPENTHERM_OUT        21

#define PIN_BACKLIGHT            32

// One wire bus and temperature sensor
#define PIN_ONE_WIRE_BUS         17
#define TEMPERATURE_PRECISION    11   // Resolution 0.125°C. Requires 375 ms to fetch temperature

// Pinout of the display is in the TFT_eSPI config file

// Keyboard
#define PIN_BUT_UP               12   // Capacitive pin for up button
#define PIN_BUT_SELECT            4   // Capacitive pin for select button
#define PIN_BUT_DOWN             15   // Capacitive pin for down button

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

enum boilerResponse_t { boilerNone, boilerSuccess, boilerInvalid, boilerTimeout };

enum sender_t { sndKeyboard, sndBacklight, sndMenu, sndTelegram, sndController, sndDisplay, sndThermometer, sndBoiler, sndWifi };

std::map<sender_t, string> senderLabels = {
  { sndKeyboard,    "Keyboard"    },
  { sndBacklight,   "Backlight"   },
  { sndMenu,        "Menu"        },
  { sndTelegram,    "Telegram"    },
  { sndController,  "Controller"  },
  { sndDisplay,     "Display"     },
  { sndThermometer, "Thermometer" },
  { sndBoiler,      "Boiler"      },
  { sndWifi,        "Wifi"        } };

enum screen_t { scnHome, scnMain, scnOverruleToday, scnOverruleTomorrow, scnOverruleMultiple, 
  scnOverruleMultipleSchedule, scnSettingsMain, scnSettingsWeekSchedule, scnSettingsDaySchedule, 
  scnSettingsHomeTimes, scnSettingsOfficeTimes, scnSettingsWeekendTimes, scnSettingsTemperature, 
  scnSettingsSensorOffset };

// API shared between menus and controller

enum command_t { cmdMenuMain, cmdSetpointHigher, cmdSetpointLower, cmdSetpointAuto, cmdGoAway, cmdUpdateStatus, 
  cmdMenuOverruleToday, cmdMenuOverruleTomorrow, cmdMenuOverruleMultipleDays, cmdMenuSettings, cmdMenuHome, 
  cmdOverruleTodayWorkFromHome, cmdOverruleTodayWorkAtOffice, cmdOverruleTodayWeekend, cmdOverruleTodayAway, 
  cmdOverruleTodayAutomatic, cmdOverruleTomorrowWorkFromHome, cmdOverruleTomorrowWorkAtOffice, 
  cmdOverruleTomorrowWeekend, cmdOverruleTomorrowAway, cmdOverruleTomorrowAutomatic, cmdOverruleMultipleMoreDays, 
  cmdOverruleMultipleFewerDays, cmdMultipleDaySchedule, cmdOverruleMultipleWorkFromHome, 
  cmdOverruleMultipleWorkAtOffice, cmdOverruleMultipleWeekend, cmdOverruleMultipleAway, 
  cmdOverruleMultipleAutomatic, cmdMenuWeekSchedule, cmdMenuHomeTimes, cmdMenuOfficeTimes, cmdMenuWeekendTimes, 
  cmdMenuTemperature, cmdMenuSensorOffset, cmdReportBoiler, cmdReportTiming, cmdReportDebug, cmdMonday, cmdTuesday, cmdWednesday, 
  cmdThursday, cmdFriday, cmdSaturday, cmdSunday, cmdSetWeekSchedule, cmdWorkFromHome, cmdWorkAtOffice, cmdWeekend, 
  cmdAllDayAway, cmdHomeWakeUpEarlier, cmdHomeWakeUpLater, cmdHomeGoToSleepEarlier, cmdHomeGoToSleepLater, 
  cmdOfficeWakeUpEarlier, cmdOfficeWakeUpLater, cmdOfficeLeaveEarlier, cmdOfficeLeaveLater, 
  cmdOfficeComeHomeEarlier, cmdOfficeComeHomeLater, cmdOfficeGoToSleepEarlier, cmdOfficeGoToSleepLater, 
  cmdWeekendWakeUpEarlier, cmdWeekendWakeUpLater, cmdWeekendGoToSleepEarlier, cmdWeekendGoToSleepLater, 
  cmdHighTemperatureUp, cmdHighTemperatureDown, cmdLowTemperatureUp, cmdLowTemperatureDown, cmdSensorOffsetUp, 
  cmdSensorOffsetDown, cmdSetTemperatureOffset, cmdControl, cmdCommandNotRecognized, 
  cmdBoilerSending, cmdBoilerNormal, cmdBacklightOn, cmdKeyUp, cmdKeySelect, cmdKeyDown, cmdUpdateScreen };

std::map<screen_t, string> screenTitle = { 
  { scnHome,                     "Home"                         }, 
  { scnMain,                     "Main menu"                    },
  { scnOverruleToday,            "Overrule today"               },
  { scnOverruleTomorrow,         "Overrule tomorrow"            },
  { scnOverruleMultiple,         "Overrule multiple days"       },
  { scnOverruleMultipleSchedule, "Overrule multiple - schedule" },
  { scnSettingsMain,             "Settings"                     },
  { scnSettingsTemperature,      "Temperature settings"         },
  { scnSettingsSensorOffset,     "Adjust sensor offset"         },
  { scnSettingsWeekSchedule,     "Set week schedule"            },
  { scnSettingsDaySchedule,      "Set day schedule"             },
  { scnSettingsHomeTimes,        "Set home times"               },
  { scnSettingsOfficeTimes,      "Set office times"             },
  { scnSettingsWeekendTimes,     "Set weekend times"            }
};

std::map<command_t, string> commandLabels = {
  { cmdMenuMain                     , "Main menu"},
  { cmdSetpointHigher               , "Setpoint higher"},
  { cmdSetpointLower                , "Setpoint lower"},
  { cmdSetpointAuto                 , "Setpoint automatic"},
  { cmdGoAway                       , "Go out"},
  { cmdUpdateStatus                 , "Update"},
  { cmdMenuOverruleToday            , "Overrule today"},
  { cmdMenuOverruleTomorrow         , "Overrule tomorrow"},
  { cmdMenuOverruleMultipleDays     , "Overrule multiple days"},
  { cmdMenuSettings                 , "Settings"},
  { cmdMenuHome                     , "Home menu"},
  { cmdOverruleTodayWorkFromHome    , "Work from home"},
  { cmdOverruleTodayWorkAtOffice    , "Work at office"},
  { cmdOverruleTodayWeekend         , "Weekend"},
  { cmdOverruleTodayAway            , "All day away"},
  { cmdOverruleTodayAutomatic       , "Automatic"},
  { cmdOverruleTomorrowWorkFromHome , "Work from home"},
  { cmdOverruleTomorrowWorkAtOffice , "Work from office"},
  { cmdOverruleTomorrowWeekend      , "Weekend"},
  { cmdOverruleTomorrowAway         , "All day away"},
  { cmdOverruleTomorrowAutomatic    , "Automatic"},
  { cmdOverruleMultipleMoreDays     , "More days"},
  { cmdOverruleMultipleFewerDays    , "Fewer days"},
  { cmdMultipleDaySchedule          , "Set day schedule"},
  { cmdOverruleMultipleAutomatic    , "Automatic"},
  { cmdOverruleMultipleWorkFromHome , "Work from home"},
  { cmdOverruleMultipleWorkAtOffice , "Work from office"},
  { cmdOverruleMultipleWeekend      , "Weekend"},
  { cmdOverruleMultipleAway         , "All day away"},
  { cmdMenuWeekSchedule             , "Set weekly schedule"},
  { cmdMenuHomeTimes                , "Set home times"},
  { cmdMenuOfficeTimes              , "Set office times"},
  { cmdMenuWeekendTimes             , "Set weekend times"},
  { cmdMenuTemperature              , "Set temperatures"},
  { cmdMenuSensorOffset             , "Adjust sensor offset"},
  { cmdReportBoiler                 , "Report boiler"},
  { cmdReportTiming                 , "Report timing"},
  { cmdReportDebug                  , "Report debug"},
  { cmdMonday                       , "Monday"},
  { cmdTuesday                      , "Tuesday"},
  { cmdWednesday                    , "Wednesday"},
  { cmdThursday                     , "Thursday"},
  { cmdFriday                       , "Friday"},
  { cmdSaturday                     , "Saturday"},
  { cmdSunday                       , "Sunday"},
  { cmdWorkFromHome                 , "Work from home"},
  { cmdWorkAtOffice                 , "Work from office"},
  { cmdWeekend                      , "Weekend day"},
  { cmdAllDayAway                   , "All day away"},
  { cmdHomeWakeUpLater              , "Wake up later"},
  { cmdHomeWakeUpEarlier            , "Wake up earlier"},
  { cmdHomeGoToSleepLater           , "Go to sleep later"},
  { cmdHomeGoToSleepEarlier         , "Go to sleep earlier"},
  { cmdOfficeWakeUpLater            , "Wake up later"},
  { cmdOfficeWakeUpEarlier          , "Wake up earlier"},
  { cmdOfficeLeaveLater             , "Leave later"},
  { cmdOfficeLeaveEarlier           , "Leave earlier"},
  { cmdOfficeComeHomeLater          , "Come in later"},
  { cmdOfficeComeHomeEarlier        , "Come in earlier"},
  { cmdOfficeGoToSleepLater         , "Go to sleep later"},
  { cmdOfficeGoToSleepEarlier       , "Go to sleep earlier"},
  { cmdWeekendWakeUpLater           , "Wake up later"},
  { cmdWeekendWakeUpEarlier         , "Wake up earlier"},
  { cmdWeekendGoToSleepLater        , "Go to sleep later"},
  { cmdWeekendGoToSleepEarlier      , "Go to sleep earlier"},
  { cmdHighTemperatureUp            , "High temp up"},
  { cmdHighTemperatureDown          , "High temp down"},
  { cmdLowTemperatureUp             , "Low temp up"},
  { cmdLowTemperatureDown           , "Low temp down"},
  { cmdSensorOffsetUp               , "Offset up"},
  { cmdSensorOffsetDown             , "Offset down"},
  { cmdSetWeekSchedule              , "Set week schedule"},
  { cmdSetTemperatureOffset         , "Set temperatureoffset"},
  { cmdControl                      , "Control temperature"},
  { cmdCommandNotRecognized         , "Command not recognized"},
  { cmdBoilerSending                , "Boiler sending icon"},
  { cmdBoilerNormal                 , "Boiler normal icon"},
  { cmdBacklightOn                  , "Turn backlight on"},
  { cmdKeyUp                        , "Key up"},
  { cmdKeySelect                    , "Key select"},
  { cmdKeyDown                      , "Key down"},
  { cmdUpdateScreen                 , "Update screen"}
};

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtWeekend, dtAway, dtAuto};

// Structure of screens to initialize the menu
typedef std::map<screen_t, std::list<command_t> > screenInitializer_t;

screenInitializer_t SCREENS = { 
  { scnHome,                     { cmdSetpointHigher, cmdMenuMain, cmdSetpointLower } }, 
  { scnMain,                     { cmdMenuOverruleToday, cmdMenuOverruleTomorrow, cmdMenuOverruleMultipleDays, cmdMenuSettings, cmdMenuHome} },
  { scnOverruleToday,            { cmdOverruleTodayWorkFromHome, cmdOverruleTodayWorkAtOffice, cmdOverruleTodayWeekend, cmdOverruleTodayAway, cmdOverruleTodayAutomatic, cmdMenuMain } },
  { scnOverruleTomorrow,         { cmdOverruleTomorrowWorkFromHome, cmdOverruleTomorrowWorkAtOffice, cmdOverruleTomorrowWeekend, cmdOverruleTomorrowAway, cmdOverruleTomorrowAutomatic, cmdMenuMain } },
  { scnOverruleMultiple,         { cmdOverruleMultipleMoreDays, cmdOverruleMultipleFewerDays, cmdMultipleDaySchedule, cmdOverruleMultipleAutomatic, cmdMenuMain } },
  { scnOverruleMultipleSchedule, { cmdWorkFromHome, cmdWorkAtOffice, cmdWeekend, cmdOverruleTodayAway, cmdOverruleTodayAutomatic, cmdMenuOverruleMultipleDays } },
  { scnSettingsMain,             { cmdMenuTemperature, cmdMenuSensorOffset, cmdMenuWeekSchedule, cmdMenuHomeTimes, cmdMenuOfficeTimes, cmdMenuWeekendTimes, cmdMenuMain } },
  { scnSettingsTemperature,      { cmdHighTemperatureUp, cmdHighTemperatureDown, cmdLowTemperatureUp, cmdLowTemperatureDown, cmdMenuSettings } },
  { scnSettingsSensorOffset,     { cmdSensorOffsetUp, cmdSensorOffsetDown, cmdMenuSettings } },
  { scnSettingsWeekSchedule,     { cmdMonday, cmdTuesday, cmdWednesday, cmdThursday, cmdFriday, cmdSaturday, cmdSunday, cmdMenuSettings} },
  { scnSettingsDaySchedule,      { cmdWorkFromHome, cmdWorkAtOffice, cmdWeekend, cmdOverruleTodayAway, cmdMenuWeekSchedule } },
  { scnSettingsHomeTimes,        { cmdHomeWakeUpLater, cmdHomeWakeUpEarlier, cmdHomeGoToSleepLater, cmdHomeGoToSleepEarlier, cmdMenuSettings } },
  { scnSettingsOfficeTimes,      { cmdOfficeWakeUpLater, cmdOfficeWakeUpEarlier, cmdOfficeLeaveLater, cmdOfficeLeaveEarlier, cmdOfficeComeHomeLater, cmdOfficeComeHomeEarlier, cmdHomeGoToSleepLater, cmdHomeGoToSleepEarlier, cmdMenuSettings } },
  { scnSettingsWeekendTimes,     { cmdWeekendWakeUpLater, cmdWeekendWakeUpEarlier, cmdWeekendGoToSleepLater, cmdWeekendGoToSleepEarlier, cmdMenuSettings } } 
};

/************************************
 * All datatypes for the controller *
 ************************************/
string DAY_NAMES[] ={ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };  

enum icon_t {iconNone, iconTempHigher, iconMenu, iconTempLower, iconHome, iconWork, iconWeekend, iconAway, 
  iconHomeOverruled, iconWorkOverruled, iconWeekendOverruled, iconAwayOverruled, 
  iconAutomatic, iconExit, iconFewer, iconMore, iconSettings, iconHighTemperature, iconLowTemperature, 
  iconClock, iconWeekschedule, iconToday, iconTomorrow, iconMultipleDays, iconMonday, iconTuesday, iconWednesday, 
  iconThursday, iconFriday, iconSaturday, iconSunday, iconShowerOn, iconShowerOff, iconFlame0, iconFlame1, 
  iconFlame2, iconFlame3, iconFlame4, iconFlame5, iconFlame6, iconFlame7, iconFlame8, iconWifiConnected, 
  iconWifiLost, iconSetpoint, iconSensorOffset, iconBoilerConnected, iconBoilerLost, iconBoilerTransmit
};

std::map<icon_t, string > ICONS = {
  {iconHome,               "/iconHome.jpg"           },
  {iconWork,               "/iconWork.jpg"           },
  {iconWeekend,            "/iconWeekend.jpg"        },
  {iconAway,               "/iconAway.jpg"           },

  {iconHomeOverruled,      "/iconHomeOv.jpg"         },
  {iconWorkOverruled,      "/iconWorkOv.jpg"         },
  {iconWeekendOverruled,   "/iconWeekendOv.jpg"      },
  {iconAwayOverruled,      "/iconAwayOv.jpg"         },

  {iconAutomatic,          "/iconAutomatic.jpg"      },

  {iconWifiConnected,      "/iconWifiConnected.jpg"  },
  {iconWifiLost,           "/iconWifiLost.jpg"       },

  {iconBoilerLost,         "/iconBoilerLost.jpg"     },
  {iconBoilerConnected,    "/iconBoilerConn.jpg"     },
  {iconBoilerTransmit,     "/iconBoilerTransmit.jpg" },

  {iconShowerOff,          "/iconShowerOff.jpg"      },
  {iconShowerOn,           "/iconShowerOn.jpg"       },

  {iconFlame0,             "/iconFlame0.jpg"         },
  {iconFlame1,             "/iconFlame1.jpg"         },
  {iconFlame2,             "/iconFlame2.jpg"         },
  {iconFlame3,             "/iconFlame3.jpg"         },
  {iconFlame4,             "/iconFlame4.jpg"         },
  {iconFlame5,             "/iconFlame5.jpg"         },
  {iconFlame6,             "/iconFlame6.jpg"         },
  {iconFlame7,             "/iconFlame7.jpg"         },
  {iconFlame8,             "/iconFlame8.jpg"         },

  {iconToday,              "/iconToday.jpg"          },
  {iconTomorrow,           "/iconTomorrow.jpg"       },

  {iconMonday,             "/iconMonday.jpg"         },
  {iconTuesday,            "/iconTuesday.jpg"        },
  {iconWednesday,          "/iconWednesday.jpg"      },
  {iconThursday,           "/iconThursday.jpg"       },
  {iconFriday,             "/iconFriday.jpg"         },
  {iconSaturday,           "/iconSaturday.jpg"       },
  {iconSunday,             "/iconSunday.jpg"         },

  {iconMenu,               "/iconMenu.jpg"           },
  {iconClock,              "/iconClock.jpg"          },
  {iconHighTemperature,    "/iconHighTemperature.jpg"},
  {iconLowTemperature,     "/iconLowTemperature.jpg" },
  {iconFewer,              "/iconFewer.jpg"          },
  {iconMore,               "/iconMore.jpg"           },
  {iconMultipleDays,       "/iconMultipleDays.jpg"   },
  {iconSensorOffset,       "/iconSensorOffset.jpg"   },
  {iconSetpoint,           "/iconSetpoint.jpg"       },
  {iconSettings,           "/iconSettings.jpg"       },
  {iconTempHigher,         "/iconTempHigher.jpg"     },
  {iconTempLower,          "/iconTempLower.jpg"      },
  {iconWeekschedule,       "/iconWeekschedule.jpg"   },
  {iconExit,               "/iconExit.jpg"           }
};

string DAY_TYPES[] = { "Work from home", "Work from office", "Weekend day", "All day away", "Automatic" };

const icon_t DAYTYPE_TO_ICON[]           = { iconHome,          iconWork,          iconWeekend,          iconAway,          iconAutomatic };
const icon_t DAYTYPE_OVERRULED_TO_ICON[] = { iconHomeOverruled, iconWorkOverruled, iconWeekendOverruled, iconAwayOverruled, iconAutomatic };

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

/**************************
 * Datatypes for WiFi     *
 **************************/
// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
};
