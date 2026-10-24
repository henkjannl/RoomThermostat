# Remote alpha.3 — domestic-hot-water icon

24 October 2026. The alpha.2 remote always drew iconShowerOn, using 25% opacity
when idle and a still fainter version when unknown. This was an ambiguous display
choice: an idle boiler could still appear to be producing hot water.

Alpha.3 uses the original iconShowerOff artwork when a fresh boiler status says
domestic hot water is idle. That artwork is blank, matching the physical thermostat.
The shower-on icon is shown only when a fresh thermostat snapshot has BOTH
BoilerOnline and HotWaterActive. Old/disconnected remote status or a missing boiler
reply shows a question mark. The status page also retains explicit In gebruik,
Niet in gebruik or Onbekend text. This applies on home and the status page.

The activity bit continues to come from opentherm.isHotWaterActive(response), not
the heating/flame bit or the hot-water-enabled setting. This display fix does not
establish whether the user also observed an incorrect boiler activity bit.

Validated with the actual C++ firmware renderer: pixel comparisons for idle, active,
active-to-idle clearing, stale-active and boiler-offline states on both screens,
plus existing navigation/command tests. Both remote builds pass. Protocol and shared
assets are unchanged; remote alpha.3 works with thermostat alpha.2 or alpha.3.
The thermostat alpha.3 network fix is separate and still recommended.

Upload remote-cyd normally in VS Code, usb-test for USB testing. No filesystem
upload, flash erase or key changes. Then test with taps closed and with real hot
water running; check the status page text as well. These are host/build results,
not physical boiler validation. If In gebruik appears while the taps are closed,
compare with the physical thermostat/Telegram: the boiler activity bit needs separate
investigation, rather than further icon dimming.
