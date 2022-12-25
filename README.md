# Roomthermostat

I wanted to create a room thermostat that can be controlled remotely ('honey, did you remember to turn down the heating?') but which also remains fully operational if internet connectivity is lost. Therefore it can be controlled through Telegram, but also via the hardware unit if WiFi is down. This is also useful since visitors may not want to use Telegram.

<p align="center">
  <img src="02 User interface\photo.png" alt="photo" width="500"/> 
</p>

This fully (software & mechanics & electronics) open source project is based on an ESP32 microcontroller and the OpenTherm protocol.

<p align="center">
    <img src="02 User interface/User manual/Onepager.png" alt="drawing" width="800"/> 
</p>

## Features

Every weekday can be programmed to be one of four kinds:
* **Work from home**: switch from low to high temperature at wake time, and back to low at sleep time
* **Work from office**: same, but also switch to low temperature when at the office
* **Weekend day**: same as home, but wake up and go to sleep at a different times
* **All day away**: low temperature throughout the day (e.g. when on holiday)

The temperature can be manually overruled to any temperature. The thermostat will automatically switch back to the programmed temperature as the next switch moment occurs.

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


The thermostat can be controlled by three touchbuttons on the device or via Telegram. In Telegram, an inline keyboard is used which changes based on the context. This way, the user experience is similar to controlling the device via the hardware. The menu status of each Telegram user is kept in memory, so all users can communicate with the thermostat independently.

## Controlling the thermostat through Telegram
### Main menu in Telegram

After selecting ```/start``` in Telegram, the following message and keyboard are returned:

<p align="center">
  <img src="02 User interface\Telegram main menu.png" alt="telegram main menu" width="800"/> 
</p>

The temperature in the room or settings of the thermostat can change over time. The display in Telegram is not automatically updated. The ```Status``` button cn be used to update the status.

The settings menu has the following options:

<p align="center">
  <img src="02 User interface\Telegram settings menu.png" alt="telegram main menu" width="800"/> 
</p>


### Simple use case in Telegram: going on a short holiday

The scenario is that we are going on a holiday for a few days and we will return on Monday.

To do this, select ```Overrule multiple days``` in the main menu:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 1.png" alt="events and messages" width="372"/> 
</p>

The icons in the next windows show that today and tomorrow are currently programmed as 'Work from office'-days, followed by two weekend days, and then some more 'work from office days'.

Select ```All day away``` since we will be going on a holiday:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 2.png" alt="events and messages" width="401"/> 
</p>

The ```All day away``` scenario means that the setpoint temperature will remain low throughout the day.

Then select ```More days``` button multiple times in the next menu until the message says there are 5 'All day away'-days, ending on Monday:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 3.png" alt="events and messages" width="401"/> 
</p>

Next select ```Back to the main menu```:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 4.png" alt="events and messages" width="401"/> 
</p>

Now, the current day is also selected as an ```All day away```, with a low temperature setting for the remaining part of the day. To resolve this select ```Overrule today```:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 5.png" alt="events and messages" width="429"/> 
</p>

Choose ```Weekend day``` so we can pack our bags comfortably:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 6.png" alt="events and messages" width="296"/> 
</p>

The remaining ```Multiple days``` will remain selected, only the current day will be overruled. It is also possible to overrule the next day while the remaining  ```multiple days``` remain overruled.

Finally, select ```Back to the main menu```:

<p align="center">
  <img src="02 User interface\Example - holiday\Step 7.png" alt="events and messages" width="296"/> 
</p>

Of course it is also possible to do these steps after we leave, without the need to overrule the current day. As we are returning home during the next Saturday, we can also overrule the current day to ensure the house is comfortable upon arrival.

### Switch off the thermostat for a longer period of time

To switch off the thermostat for a longer period of time, for instance during the summer or during a long holiday, proceed as follows:
* Select ```Overrule multiple days``` in the main menu
* Select ```All day away```
* Select ```Forever```
* Select ```Back to the main menu```

The thermostat is now switched off until ```Overrule multiple days``` is switched to ```Automatic```


