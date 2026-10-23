# Room thermostat 2.1.4

## 2.1.2 memory and timing update

- Network task stack reduced from 16 KiB to 12 KiB, releasing 4,096 bytes of reserved heap. Based on 2.1.1's observed 5,416-byte peak usage, this leaves 6,872 bytes of margin above that observation; it is not proof of worst-case stack usage.
- Timing reports now separate maximum smooth-font loading, text drawing into sprites, and sprite transfer durations in microseconds. Measurements cover home fields and menu rows, exclude the welcome screen, and accumulate since boot. Font loading and text rendering include their underlying filesystem activity; renderer checkpoints are outside these individual measurements.
- No graphics, SPI speed, scheduling, PID or boiler interval changes in this revision.

Upload firmware only and confirm 2.1.2. Exercise the same menus/buttons and Telegram reports before comparing timings. Include network reconnection and large reports when checking minimum heap, largest block and minimum free network stack. Earlier 2.1.0 and 2.1.1 maxima were collected under different button activity, so they are not controlled benchmarks. Request Debug and Timing after 5 and 15 minutes; the new stage maxima will guide any further display optimization.

## 2.1.1 display and diagnostics update

- Home-screen fields/icons are compared with their displayed values. Only changes are drawn. At most one field/icon is serviced per main-loop iteration; new requests are coalesced. Day icons remain blank until clock/schedule validity is established. Unset time and disconnected temperature have explicit placeholders.
- The home background renders eight rows per iteration. Menu titles, subtitles and entries render one row per iteration. Static menus ignore periodic status-only refresh requests; sensor calibration remains live. Screen changes invalidate display caches and cancel the old background render.
- 51 original JPEG assets (icons and home background) are converted to RGB565 constants in firmware flash (265,696 pixel bytes). Transfers use one fixed 640-byte row buffer; there is no icon cache or full-screen framebuffer in heap. Original artwork is preserved subject to RGB565 color quantization. No filesystem upload is required.
- Button polling and OpenTherm receive processing run at renderer checkpoints, without re-entering UI/controller dispatch. Request transmission remains in the main-loop temperature service. SPI stays at 40 MHz. Individual font/file operations are still synchronous; measured timing is needed to establish their maximum duration.
- Text sprites are allocated only for changed home fields or the current menu row, allocation failures are counted, and the welcome-screen font is released. The 16 KiB network task stack remains unchanged until measured headroom justifies reducing it.
- Debug adds largest free heap block and network-task minimum free stack (bytes on this ESP32 framework). Timing adds maximum render-step/pixel-chunk duration and sprite allocation failures. Render maxima are since boot; button-poll maximum retains the existing periodic reset. These software measurements are not electrical timing measurements.
- Boiler actual temperatures are reported as unavailable/stale if the latest read failed or is over 30 seconds old. Percentage is labeled calculated heating demand, not measured burner modulation.
- Five-second boiler scheduling, PID gains, core ownership and backlight behavior are unchanged.

Regenerate artwork with `python test/generate_display_assets.py` (Pillow required); verify with `python test/verify_display_assets.py`. The generated header is included, so Pillow is not needed to build firmware.

After firmware-only upload, confirm version 2.1.1, inspect icon colors and all menus, navigate rapidly while a home-screen redraw is in progress, and test cold startup before time sync. Send Debug and Timing reports after 5 and 15 minutes of screen/Telegram use. Check allocation failures remain zero, stack/heap headroom, button-poll gaps and valid boiler-status gaps. Compare against 2.1.0's measured 1,367 ms display maximum; this update has not been measured on hardware yet.

Candidate derived from the complete Roomthermostat V2.0 directory. V2.0 is unchanged.
No Git migration or device upload was performed.

## Changes

