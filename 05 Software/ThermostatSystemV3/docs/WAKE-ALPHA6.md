# Remote wake transition — alpha.6

24 October 2026. Remote-only update; thermostat alpha.5 remains compatible.

The remote keeps the initial “Even verbinden / Temperaturen ophalen” screen until
it receives its first authenticated thermostat snapshot. It no longer paints the
home screen from the default empty packet after battery measurement/radio startup.
The host boot ID is populated only when serviceRemoteRadio accepts Status/Ack from
the confirmed peer with the matching session/request. Discovery/Welcome is not data.

No home buttons are drawn or enabled before that snapshot. After a failed discovery
round it shows “Geen verbinding / Tik om opnieuw te zoeken”; tapping starts a fresh
round and immediately restores the waiting screen. Radio initialization errors show
“Radiofout”. The existing release-before-action guard remains in place.

After one real snapshot, a later lost link still shows the last real values with
the existing stale/unknown indication. This update does not discard useful readings
or change retry timing, awake timeout, radio cadence, protocol v3 or power behavior.
All custom alpha.5 icon assets and earlier fixes are retained.

Validated with the actual C++ renderer: wait-screen pixel equality before any
snapshot, unchanged wait screen on battery/discovery changes, no home controls or
commands during waiting, failure/retry states, first-snapshot transition caused by
boot ID alone and retention of real data on a subsequent lost link. Existing
navigation, flame, shower and protocol tests, fourteen architecture checks and
56 timezone cases pass. All three firmware builds are checked. These are host/build
tests; the physical wake transition still needs a check after upload.

Upload only remote-cyd using normal PlatformIO Upload (usb-test or battery as
appropriate). Remote reports 3.0.0-alpha.6. No thermostat update, filesystem upload,
flash erase or regeneration of the user's icon exports is required.
