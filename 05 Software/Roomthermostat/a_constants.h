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

// One wire bus and temperature sensor
#define PIN_ONE_WIRE_BUS         17
#define TEMPERATURE_PRECISION    11   // Resolution 0.125°C. Requires 375 ms to fetch temperature

// Keyboard
#define PIN_BUT_UP               12   // Capacitive pin for up button
#define PIN_BUT_SELECT            4   // Capacitive pin for select button
#define PIN_BUT_DOWN             15   // Capacitive pin for down button

#define KEYBOARD_TIMER_ID         0   // Number of the keyboard timer (0..3)
#define KEY_THRESHOLD            25   // Level to trigger capacitive keyboard button

// Backlight
#define PIN_BACKLIGHT            32

#define BACKLIGHT_TIMEOUT       20*1000
#define BACKLIGHT_LED_CHANNEL         0
#define BACKLIGHT_FREQUENCY        5000
#define BACKLIGHT_RESOLUTION          8

// Pinout of the display is in the TFT_eSPI config file

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
  Height in pixels for each font:
  Font Noway_Medium24 height    24
  Font Noway_Light18 height     17
  Font Noway_Light24 height     23
  Font Noway_Medium60 height    58
  Font Noway_Light32 height     31
  Font Noway_Medium32 height    31
  Font Noway_Regular24 height   23
 */

#define SECS_PER_YEAR    (365.25 * 24 * 60 * 60)
#define SECS_PER_WEEK    (     7 * 24 * 60 * 60)
#define SECS_PER_DAY     (         24 * 60 * 60)
#define SECS_PER_HOUR    (              60 * 60)
#define SECS_PER_MINUTE  (                   60)

const char EMOTICON_ALARM_CLOCK[]         = {0xe2, 0x8f, 0xb0, 0x0};
const char EMOTICON_BATH[]                = {0xf0, 0x9f, 0x9b, 0x81, 0x0};
const char EMOTICON_BED[]                 = {0xf0, 0x9f, 0x9b, 0x8f, 0x0};
const char EMOTICON_BULLSEYE[]            = {0xf0, 0x9f, 0x8e, 0xaf, 0x0};
const char EMOTICON_CALENDAR[]            = {0xf0, 0x9f, 0x93, 0x85, 0x0};
const char EMOTICON_CAMERA[]              = {0xf0, 0x9f, 0x93, 0xb7, 0x0};
const char EMOTICON_CHECKERED_FLAG[]      = {0xf0, 0x9f, 0x8f, 0x81, 0x0};
const char EMOTICON_CHECKMARK[]           = {0xe2, 0x9c, 0x85, 0x0};
const char EMOTICON_CLOCK[]               = {0xe2, 0x8f, 0xb0, 0x0};
const char EMOTICON_CROSSMARK[]           = {0xe2, 0x9d, 0x8c, 0x0};
const char EMOTICON_DOWN_ARROW[]          = {0xe2, 0xac, 0x87, 0xef, 0xb8, 0x8f, 0x0};
const char EMOTICON_FLAME[]               = {0xf0, 0x9f, 0x94, 0xa5, 0x0};
const char EMOTICON_FOOTSTEPS[]           = {0xf0, 0x9f, 0x91, 0xa3, 0x0};
const char EMOTICON_GEAR[]                = {0xe2, 0x9a, 0x99, 0xef, 0xb8, 0x8f, 0x0};
// const char EMOTICON_GEAR[]                = ":gear:";
const char EMOTICON_GLASSES[]             = {0xf0, 0x9f, 0x91, 0x93, 0x0};
const char EMOTICON_HAMMER_WRENCH[]       = {0xf0, 0x9f, 0x9b, 0xa0, 0x0};
const char EMOTICON_HOURGLASS[]           = {0xe2, 0x8c, 0x9b, 0xef, 0xb8, 0x8f, 0x0};
const char EMOTICON_HOUSE[]               = {0xf0, 0x9f, 0x8f, 0xa0, 0x0};
const char EMOTICON_ISLAND[]              = {0xf0, 0x9f, 0x8f, 0x96, 0x0};
const char EMOTICON_LAMP[]                = {0xf0, 0x9f, 0x92, 0xa1, 0x0};
const char EMOTICON_MAGIC_STICK[]         = {0xf0, 0x9f, 0xaa, 0x84, 0x0};
const char EMOTICON_OFFICE[]              = {0xf0, 0x9f, 0x8f, 0xa2, 0x0};
const char EMOTICON_PIN[]                 = {0xf0, 0x9f, 0x93, 0x8d, 0x0};
// const char EMOTICON_PIN[]                 = ":dart:";
const char EMOTICON_POINTING_FINGER[]     = {0xf0, 0x9f, 0x91, 0x89, 0x0};
const char EMOTICON_RED_QUESTION_MARK[]   = {0xe2, 0x9d, 0x93, 0x0};
const char EMOTICON_RUNNER[]              = {0xf0, 0x9f, 0x8f, 0x83, 0x0};
const char EMOTICON_SHOWER[]              = {0xf0, 0x9f, 0x9a, 0xbf, 0x0};
const char EMOTICON_SMILEY[]              = {0xf0, 0x9f, 0x98, 0x80, 0x0};
const char EMOTICON_STHETOSCOPE[]         = {0xf0, 0x9f, 0xa9, 0xba, 0x0};
const char EMOTICON_STOPWATCH[]           = {0xe2, 0x8f, 0xb1, 0x0};
const char EMOTICON_THERMOMETER[]         = {0xf0, 0x9f, 0x8c, 0xa1, 0x0};
const char EMOTICON_TRIANGLE_DOWN[]       = {0xf0, 0x9f, 0x94, 0xbd, 0x0};
const char EMOTICON_TRIANGLE_UP[]         = {0xf0, 0x9f, 0x94, 0xbc, 0x0};
const char EMOTICON_UP_ARROW[]            = {0xe2, 0xac, 0x86, 0xef, 0xb8, 0x8f, 0x0};
const char EMOTICON_WARNING[]             = {0xe2, 0x9a, 0xa0, 0xef, 0xb8, 0x8f, 0x0};