- OpenTherm exchanges use the bundled 1.1.5 library's `sendRequestAync()` and `process()` API. Each transaction is advanced from the main loop, including the driver's recovery interval. One transmission still occupies approximately 34 ms.
- Status, heating setpoint, hot-water setpoint and temperature queries are spread across loop iterations. The status cycle starts every five seconds when the previous cycle has finished. A failed required status/heating request aborts the cycle; unsupported optional hot-water/telemetry requests are logged and skipped. Returned data IDs are checked. Failed telemetry reads retain the last reading.
- Boiler scheduling no longer depends on display messages or backlight state. Button scanning is not disabled for boiler communication.
- Buttons are polled in loop context at a nominal 10 ms interval, with 30 ms debounce and one event per press. No touch reads or message construction run in a hardware interrupt.
- DallasTemperature conversion is started without waiting. Results are collected after the configured conversion time. Sensor addresses are capped at four; invalid/non-finite readings are rejected. OneWire transfers run between boiler exchanges.
- Backlight fading advances one brightness step at a time, without delay loops.
- Central-heating enable now uses the calculated demand. Central heating is disabled at startup until a valid room measurement exists, on measurement failure, and when the last valid measurement is 30 seconds old. The PID integral is reset while measurements are invalid. Hot-water enable is independent. This is a software control policy, not a replacement for boiler protection.
- PID elapsed seconds retain fractional precision; gains are unchanged.
- Serial diagnostics include each OpenTherm step/status/duration and maximum loop/button-poll gaps. Status numbers follow the bundled library enum. Maximum touch-read duration is since boot; loop/poll gaps reset at each report.
- Removed serial printing of the bot API token and fixed the timezone parse error exit that left button polling disabled.
- Wi-Fi setup/reconnection, Telegram, timezone HTTP and SNTP now run in a priority-1 worker pinned to Core 0. Core 1 owns OpenTherm initialization/processing, sensors, controller state, touch polling, screen/JPEG rendering, and NVS saves. Startup no longer waits for network connectivity or DNS; losing Wi-Fi no longer intentionally reboots the thermostat.
- The worker owns all network clients and conversations. Core 1 publishes bounded value-copy snapshots once a second; Telegram commands return through the controller queue. Command acknowledgements wait for a snapshot at least as recent as the applied command. A newer consistent snapshot may be used when commands arrive rapidly. No live strings, maps, logs or controller references cross the queues.
- Snapshots include bounded log (4095 bytes plus terminator) and timing (2047 bytes plus terminator) text; longer reports are truncated. Latest status snapshots overwrite older pending snapshots, while up to 20 command acknowledgements are queued. Overflow drops a reply, never blocks boiler control, and increments a serial diagnostic counter. The command may already have been applied when its reply is dropped.
- Caller-owned `localtime_r()` buffers replace shared time-conversion buffers in both tasks. Device reboot requests return to Core 1. OTA remains disabled and the network worker performs no application flash writes.
- Pending NVS autosaves wait until OpenTherm is idle and the driver's recovery interval has ended. This prevents an application autosave during reception; flash writes can still pause both cores. The worker always yields and does not run above the Wi-Fi/TCP-IP system tasks.

## Remaining responsiveness limits

Telegram, timezone HTTP requests and Wi-Fi operations remain synchronous inside their worker, so remote replies can be delayed by slow networks. They no longer execute in the control/UI loop. HTTP connection/read timeouts and TLS handshake limits bound individual operations, not the duration of an entire Telegram batch or a hard real-time deadline.

Display redraws, serial logging, sensor bus transfers, snapshot/report preparation and NVS saves still consume Core 1 time. The OpenTherm bit-banged transmission remains approximately 34 ms and can be affected by interrupts. This design preserves the requested five-second batch target; it does not establish a hard timing guarantee or claim conformance to OpenTherm's stricter nominal communication interval. No backlight changes were added in the network-worker revision; the earlier nonblocking fade remains.

## Build

Open this directory as a PlatformIO project and run `pio run`. The tested board is `mhetesp32minikit`, Espressif32 platform 6.10.0, Arduino ESP32 2.0.17. The copied `.pio/libdeps` includes the original project dependencies and TFT configuration; keep those files for reproducing this hardware configuration. Existing dependency warnings are documented in VALIDATION.md.

Run `python test/verify_architecture.py` for the ten source-boundary regression checks. They verify ownership restrictions, snapshot field coverage, task placement, five-second scheduling, flash-save gating, and reentrant time conversions. They are not runtime or electrical tests. Compilation additionally verifies queue payloads are trivially copyable and the Arduino control task is configured for Core 1.

Do not upload the filesystem just to test the firmware: the data directory contains configuration and display assets. Review PlatformIO upload targets and preserve existing device settings. Firmware upload has not been performed.

## Hardware acceptance checks

1. Save the existing firmware/configuration or confirm a known working V2.0 upload path before installing this candidate. Confirm the configured display pins and board match the thermostat.
2. Verify the serial version is 2.1.4. Record the baseline and new maximum button-poll/loop gaps at 115200 baud, with Telegram active and inactive.
3. Repeatedly wake the screen, navigate menus, change temperature, hold/release buttons, and interrupt a fade with a new press. Check for missed or duplicate events and crashes.
4. Confirm boiler status is exchanged with the backlight both on and off. Verify CH enable, heating setpoint and domestic hot-water behavior at low and high demand.
5. Test a silent/disconnected boiler: UI must remain usable during each response timeout. Reconnect and verify recovery. Check optional unsupported telemetry does not prevent subsequent cycles.
6. Disconnect room sensors while heating, then reconnect. CH disable must be requested after invalid detection (or expiry of a stale reading) on the next status exchange. Verify recovery after measurement/re-discovery. If the bus cannot deliver messages, the thermostat cannot guarantee boiler shutdown.
7. Boot with Wi-Fi unavailable and verify local control/boiler communication still start. Then disconnect/reconnect Wi-Fi, test slow/unavailable Telegram and timezone services, and compare the maximum transmit gap/late-cycle diagnostics. Confirm no network-outage restart occurs. Boot should report control/UI on Core 1 and the network worker on Core 0 at priority 1.
8. Check schedules, overrides, midnight transitions, calibration and NVS settings survive reboot. First synchronization and clock corrections preserve overrides; verify real midnight advances them exactly once.
9. While the screen is active, issue Telegram changes and verify acknowledgements show the applied values when Telegram resumes. Exercise rapid commands and check dropped-reply counters. Force an autosave and confirm it starts between boiler exchanges. Run long enough to observe minimum heap and the network-worker heartbeat age under TLS reconnections.

