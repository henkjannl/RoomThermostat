# Remote thermostat
I wanted to create a room thermostat that can be controlled remotely ('honey, did you remember to turn down the heating?') but which also remains fully operational if internet connectivity is lost. Therefore it can be controlled through Telegram, but also via the hardware unit if WiFi or internet are down.

This is a fully (software & mechanics & electronics) open source room thermostat, based on an ESP32 microcontroller and the OpenTherm protocol.

<img src="02 User interface/User manual/Onepager.png" alt="drawing" width="800"/> 


Every weekday can be programmed to be one of four kinds:
* **Work from home**: switch from low to high temperature at wake time, and back to low at sleep time
* **Work from office**: same, but also switch to low temperature when at the office
* **Weekend day**: same as home, but wake up and go to sleep at a different times
* **All day away**: low temperature throughout the day (e.g. when on holiday)


Only two temperatures are defined common to all day kinds:
* a high temperature for when awake and present
* a low temperature for when away or asleep


All times for each day type (8 times in total) can be modified.


There is flexibility on top of that:
* **Today** and **tomorrow** can be overruled by a schedule that is different from the normal schedule
* It is also possible to **overrule multiple days**, for instance to program multiple days as weekend day (e.g. holiday at home) or as days away (e.g. holiday away)
* After **overruling multiple days** ahead, **today** and **tomorrow** can be overruled again to choose a different kind of day for today or tomorrow. For instance: if the holiday starts tomorrow, **multiple days** can be overruled to be **away**, after which **today** can be overruled as **weekend day**
* Icons of days according to the normal week schedule are displayed in black, overruled days are displayed in blue
* At all times, a **higher or lower temperature** can be manually selected. When the next programmed timeslot arrives, the temperature switches back to the program.


The thermostat can be controlled by three touchbuttons on the device or via Telegram. In Telegram, an inline keyboard is used which changes based on the context. This way, the user experience is similar to controlling the device via the hardware. A list of all Telegram users is kept and each user can be in a different menu without receiving updates from other users.

The Telegram main menu looks like this:

<img src="02 User interface\Telegram menu.png" alt="events and messages" width="400"/> 


