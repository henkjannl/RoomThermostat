"""Read selected non-secret thermostat diagnostics without toggling reset lines."""
import argparse
import time
import serial

parser = argparse.ArgumentParser()
parser.add_argument("--port", required=True)
parser.add_argument("--seconds", type=int, default=45)
args = parser.parse_args()
prefixes = ("OpenTherm", "Software version:", "Influx", "Control/UI", "Network worker",
            "Initializing", "Starting network", "Setup finished", "ESP-NOW", "Network slow/",
            "rst:", "ets ", "Guru Meditation", "Backtrace:", "abort()", "E (")
port = serial.Serial()
port.port = args.port
port.baudrate = 115200
port.timeout = .2
port.dtr = False
port.rts = False
try:
    port.open()
    end = time.monotonic() + args.seconds
    while time.monotonic() < end:
        line = port.readline().decode(errors="replace").strip()
        if line.startswith(prefixes):
            print(line, flush=True)
finally:
    port.close()