// Configuration file, containing WiFi access point data
#define CONFIG_FILE "/config.jsn"
#define TEMP_CONFIG "/config.tmp"


enum boilerResponse_t { boilerNone, boilerSuccess, boilerInvalid, boilerTimeout };

enum sender_t { sndKeyboard, sndMenu, sndTelegram, sndController, sndTemperature, sndWifi, sndDisplay, sndBacklight, sndInvalid };

std::map<sender_t, string> senderLabels = {
  { sndKeyboard,    "Keyboard"    },
  { sndMenu,        "Menu"        },
  { sndTelegram,    "Telegram"    },
  { sndController,  "Controller"  },
  { sndTemperature, "Temperature" },
  { sndWifi,        "Wifi"        },
  { sndDisplay,     "Display"     },
  { sndBacklight,   "Backlight"   },
  { sndInvalid,     "Unknown"     } 
};

enum screen_t { scnHome, scnMain, scnOverruleToday, scnOverruleTomorrow, scnOverruleMultiple, 
  scnOverruleMultipleSchedule, scnSettingsMain, scnSettingsWeekSchedule, scnSettingsDaySchedule, 
  scnSettingsHomeTimes, scnSettingsOfficeTimes, scnSettingsWeekendTimes, scnSettingsTemperature, 
  scnSettingsSensorOffset, scnResetDevice, scnInvalid };

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
  { scnSettingsWeekendTimes,     "Set weekend times"            },
  { scnResetDevice,              "Reset device"                 },
  { scnInvalid,                  "Invalid"                      }
};

// API shared between menus and controller

