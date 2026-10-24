# Thermostat network recovery — 3.0.0-alpha.3

24 October 2026. Thermostat-only update; remote alpha.2 remains compatible (wire v2).

## Report and confirmed defect

Telegram worked at 08:22, then both Telegram and the remote were unavailable less
than an hour later. OpenTherm and local buttons kept working; both network paths
recovered within a few seconds of a local button press, before the 20-second
backlight timeout. No serial capture of that failure is available.
The exact incident cause, especially the ESP-NOW failure, is not yet established.

The pinned Arduino-ESP32 2.0.17 implementation of
`WiFiClientSecure::setTimeout(uint32_t seconds)` multiplies its argument by 1000.
The application called `setTimeout(3000)`, configuring a 3,000-second / 50-minute
socket timeout. This affects TCP connect/write waits and can hold the Core 0 worker,
preventing its normal Wi-Fi recovery loop. It does not directly block Core 1 or
prove that ESP-NOW must stop. The earlier network isolation did not eliminate this
unit error. The correct socket setting is 3 seconds; handshake remains 5 seconds.
The Telegram response wait is separately 1500 milliseconds.

Local keys do not explicitly reconnect Wi-Fi. They temporarily disable Telegram
polling until the backlight expires (20 seconds); that can change traffic/retry
timing but does not establish why the connection recovered in this case.

## Change

- Unit-labelled Telegram configuration in `network_policy.h`, with a C++ regression
  test that runs the actual configuration helper against the library's unit contract.
- Worker phase and per-poll counters/timestamps shared through atomics. Core 1 emits
  an independent slow-worker report if the heartbeat is overdue, without killing
  the worker, restarting the thermostat or altering OpenTherm ownership.
- Periodic diagnostics include phase/age, pause state, polls/completion age, radio
  transmit-busy state, and count/age of authenticated remote requests. Slow-worker
  reports also include free heap and largest allocation.
- Core 0 logs Wi-Fi status/channel transitions without SSIDs or passwords and checks
  the driver's actual power-save setting. It reasserts WIFI_PS_NONE if necessary.
  This is a receiver safeguard, not evidence that modem sleep caused the incident.

The fix changes no pairing keys, stored settings, remote battery behavior or wire
protocol. It adds no remote periodic wakeups. Socket/handshake/response waits apply
per operation: DNS, library retries and multiple calls mean the whole worker loop
is not guaranteed to finish within three seconds.

## Validation and upload

Build thermostat, remote usb-test and remote battery; run the timeout C++ regression,
13 architecture checks, five contract tests and 56 Amsterdam timezone cases.
These are computer/build tests, not a reproduction of the morning failure.

Open the moved project's `thermostat` folder in VS Code and use normal PlatformIO
Upload (`mhetesp32minikit`). Confirm `3.0.0-alpha.3` on the thermostat. No remote
upload, filesystem upload or flash erase is needed for this fix.

To validate: leave the thermostat untouched for at least an hour and check Telegram
and repeated remote wakes. If it fails again, capture serial at 115200 before
pressing a physical button, including `Network slow/stalled`, `Network phase`,
`Receiver WiFi`, `Remote authenticated requests` and heap/OpenTherm diagnostics.
Then note the recovery timing after pressing a button. Serial monitors that reset
the device on connection would erase the live failure; keep a monitor connected in
advance when possible. Further hardware evidence is required before calling the
combined Telegram/remote failure solved.
