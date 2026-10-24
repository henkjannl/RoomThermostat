# Updated icon exports — alpha.5

24 October 2026. Both devices report 3.0.0-alpha.5; protocol remains v3.

The user's 12 icon*.png exports and 14 remote-/battery-PNG exports are now included
in the generated flash assets. Where iconName.png exists, it overrides the original
iconName.jpg pixels, while the runtime lookup name remains /iconName.jpg. Original
JPEG artwork is preserved. All transparent pixels are composited onto white before
RGB565 conversion, including partial alpha at antialiased edges. No renderer lookup
changes or filesystem upload are required.

For subsequent icon edits, put the exported 32x32 PNG in shared/assets/icons, then
run tools/generate_display_assets.py with Python and Pillow, followed by
tools/verify_display_assets.py. Normal PlatformIO Build/Upload does not run this
generator automatically. Changing only an SVG or PNG does not change the embedded
header until regeneration. Do not rerun the old control-icon generators unless you
intend to replace the hand-edited PNGs with their generated versions.

Build thermostat/mhetesp32minikit, remote/usb-test and remote/battery, then use normal
Upload: thermostat first, remote second. No flash erase or Upload Filesystem.
Alpha.5 retains all alpha.4 networking, shower and matching-flame functionality.

Validated on the computer: 65 generated images against selected source pixels,
PNG-precedence/transparency regression tests, actual C++ remote screen rendering
and navigation/flame/shower checks, five protocol contracts, fourteen architecture
checks, 56 timezone cases, and all three PlatformIO builds. Menus were visually
reviewed from the actual C++ renderer. Physical screen colors/touch still require
checking after upload. Icon usage is unchanged: unused exports remain unused.
The home-screen red thermometer and target still come from HomeScreen.jpg, so edits
to standalone iconTemperature/iconSetpoint do not replace those two graphics.