enum command_t { 
  // Commands that affect the controller
  cmdControl, cmdSetpointHigher, cmdSetpointLower, cmdSetpointAuto, cmdOverruleTodayWorkFromHome, cmdOverruleTodayWorkAtOffice, cmdOverruleTodayWeekend, cmdOverruleTodayAway, 
  cmdOverruleTodayAutomatic, cmdOverruleTomorrowWorkFromHome, cmdOverruleTomorrowWorkAtOffice, cmdOverruleTomorrowWeekend, cmdOverruleTomorrowAway, cmdOverruleTomorrowAutomatic, 
  cmdOverruleMultipleMoreDays, cmdOverruleMultipleFewerDays, cmdOverruleMultipleWorkFromHome, cmdOverruleMultipleWorkAtOffice, cmdOverruleMultipleWeekend, cmdOverruleMultipleAway, 
  cmdOverruleMultipleAutomatic, cmdSetWeekSchedule, cmdHomeWakeUpEarlier, cmdHomeWakeUpLater, cmdHomeGoToSleepEarlier, cmdHomeGoToSleepLater, cmdOfficeWakeUpEarlier, 
  cmdOfficeWakeUpLater, cmdOfficeLeaveEarlier, cmdOfficeLeaveLater, cmdOfficeComeHomeEarlier, cmdOfficeComeHomeLater, cmdOfficeGoToSleepEarlier, cmdOfficeGoToSleepLater, 
  cmdWeekendWakeUpEarlier, cmdWeekendWakeUpLater, cmdWeekendGoToSleepEarlier, cmdWeekendGoToSleepLater, cmdHighTemperatureUp, cmdHighTemperatureDown, cmdLowTemperatureUp, 
  cmdLowTemperatureDown, cmdSensorOffsetUp, cmdSensorOffsetDown, cmdSetTemperatureOffset, cmdUpdateStatus, cmdUpdateScreen, cmdSilentUpdate,

  // Separator command
  cmdLastControllerCommand, 

  // Commands that do not affect the controller
  cmdStartTelegram, cmdMenuMain, cmdMenuOverruleToday, cmdMenuOverruleTomorrow, cmdMenuOverruleMultipleDays, cmdMenuSettings, cmdMenuHome, cmdMultipleDaySchedule, cmdMenuWeekSchedule, 
  cmdMenuHomeTimes, cmdMenuOfficeTimes, cmdMenuWeekendTimes, cmdMenuTemperature, cmdMenuSensorOffset, cmdReportBoiler, cmdReportTiming, cmdReportDebug, cmdReportLog, 
  cmdMonday, cmdTuesday, cmdWednesday, cmdThursday, cmdFriday, cmdSaturday, cmdSunday, cmdWorkFromHome, cmdWorkAtOffice, cmdWeekend, cmdAllDayAway, cmdCommandNotRecognized, 
  cmdBoilerSending, cmdBacklightOn, cmdKeyUp, cmdKeySelect, cmdKeyDown, cmdKeyPressed, cmdDisableTelegram, cmdEnableTelegram, 
  cmdResetDeviceMenu, cmdResetDeviceYes, cmdResetDeviceNo};

std::map<command_t, string> commandLabels = {
  { cmdMenuMain                     , "Main menu"},
  { cmdSetpointHigher               , "Setpoint higher"},
  { cmdSetpointLower                , "Setpoint lower"},
  { cmdSetpointAuto                 , "Setpoint automatic"},
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
  { cmdMenuWeekSchedule             , "Set weekschedule"},
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
  { cmdLastControllerCommand        , "Last controller command"},
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
  { cmdBacklightOn                  , "Turn backlight on"},
  { cmdKeyUp                        , "Key up"},
  { cmdKeySelect                    , "Key select"},
  { cmdKeyDown                      , "Key down" },
  { cmdUpdateScreen                 , "Update screen" },
  { cmdSilentUpdate                 , "Silent update" },
  { cmdKeyPressed                   , "Key pressed" },
  { cmdStartTelegram                , "Start Telegram" },
  { cmdDisableTelegram              , "Disable Telegram" },
  { cmdEnableTelegram               , "Enable Telegram" },
  { cmdResetDeviceMenu              , "Restart menu" },
  { cmdResetDeviceYes               , "Restart device" },
  { cmdResetDeviceNo                , "Return without restart" }
};

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
  { scnSettingsWeekSchedule,     { cmdMonday, cmdTuesday, cmdWednesday, cmdThursday, cmdFriday, cmdSaturday, cmdSunday, cmdMenuSettings } },
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