Actual responsiveness, electrical timing and boiler behavior require the connected hardware; compilation alone does not validate them.

## Review corrections (23 October 2026)

- Wi-Fi persistence is disabled before driver initialization. This prevents configuration persistence through this path; it is not a guarantee that framework initialization never accesses flash.
- Telegram accepts only the configured ChatID, waits for a valid clock, and retries failed initialization at 30-second intervals. An empty ChatID disables Telegram. A configured group authorizes the group, not individual members.
- Before valid time, local heating uses lowTemp unless a manual temperature override is active. Existing day overrides are preserved. After synchronization, weekly scheduling resumes. Once synchronized, the clock continues through Wi-Fi outages.
- Initial clock synchronization, backward/forward clock corrections, timezone/DST changes and control-service gaps over five minutes rebase schedule tracking without consuming overrides or expiring a manual hold. Ordinary midnight advances overrides. Small clock corrections within two seconds are treated as continuous time.
- OpenTherm replies must match both the data ID and read/write acknowledgement type. Diagnostics separately report successful status-response age and maximum observed gap; these are software observation times, not electrical measurements.
- Five-second scheduling, core ownership, PID gains and existing CH/sensor-failure policy are retained. No additional backlight changes.

Additional hardware checks: cold boot without network/time; first synchronization with today/tomorrow and manual overrides set; normal midnight; DST/time correction; configured versus unauthorized Telegram chats; failed Telegram startup and retry. Capture actual OpenTherm traffic during display redraws, NVS saves and Wi-Fi reconnections. Validate the previously changed CH-enable, fractional PID timing and sensor-failure policy independently against V2.0.

Startup icon correction: weekday icons and their override borders remain blank until the clock is valid and the controller has calculated the seven-day schedule. They remain available during subsequent Wi-Fi outages while the clock is valid. Hardware check: cold boot without Wi-Fi, reconnect and verify icons appear with the correct day ordering; disconnect Wi-Fi again and verify they remain visible.



## 2.1.3 flash-backed fonts

All four fonts used by the display now use TFT_eSPI's memory-backed loadFont API. Original VLW bytes are embedded unchanged in firmware flash (157,541 bytes); font headers, glyph metrics and glyph alpha bitmaps are read without filesystem access. The existing font-loading, text-drawing and sprite-transfer diagnostics remain comparable. SPI, PID, boiler interval and the 12 KiB network stack are unchanged.

This is not a permanent RAM font cache. TFT_eSPI still allocates glyph metrics while a font is loaded (1,116 bytes for each 93-glyph font or 2,244 bytes for the 187-glyph submenu font, plus allocator overhead) and releases them on unload. The large glyph bitmaps stay in flash. Existing sprite allocation and font-unload lifetimes are retained.

Regenerate with `python test/generate_font_assets.py`; verify exact source agreement and glyph bitmap bounds with `python test/generate_font_assets.py --check`. No extra Python packages are required. The generated header is supplied, so regeneration is not required for building.

Upload firmware only, confirm 2.1.3, inspect every menu and temperature/date text, then repeat the same button and Telegram activity used for 2.1.2. Compare font-load max (previously 111,448 us), text-draw max (23,385 us), button-poll gaps and memory/stack headroom. On-device appearance and performance have not yet been validated.


## 2.1.4 Amsterdam summer/winter time

Europe/Amsterdam now uses the local POSIX rule CET-1CEST,M3.5.0/2,M10.5.0/3, shared by display and heating schedule. NTP supplies UTC; the ESP32 converts locally, including DST transitions without Wi-Fi once its clock is valid. Amsterdam no longer calls WorldTimeAPI. Other configured zones retain the previous web-based offset path; this is not a general IANA-zone database.

The network worker refreshes diagnostic DST metadata even while disconnected. Offset changes trigger a controller/display refresh. Existing schedule guards preserve overrides on clock corrections and repeated/skipped local hours. Debug reports local time, timezone and DST state. Cold startup still waits for a valid clock before weekly scheduling; manual/low-temperature fallback is unchanged.

Validation: test/verify_timezone_rules.py compares the configured POSIX transition policy against Python zoneinfo's Europe/Amsterdam for winter, summer and immediately before/at/after both transitions in 2024–2030 (56 cases). Python requires an available IANA database/tzdata. This tests rule/calendar logic, not execution of the ESP32 C library. Hardware check: firmware-only upload, version 2.1.4, correct local clock and Debug timezone/DST; verify ordinary schedule behavior and subsequent offline operation. On 23 October 2026 the expected status is CEST/DST active, UTC+2; on 25 October at 03:00 local it changes to CET, UTC+1.

The supplied 2.1.3 measurements support keeping display, SPI, PID and 12 KiB network-stack settings unchanged. Font loading peaked at 2,034 us, text drawing at 2,388 us, minimum heap was 33,148 bytes and network stack minimum free was 6,840 bytes over 40 minutes. Button-gap measurements include work outside the timed loop region and have a different reset interval from display maxima; do not infer their cause solely from those aggregate values.
