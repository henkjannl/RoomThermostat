# Validation of 2.1.4 candidate

Date: 23 October 2026.

## Completed

- PlatformIO debug build for `mhetesp32minikit`: SUCCESS after final code changes.
- Espressif32 6.10.0 / Arduino ESP32 2.0.17 / Xtensa toolchain 8.4.0.
- RAM: 74,092 / 327,680 bytes (22.6%; static allocation only, excludes dynamically allocated task stack/queues and TLS heap).
- Flash: 1,728,317 / 2,097,152 bytes (82.4%).
- Reviewed the sensor startup, conversion-completion, failed-reading and stale-reading paths; required and optional OpenTherm response handling; response data-ID checking; button debounce/release handling; and reversible backlight fades.
- Confirmed the modified temperature/button/backlight modules contain no synchronous OpenTherm convenience calls, delay loops or touch-reading timer ISR.
- Copied bundled libraries and display/data assets are retained from V2.0 without intentional modifications.
- Network worker is pinned to Core 0 at priority 1, with a 16,384-byte task stack and a 20 ms delay between iterations. Core 1 configuration is checked at compile time. Task/queue allocation failures leave local control running and log an error.
- Ten automated source-boundary regression checks pass (`python test/verify_architecture.py`). These guard control-loop/network separation, worker/peripheral ownership, Telegram snapshot-field coverage, core/priority/nonblocking acknowledgement handoff, idle-only flash saving/five-second boiler scheduling, and reentrant time conversion. The time-conversion check found a remaining `localtime()` use in the display, which was corrected before the final build.
- Compiler assertions verify that snapshot and command payloads are trivially copyable. Fixed-size queue payloads own no pointers, strings or containers. Initial configuration is copied before the worker is created; runtime controller updates are transferred as value-copy snapshots.
- Controller acknowledgements carry a revision and are processed only after the worker has received a snapshot at least that recent. Queue overflow is counted and never makes the control task wait for the network worker. Log/timing text snapshots have documented size limits.
- Reviewed application flash writes: NVS autosave now waits for idle OpenTherm state and driver readiness. Disabled OTA setup was removed from the network path. Wi-Fi configuration persistence is disabled, and the worker does not access filesystem assets.

The initial full build emitted two existing OneWire 2.3.8 warnings about extra tokens in `#undef` directives. The final incremental build succeeded without new application warnings.

## Not performed

No on-device upload, hardware timing measurement, live boiler exchange, physical button test, sensor-disconnection test or network-outage test was performed. The automated checks inspect source boundaries; they are not a runtime scheduler/queue simulation. Source review and successful compilation do not establish runtime behavior or a hard maximum transmission gap. Follow README-V2.1.md for hardware checks. The requested five-second target is retained, without claiming compliance with the stricter nominal OpenTherm communication interval.

## Changed source files

- `src/main.cpp`: unchanged 2.1.0 version, task startup, timing diagnostics, button service and idle-only NVS save.
- `src/d_keyboard.h`: polling and debounce outside interrupt context.
- `src/h_temperature.h`: asynchronous sensor/boiler state machines, measurement validity and CH enable.
- `src/l_backlight.h`: timed fading.
- `src/k_display.h`: remove display-to-boiler scheduling.
- `src/i_wifi_time.h`: worker-owned Wi-Fi/HTTP/SNTP, reconnect retries/timeouts and removal of outage-triggered restart/startup DNS halt.
- `src/f_telegram.h`: worker-private presentation state/clients, bounded polling batches, queued reboot requests, removal of OTA setup, copied log/timing reports.
- `src/network_messages.h`: bounded POD snapshots, revisioned acknowledgements and network-status queue.
- `src/m_network.h`: Core 0 worker and Core 1 status handoff.
- `src/a_constants.h`, `src/e_menu.h`, `src/g_controller.h`, `src/k_display.h`: caller-owned time-conversion buffers where used; controller reply bridge/reboot routing in `g_controller.h`.
- `src/c_interfaces.h`: use `chatID.c_str()` in varargs formatting of queued commands.
- `test/verify_architecture.py`: automated source-boundary checks.

PlatformIO configuration, bundled library implementations, NVS layout, scheduling rules and PID gains are retained. Existing nonblocking backlight behavior was not further modified during the network-worker work.

## Review-fix validation

Final PlatformIO build and all ten source-boundary checks passed after the review corrections. Added checks cover Wi-Fi persistence ordering, chat authorization before dispatch, clock-gated/retryable Telegram startup, clock initialization guards, and expected OpenTherm acknowledgement types/status diagnostics. These are source checks, not executable behavioral tests of the scheduler or clock policy.