enum dayType_t {dtWorkFromHome, dtWorkAtOffice, dtWeekend, dtAway, dtAuto};
String DAY_TYPES[] = { "Work from home", "Work from office", "Weekend day", "All day away", "Automatic" };
  
const icon_t DAYTYPE_TO_ICON[]           = { iconHome,          iconWork,          iconWeekend,          iconAway,          iconAutomatic };
const icon_t DAYTYPE_OVERRULED_TO_ICON[] = { iconHomeOverruled, iconWorkOverruled, iconWeekendOverruled, iconAwayOverruled, iconAutomatic };
const char DAYTYPE_TO_EMOTICON[][5]      = { {0xf0, 0x9f, 0x8f, 0xa0, 0x0},   // House
                                             {0xf0, 0x9f, 0x8f, 0xa2, 0x0},   // Office
                                             {0xf0, 0x9f, 0x93, 0x85, 0x0},   // Calendar
                                             {0xf0, 0x9f, 0x8f, 0x96, 0x0},   // Island
                                             {0xf0, 0x9f, 0xaa, 0x84, 0x0} }; // Magic stick

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

    String str() {
      char res[16];
      int hour   = minutesSinceMidnight / 60;
      int minute = minutesSinceMidnight % 60;
      snprintf(res, 16, "%d:%02d", hour, minute);
      return String(res);
    };
};

timeValue_t operator+(timeValue_t lhs, const timeValue_t & rhs) { lhs += rhs;  return lhs; };    
timeValue_t operator-(timeValue_t lhs, const timeValue_t & rhs) { lhs -= rhs;  return lhs; };    

// Function to return the current local time as timeValue_t 
timeValue_t timeNow() {
  time_t now;
  struct tm * localTime;
  
  time(&now); // Get current time
  localTime = localtime(&now);
  return timeValue_t(localTime->tm_hour, localTime->tm_min);
}

String secondsToDuration(float timeInSecs) {
  String result = "";
  uint32_t n;

  if(timeInSecs<1) return "0 sec";
    
  if( timeInSecs > SECS_PER_YEAR )   { n = int( timeInSecs / SECS_PER_YEAR );   result += String(n)+" yr ";   timeInSecs-= n*SECS_PER_YEAR;   }
  if( timeInSecs > SECS_PER_WEEK )   { n = int( timeInSecs / SECS_PER_WEEK );   result += String(n)+" wk ";   timeInSecs-= n*SECS_PER_WEEK;   }
  if( timeInSecs > SECS_PER_DAY )    { n = int( timeInSecs / SECS_PER_DAY );    result += String(n)+" day ";  timeInSecs-= n*SECS_PER_DAY;    }
  if( timeInSecs > SECS_PER_HOUR )   { n = int( timeInSecs / SECS_PER_HOUR );   result += String(n)+" hr ";   timeInSecs-= n*SECS_PER_HOUR;   }
  if( timeInSecs > SECS_PER_MINUTE ) { n = int( timeInSecs / SECS_PER_MINUTE ); result += String(n)+" min ";  timeInSecs-= n*SECS_PER_MINUTE; }
  if( timeInSecs > 1 )               { n = int( timeInSecs );                   result += String(n)+" sec";                                   }

  return result;
};

/**************************
 * Datatypes for WiFi     *
 **************************/
// WiFi access point data
struct tAccessPoint {
  char ssid[64];
  char password[64];
  char timezone[64];
};
