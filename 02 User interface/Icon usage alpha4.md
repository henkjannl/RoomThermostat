# Icon usage in the current thermostat and remote

Source audit of alpha.4, 24 October 2026. “Used” means displayed during supported normal operation. Mere presence in ICONS, an enum, LittleFS or display_assets.h does not count. This is not hardware coverage.

64 icon assets; thermostat references 19, remote references 45. Filenames below omit extensions.

iconAutomatic is REMOTE ONLY for normal operation. The thermostat has an Auto lookup-table slot, but the valid day strip is populated from four concrete day types; selecting Auto cancels an override. Unsupported externally edited NVS could expose that table slot. This corrects the earlier reference-based count of 20 to 19 normally displayed thermostat icons.

The physical thermostat menus render text, not the menu icon artwork. Its red thermometer/target are part of HomeScreen.jpg. The remote also crops those two red home-screen graphics from HomeScreen.jpg; its separate iconTemperature/iconSetpoint files are used in settings menus.

HomeScreen.jpg is used by both devices; HomeScreenMask.jpg is unused by both. They are backgrounds and excluded from the 64-icon counts. The remote also draws question marks, infinity, OK, selection underlines and override markers in code; they are not separate icon files.

References checked: thermostat/src/a_constants.h (ICONS and DAYTYPE_TO_ICON), k_display.h (actual bitmap draws and text menus), h_temperature.h (boiler/shower/flame selection), m_network.h and c_interfaces.h (status initialization), remote-cyd/include/remote_menu.h (all pages and battery formatting).

| Icon | Thermostat | Remote |
|---|---|---|
| battery-0 | Unused | Used |
| battery-100 | Unused | Used |
| battery-25 | Unused | Used |
| battery-50 | Unused | Used |
| battery-75 | Unused | Used |
| iconAutomatic | Table slot only; not normally displayed | Used |
| iconAway | Used | Used |
| iconAwayOv | Unused | Unused |
| iconBoilerConn | Used | Used |
| iconBoilerLost | Used | Used |
| iconBoilerTransmit | Unused | Unused |
| iconClock | Unused | Used |
| iconExit | Unused | Used |
| iconFewer | Unused | Unused |
| iconFlame0 | Used | Used |
| iconFlame1 | Used | Used |
| iconFlame2 | Used | Used |
| iconFlame3 | Used | Used |
| iconFlame4 | Used | Used |
| iconFlame5 | Used | Used |
| iconFlame6 | Used | Used |
| iconFlame7 | Used | Used |
| iconFlame8 | Used | Used |
| iconFriday | Unused | Unused |
| iconHighTemperature | Unused | Used |
| iconHome | Used | Used |
| iconHomeOv | Unused | Unused |
| iconLowTemperature | Unused | Used |
| iconMenu | Unused | Unused |
| iconMonday | Unused | Unused |
| iconMore | Unused | Unused |
| iconMultipleDays | Unused | Used |
| iconNone | Unused | Unused |
| iconSaturday | Unused | Unused |
| iconSensorOffset | Unused | Unused |
| iconSetpoint | Unused | Used |
| iconSettings | Unused | Used |
| iconShowerOff | Used | Used |
| iconShowerOn | Used | Used |
| iconSunday | Unused | Unused |
| iconTempHigher | Unused | Unused |
| iconTempLower | Unused | Unused |
| iconTemperature | Unused | Used |
| iconThursday | Unused | Unused |
| iconToday | Unused | Used |
| iconTomorrow | Unused | Used |
| iconTuesday | Unused | Unused |
| iconWednesday | Unused | Unused |
| iconWeekend | Used | Used |
| iconWeekendOv | Unused | Unused |
| iconWeekschedule | Unused | Used |
| iconWifiConnected | Used | Used |
| iconWifiLost | Used | Used |
| iconWork | Used | Used |
| iconWorkOv | Unused | Unused |
| remote-arrive | Unused | Used |
| remote-leave | Unused | Used |
| remote-menu | Unused | Used |
| remote-minus | Unused | Used |
| remote-next | Unused | Used |
| remote-plus | Unused | Used |
| remote-previous | Unused | Used |
| remote-sleep | Unused | Used |
| remote-wake | Unused | Used |