## First use
It is required to make a specific Telegram bot account for the thermostat. There are many sources explaining how a new Telegram bot can be created, such as in this [Random Nerd Tutorials Telegram example](https://randomnerdtutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/).

One ChatID can be provided to which the roomthermostat sends a message upon startup. This is useful to be aware that the thermostat was reset, due to a software bug or power outage.

In order to use the software, the user must first:
* make a copy of `template.jsn` (located in the `..\RoomThermoStat\05 Software\Roomthermostat\data` folder)
* rename the copy `config.jsn` 
* fill in details of one or more WiFi routers, including the timezone in which each router is located (`lon` and `lat` are currently not used)
* upload the data to SPIFFS using the ESP32 sketch upload data tool
* copy `User_Setup.h` and `User_Setup_Select.h` to the `..\Arduino\libraries\TFT_eSPI` folder (perhaps good to backup the existing versions)

The format of the `config.json` file is as follows:

```JSON
{
    "AccessPoints": [
        { "SSID": "SSIDOfWifi1", "password": "PasswordOfWifi1", "timezone" : "Europe/Amsterdam" },
        { "SSID": "SSIDOfWifi2", "password": "PasswordOfWifi2", "timezone" : "Europe/Amsterdam" }
    ],
    "BotName"     : "Name of the bot",
    "BotUsername" : "Username of the bot",
    "BotToken"    : "##########:XXXXXXXXXXXXXXXXXXXXXXXX",
    "ChatID"      : "########"
}
```

Also, a number of libraries must be installed in Arduino:
* `TFT_eSPI` by Bodmer - https://github.com/Bodmer/TFT_eSPI
* `JPEGDecoder` by Bodmer - https://github.com/Bodmer/JPEGDecoder
* `ArduinoJSON` by Benoît Blanchon - https://arduinojson.org/
* `DallasTemperature` by Miles Burton - https://github.com/milesburton/Arduino-Temperature-Control-Library
* `OpenTherm` by Ihor Melnyk - https://github.com/ihormelnyk/opentherm_library

Big shout out to everyone for creating and maintaining these beautiful pieces of work!


## Electronic hardware 
The hardware consists of:
* an ESP32 (mh-et-live-minikit-d1)
* an OpenTherm adaptor designed by Ihor Melnyk (DIYless Master OpenTherm Shield - https://diyless.com/product/master-opentherm-shield)
* a 2.4" 320x240 TFT screen (ILI9341)
* a 3D printed housing designed in FreeCad
* up to 4 Dallas DS18B20 temperature sensors
* a 4k7 resistor
* 3 touch buttons (stainless steel screws, ISO7380-1, M5 x 16 + nut ISO4032 M5)
* 16 magnets (ø10 x 2.75 mm)
* a PCB designed in KiCad. This is optional, components can also be connected with wires, although it is less reliable

Except for the housing and the OpenTherm interface, everything was ordered at AliExpress. The total bill of materials was about €35. The bill of materials can be found here ```\04 Electrical\Bill of materials.ods```.


## Mechanical hardware

There are two options:
* Connecting electronics via the cross connect PCB - the casing then consists of two panels, see ```03 Mechanical\RoomThermostat.FCStd```
* Connecting electronics via wires - the casing then consists of three panels, see ```03 Mechanical\RoomThermostat with PCB.FCStd```

In case of the PCB, there are two main parts to 3D print, a **front panel** and a **rear panel**. The rear panel can be screwed to the wall. The front panel is clicked on the rear panel through magnets.

Both prints need to be paused in some layer during printing to add round magnets before recommencing the printing.

It is possible to add up to four Dallas temperature sensors on the PCB (but fewer sensors should also work).

There are central ventilation openings on the bottom and top of the device to cool the ESP32 and prevent thermal cross talk between the heat of the ESP32 and the temperature sensors. There are ventilation openings in front of each temperature sensor also to measure the room temperature as accurate as possible.

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

<p align="center">
<img src="01 System/Events and messages.png" alt="events and messages" width="800"/> 
</p>

<br><br>

* On the left hand side, the events that trigger activities are listed
* The circles represent code
* The arrows between the circles represent messages that are sent over the queues


### User presses key
* the ISR is running at about 30 Hz and is kept a small as possible. The capacitive sensors sometimes give a spike which only lasts one clock tick. Therefor, a key press is only considered valid if the key remains pressed for 2 subsequent samples. The ISR of the keyboard only sends a message to the menu, containing the key that was pressed. Subsequent actions are then carried out by the menu and other modules
* The capacitive sensor does not work well together with the SPI bus of the display, resulting in a crash of the microcontroller. Also, the keyboard ISR can interfere with the timing of the OpenThrem protocol. Therefore, polling the capacitive sensors is disabled during display or OpenTherm communication 
* the Backlight is controlled by the Display. In some cases, a button press does not result in a command to the controller (e.g., pressing the 'up'-button while the top menu item was already selected). In these cases, the backlight should also switch on. Therefore, the menu always sends a `cmdBacklightOn` command directly to the Backlight
* Telegram can take a few seconds to return. To improve responsivity of the keyboard, Telegram is temporarily disabled while the user is interacting with the keyboard. Telegram will be re-enabled as soon as the blacklight switches off.
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

## Over the air software updates

After uploading the code over the COM port the first time, over the air updates are possible. However, during normal operation, the keyboard handler will interfere with the update process.
Therefore, first use Telegram to select ```Settings``` > ```Receive over the air update...```. The keyboard handler, the boiler and Telegram will 
be disabled for five minutes during which time boiler will switch off. 
During these five minutes, th blue LED on the ESP32 will light up. During this time, choose the over the air port in Arduino (instead of the COM port) and upload the software over WiFi.

# To do

Potential improvements:
* improvement of responsiveness (keys and Telegram)
* switch off multiple days forever fro the main menu
* while waiting for over the air software update, display message on screen
* introduce permanent 'off' mode, for instance during the summer
* improve responsiveness
* send logfile as attachment to Telegram
* include icons in the menu
* allow user to modify water temperature of heater and shower
* save logdata through WiFi connection (Deta Base?)
* much code can be simplified to remove structures that were used in previous attempts to get the code working
* try to get FreeRTOS working again to improve performance of buttons and Telegram
* automatic updates to last message of known clients every 15 minutes
* implement OpenTherm protocoll in hardware timer
* optimize actual heater control functionality
    * check if D-action is implemented well in PID controller
    * take into account the weather in the control strategy
    * monitor the time it takes to heat the room in the morning, and compensate that by starting earlier
* also enable setting the time and date through hardware buttons if WiFi is unavailable
* display progress upon startup
* try if SPI frequency in TFT_eSPI can be higher

Code cleanup:
* find replace action GoOut > Leave
* cleanup Serial.print
* check use of const in function calls
* cleanup the use of messages. Some fields may no longer be needed

Hardware:
* add a capacitor to prevent brownout of the ESP32 at startup
