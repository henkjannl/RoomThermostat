"""Keep Arduino serial-upload addresses aligned with the CYD partition table."""
import csv
from pathlib import Path

Import("env")

table = Path(env.subst("$PARTITIONS_TABLE_CSV"))
with table.open(encoding="utf-8") as stream:
    rows = [
        [field.strip() for field in row]
        for row in csv.reader(line for line in stream if line.strip() and not line.lstrip().startswith("#"))
    ]
app = next(row for row in rows if row[1:3] == ["app", "ota_0"])
ota = next(row for row in rows if row[1:3] == ["data", "ota"])
app_offset, ota_offset = int(app[3], 0), int(ota[3], 0)
if int(env.subst("$ESP32_APP_OFFSET"), 0) != app_offset:
    raise RuntimeError("CYD upload.offset_address must match ota_0 in partitions.csv")
if int(ota[4], 0) != 0x2000:
    raise RuntimeError("CYD OTA metadata must reserve two 4 KiB sectors")

images = list(env.get("FLASH_EXTRA_IMAGES", []))
matches = [i for i, (_, path) in enumerate(images) if Path(str(path)).name == "boot_app0.bin"]
if len(matches) != 1:
    raise RuntimeError("Expected one Arduino OTA metadata image in upload configuration")
index = matches[0]
images[index] = (hex(ota_offset), images[index][1])
env.Replace(FLASH_EXTRA_IMAGES=images)
# Post scripts run after the serial uploader has copied FLASH_EXTRA_IMAGES.
# Update that assembled command as well as the image list used by other tools.
if env.get("UPLOAD_PROTOCOL") == "esptool":
    flags = list(env.get("UPLOADERFLAGS", []))
    matches = [i for i, flag in enumerate(flags) if Path(str(flag)).name == "boot_app0.bin"]
    if len(matches) != 1 or matches[0] == 0:
        raise RuntimeError("Expected one OTA metadata image in esptool upload flags")
    flags[matches[0] - 1] = hex(ota_offset)
    env.Replace(UPLOADERFLAGS=flags)
print("CYD upload layout: app=%s, OTA metadata=%s" % (hex(app_offset), hex(ota_offset)))
