# Matching flame icons — alpha.4

24 October 2026. Thermostat and remote both become 3.0.0-alpha.4.

The thermostat sends its already-selected `dspFlameIcon` as a level 0..8. The remote
uses the same original iconFlame0..iconFlame8 artwork. It does not infer the level
from centralHeatingActive, flameOn or boilerPercentage. This preserves the exact
selection used by the physical thermostat, including level zero when the controller
chooses it. The remote's smaller 24-pixel rendering remains unchanged.

Changes appear on the next successful snapshot, normally within the 1.5-second poll
interval while awake. Old or disconnected status remains visually dimmed. No radio
traffic or wakeups have been added, and the OpenTherm interval is unchanged.

## Upload

The authenticated status/ack packet gains one byte and is explicitly protocol v3,
124 bytes including its HMAC tag. It is deliberately incompatible with old wire v2:
neither device accepts the old layout as the new one. Upload BOTH devices:

1. In the moved project, open thermostat in VS Code and upload mhetesp32minikit.
2. Open remote-cyd and upload usb-test (or battery for standalone discharge logging).
3. Confirm alpha.4 on both and compare their flame icons while the remote is awake.

Normal Upload only; no filesystem upload, flash erase or key changes. Alpha.4 also
includes the alpha.3 Telegram socket-timeout correction and shower-indicator fix.

## Validation

- Actual C++ renderer pixel comparisons for levels 8 down to 0, checking that a
  level-only change repaints without relying on HeatingActive, and that invalid
  level 255 falls back to level zero. Existing shower/navigation/command tests pass.
- Host source check confirms export from dspFlameIcon; a compile-time assertion
  checks contiguous flame enum values.
- Five contract/HMAC tests use protocol v3, 124-byte layout and the authenticated
  flame-level byte. Fourteen architecture checks and 56 timezone cases pass.
- Build thermostat, remote usb-test and remote battery. These are host/build results;
  equality during actual heating and the radio snapshot delay require hardware testing.

All nine flame icon files are now used by BOTH devices. The older icon-usage report
that lists Flame2..Flame8 as unused on the remote describes alpha.3 and earlier.
