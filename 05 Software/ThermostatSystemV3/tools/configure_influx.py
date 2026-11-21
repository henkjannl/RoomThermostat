"""Store a write-only Influx token locally without echoing it or putting it in argv."""
from pathlib import Path
import getpass
import json
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--replace", action="store_true", help="Replace the previously entered local token")
    args = parser.parse_args()
    target = Path(__file__).resolve().parents[1] / "thermostat/include/influx_config.h"
    if target.exists() and not args.replace:
        raise SystemExit("Local configuration already exists; edit it locally instead of overwriting it.")
    token = getpass.getpass("Influx token (Write only on Zonstraat35): ").strip()
    if not token or any(c.isspace() for c in token):
        raise SystemExit("Token must be nonempty and contain no whitespace.")
    # JSON encoding also produces an escaped ASCII C string for this token.
    with target.open("w" if args.replace else "x", encoding="utf-8") as output:
        output.write('#pragma once\n#define THERMOSTAT_INFLUX_ENABLED 1\n')
        output.write('#define THERMOSTAT_INFLUX_URL "http://192.168.178.63:8086/api/v2/write?org=Zonstraat35&bucket=Zonstraat35&precision=ms"\n')
        output.write('#define THERMOSTAT_INFLUX_TOKEN ' + json.dumps(token, ensure_ascii=True) + '\n')
    print("Local configuration saved. Rebuild firmware; do not publish the header or firmware.")


if __name__ == "__main__":
    main()