Reviewed clock paths: invalid time selects lowTemp/manual override; first valid time rebases tracking; continuous midnight rolls overrides; discontinuous epoch/local time and service gaps over five minutes rebase without consuming overrides. These paths still require on-device testing, including DST and reconnection. Successful status age is measured when the main loop processes the reply, not at the wire edge. Existing driver late-processing timeout behavior and UI/flash timing limits remain. No bundled driver modification or hardware upload was performed.

Startup icon correction: build and ten existing source checks pass. Reviewed invalid-clock and schedule-not-yet-calculated guards; the icon area is cleared before the guard to remove stale icons/borders. No new screen buffer. Physical startup/reconnect rendering remains untested.


## 2.1.1 validation

- Final PlatformIO build: SUCCESS. Application flash remaining: 528,131 bytes. Static RAM grows by 1,096 bytes versus the last 2.1.0 build. Runtime TLS, sprites and font allocations are not included in static RAM figures.
- Ten existing source-boundary checks pass, including snapshot coverage for new telemetry-validity fields. These remain structural checks, not executed scheduler tests.
- Verified all 51 generated flash assets against source JPEGs: dimensions, pixel counts, RGB565 quantization, and maximum width compatible with the 320-pixel row buffer. The converter preserves original artwork; no filesystem change/upload is required.
- Reviewed render progression: background interrupted by navigation is discarded; home cache invalidates on re-entry; allocation failures retry without marking the field updated; home phases advance one widget at a time; menu rendering advances one row at a time. Time-invalid weekday icons remain blank. Menu items outside the viewport stop rendering and clear the trailing area.
- Rendering checkpoints poll buttons and process OpenTherm reception only. They do not dispatch menu/controller commands, access network clients, or re-enter the display/JPEG renderer. No extra task or display mutex is introduced.
- Confirmed ESP32 task stack high-water API reports bytes in the installed FreeRTOS headers. Debug queries the current network task from its own context; it does not read a task handle concurrently.
- User-supplied 2.1.0 baseline: display max 1,367 ms, loop max 1,415 ms, temperature max 60 ms; minimum heap 10,668 bytes. New firmware still requires on-device comparison, including graphics/color inspection, input latency, long TLS runs, stack margin and actual boiler traffic.
- SPI remains 40 MHz, the network stack reservation remains 16 KiB, and the five-second boiler interval and PID gains are unchanged. No claim of a hard render deadline: individual font/filesystem operations remain synchronous. Largest-free-block, stack and render-step diagnostics guide further changes.


## 2.1.2 validation

Final PlatformIO build succeeded; all ten source-boundary checks passed with the updated 12 KiB stack expectation. New timing counters remain Core-1-owned and are sent as snapshot text. Timing brackets exclude cooperative checkpoints. Static RAM: 74,028 bytes; application flash: 1,569,401 bytes. The smaller dynamically allocated task stack releases 4,096 bytes; this does not appear in static build RAM usage. No on-device validation or upload was performed. Reconnection, long Telegram reports and comparable button activity must validate remaining stack margin and timing. Earlier asset checks remain applicable because no assets or pixel-transfer code changed.


## 2.1.3 validation

PlatformIO build succeeded; ten architecture checks passed. Font generator --check verified all four embedded fonts match source VLW bytes and validated glyph/bitmap bounds. Installed TFT_eSPI source confirms memory-backed font loading and sprite glyph drawing use flash reads; glyph metrics still allocate on heap and unload normally. Final static RAM is unchanged at 74,028 bytes; flash is 1,728,317 / 2,097,152 bytes (82.4%), leaving 370,779 bytes. Existing OneWire preprocessor warnings remain. No dependency edits, filesystem upload or device upload. On-device glyph appearance, timing and memory still need comparison against the supplied 2.1.2 measurements.


## 2.1.4 validation

PlatformIO build SUCCESS; all ten source-boundary checks passed. 56 calendar/transition-policy tests passed against IANA Europe/Amsterdam for 2024–2030, including exact UTC boundaries and the current summer offset. These do not execute ESP32 libc. Reviewed Amsterdam's HTTP bypass, local-time evaluation while offline, offset-change notification, and worker ownership of timezone data. Build RAM 74,092 bytes; flash 1,728,317 bytes (82.4%). Existing OneWire warnings remain. No device upload or simulated hardware clock change was performed. Display/PID/SPI/stack settings are unchanged.
