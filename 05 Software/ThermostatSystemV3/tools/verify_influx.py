"""Check the local token without printing it or writing fake thermostat readings."""
from pathlib import Path
import json
import re
import time
import urllib.error
import urllib.parse
import urllib.request


def main():
    config = Path(__file__).resolve().parents[1] / "thermostat/include/influx_config.h"
    text = config.read_text(encoding="utf-8")

    def setting(name):
        match = re.search(r"^#define\s+" + name + r"\s+(\"[^\n]+\")\s*$", text, re.M)
        if not match:
            raise SystemExit("Missing local setting: " + name)
        return json.loads(match[1])

    url = setting("THERMOSTAT_INFLUX_URL")
    token = setting("THERMOSTAT_INFLUX_TOKEN")
    if not token:
        raise SystemExit("Local token is empty; configure it first.")
    endpoint = urllib.parse.urlsplit(url)
    health = urllib.parse.urlunsplit((endpoint.scheme, endpoint.netloc, "/health", "", ""))
    try:
        with urllib.request.urlopen(health, timeout=5) as response:
            status = json.load(response)
        print("InfluxDB:", status.get("version"), "health:", status.get("status"))
        if status.get("status") != "pass":
            raise SystemExit("Influx health check failed.")
        body = f"room_thermostat_setup,device=room-thermostat write_test=true {time.time_ns() // 1000000}\n"
        request = urllib.request.Request(url, data=body.encode("ascii"), method="POST", headers={
            "Authorization": "Token " + token,
            "Content-Type": "text/plain; charset=utf-8",
        })
        class NoRedirect(urllib.request.HTTPRedirectHandler):
            def redirect_request(self, req, fp, code, msg, headers, newurl):
                return None
        with urllib.request.build_opener(NoRedirect()).open(request, timeout=5) as response:
            if response.status != 204:
                raise SystemExit("Unexpected write status: " + str(response.status))
        print("HTTP 204: setup probe accepted in room_thermostat_setup; no fake room measurement written.")
    except urllib.error.HTTPError as error:
        raise SystemExit("Influx request rejected: HTTP " + str(error.code)) from None
    except urllib.error.URLError:
        raise SystemExit("Influx network request failed; check LAN/server reachability.") from None


if __name__ == "__main__":
    main()