## Electronic hardware 
The hardware consists of:
* an ESP32 (mh-et-live-minikit-d1)
* an OpenTherm adaptor designed by Ihor Melnyk (DIYless Master OpenTherm Shield - https://diyless.com/product/master-opentherm-shield)
* a 2.4" 320x240 TFT screen (ILI9341)
* a 3D printed housing designed in FreeCad
* a PCB designed in KiCad
* up to 4 Dallas DS18B20 temperature sensors
* a 4k7 resistor
* 3 touch buttons (stainless steel screws, ISO7380-1, M5 x 16 + nut ISO4032 M5)
* 8 magnets (ø10 x 2.75 mm)
I think the total bill of materials is about €35


## First use
In order to use the software, the user must first:
* make a copy of `template.jsn` (located in the `..\RoomThermoStat\05 Software\Roomthermostat\data` folder)
* rename the copy `config.jsn` 
* fill in details of one or more WiFi routers, including the timezone in which each router is located (`lon` and `lat` are currently not used)
* upload the data to SPIFFS using the ESP32 sketch upload data tool
* copy `User_Setup.h` and `User_Setup_Select.h` to the `..\Arduino\libraries\TFT_eSPI` folder (perhaps good to backup the existing versions)


Also, a number of libraries must be installed in Arduino:
* `TFT_eSPI` by Bodmer - https://github.com/Bodmer/TFT_eSPI
* `JPEGDecoder` by Bodmer - https://github.com/Bodmer/JPEGDecoder
* `ArduinoJSON` by Benoît Blanchon - https://arduinojson.org/
* `DallasTemperature` by Miles Burton - https://github.com/milesburton/Arduino-Temperature-Control-Library
* `OpenTherm` by Ihor Melnyk - https://github.com/ihormelnyk/opentherm_library

Big shout out to everyone for creating and maintaining these beautiful pieces of work!


## Mechanical hardware

There are two main parts to 3D print, a **front panel** and a **rear panel**. The rear panel can be screwed to the wall. The front panel is clicked on the rear panel through magnets.

Both panels need to be stopped in some layer during printing to add round magnets before recommencing the printing. In total, 2x4x2 = 16 magnets are needed, dimensions ø10 x 2.75 mm (available on AliExpress)

It is possible to add up to four Dallas temperature sensors on the PCB (but two also works).

There are central ventilation openings on the bottom and top of the device to cool the ESP32 and prevent cross talk between the heat of the ESP32 and the temperature sensors. There are ventilation openings in front of each temperature sensors also to measure the room temperature as accurate as possible.

The sofware has an **adjust sensor offset** feature to add or subtract a value from the measured temperature for calibration. That means that if the temperature measured by the thermostate is different from the actual room temperature, this can be corrected through the hardware buttons or via Telegram. 


## Software

### Overview of processes
| File               | Module                                                       |
|--------------------|--------------------------------------------------------------|
| Roomthermostat.ino | Initialization and main loop                                 |
| a_constants.h      | Defines, constants, data types                               |
| b_interfaces.h     | Queues and structs that connect the subsystems               |
| c_logger.h         | Datalogging (to be implemented)                              |
| d_keyboard.h       | Keyboard                                                     |
| e_menu.h           | Menu of hardware unit                                        |
| f_telegram.h       | Handler for Telegram messages                                |
| g_controller.h     | Temperature controller, including PID controller and program |
| h_temperature.h    | Temperature measurement and communication with boiler        |
| i_wifi_time.h      | Wifi controller and real time clock                          |
| j_jpeg.h           | Library to display jpeg images                               |
| k_display.h        | Display of the hardware unit                                 |
| l_backlight.h      | Backlight                                                    |

<br><br>


## Flow of events and messages

Initially, FreeRTOS was envisaged to handle all different processes. However, I did not (yet) get this working together with the OpenTherm library, which is very time critical. It is only possible to debug this with the live boiler attached,  which is a little barrier. Now, all processes are handled sequentially by the main loop.

It also appears that the SPI bus that communicates with the display does not work well while the capacitive touch button routine is called. Every time the display updates, the `touchRead()` is temporarily disabled.

The touch buttons are also disabled while OpenTherm communicates with the boiler.

<br>
The flow of events and messages is as follows:
<br>

<img src="01 System/Events and messages.png" alt="events and messages" width="800"/> 

<br><br>

* On the left hand side, the events that trigger activities are listed
* The circles represent code
* The arrows between the circles represent messages that are sent over the queues


### User presses key
* the ISR is running at about 30 Hz and is kept a small as possible. Most actions are carried out by the Menu. The ISR of the keyboard only sends a message to the menu, containing the key that was pressed 
* the Backlight is controlled by the Display. In some cases, a button press does not result in a command to the controller (e.g., pressing the 'up'-button while the top menu item was already selected). In these cases, the backlight should also switch on. Therefore, the menu always sends a `cmdBacklightOn` command directly to the Backlight
* Telegram can take a few seconds to return. To improve responsivity of the keyboard, Telegram is temporarily disabled while the user is interacting with the keyboard. Telegram will be re-enabled as son as the blacklight switches off
* the first key press is only used to switch on the backlight. If the backlight is off while a key is pressed, the key is ignored and the backlight is woken up


### User enters a command in Telegram
* the status of the Telegram menu is stored independently for each individual ChatID, so each user can interact with Telegram independently of the other Telegram users and of the hardware unit
* some commands change the status of the controller (e.g., change a wake up time may cause a different setpoint for the boiler), others do not (e.g., moving to another screen in Telegram)
* only commands that change the status of the controller are sent to the controller, the other commands are directly forwarded to the Telegram queue that handles sending a response to the chat
* if the controller receives the command, it causes an update of the display which in turn switches on the backlight. This way, people in the room may notice that the setpoint was potentially changed and may act accordingly
* after the controller completes the action, the command is forwarded to the Telegram queue that returns the response to the user

### The timer for temperature measurement expires
* after the temperature is measured, the controller gets the chance to change the boiler setpoint, and a display update is triggered. This is done via a  `cmdSilentUpdate` command, which does not trigger the backlight: the change is only visible to the user if the backlight was already switched on


### The timer for the boiler expires
* as the timer expires, the controller is called to update the boiler setpoint
* the controller then forwards the `cmdBoilerSending` command to the screen which changes the boiler icon
* then, the screen again forwards the command to the temperature queue which triggers actual communication with the boiler
* finally, a `cmdSilentUpdate` command restores the boiler icon to the normal state


### Check WiFi and time timer expires
* this event (re-)connects to WiFi if needed, syncs with the timeserver if needed and updates the screen to reflect the change of the icon, without switching on the backlight

### Backlight timer expires
* the backlight is switched off
* the menu of the hardware unit is flipped back to the home screen
* Telegram is re-enabled

## Overview of directories:
* GUI and icons were created in Inkscape and are located in `..\RoomThermoStat\02 User interface\GUI design.svg`
* All types and structures of the menu structure are managed in `..\RoomThermoStat\02 User interface\Menu structure.xlsx`
* GUI colors are chosen in Inkscape converted to RGB565 in  `..\RoomThermoStat\02 User interface\Color converter RGB565.xlsx`
* Mechanical design is done in FreeCAD and is located in `..\RoomThermoStat\03 Mechanical\_RoomThermostat.FCStd`
* The interconnect PCB is designed in KiCAD and is located in  `..\RoomThermoStat\04 Electrical\Interconnect PCB V1.0 - as ordered JLC PCB`
* Arduino programs to test individual pieces of hardware are located in `..\RoomThermoStat\05 Software\Tests`
* Current version of the Arduino software is located in `..\RoomThermoStat\05 Software\Roomthermostat`

## To do

Potential improvements:
* re-introduce FreeRTOS
* introduce permanent 'off' mode, for instance during the summer
* much code can be simplified to remove structures that were used in previous attempts to get the code working
* hardware: add a capacitor to prevent brownout of the ESP32 at startup
* save logdata through WiFi connection (Deta?)
* try to get FreeRTOS working again to improve performance of buttons and Telegram
* implement OTA software updates
* optimize actual heater control functionality
    * take into account the weather in the control strategy
    * monitor the time it takes to heat the room in the morning, and compensate that by starting earlier
* periodically save settings to SPIFFS
* also enable setting the time and date through hardware buttons if WiFi is unavailable
* include icons in the menu
