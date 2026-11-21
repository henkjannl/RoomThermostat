# InfluxDB logging — thermostat alpha.8

Prepared 1 November 2026 for AsusHome, Debian 13, InfluxDB OSS 2.9.1
at `192.168.178.63:8086`. Organization and bucket: `Zonstraat35`.
Server SSH access and its healthy InfluxDB 2.9.1 endpoint were verified today.
The corrected dedicated token passed an authenticated write into organization/
bucket Zonstraat35: HTTP 204 for a separate room_thermostat_setup probe.
The enabled firmware was uploaded to the confirmed thermostat on COM7. Real writes
were acknowledged. A user-operated 60-second InfluxDB container outage retained
the pending batch; OpenTherm continued with zero observed late cycles, and the
queue drained to zero after recovery. Observed maximum valid-status gap: 5,045 ms.
Wi-Fi outages, long overflow tests and real DHW transitions remain untested on
hardware; see VALIDATION.md for the exact scope.

## Control ownership

Core 1 retains all PID, sensor and OpenTherm ownership and the five-second boiler
cycle. Logging adds fixed-size, zero-wait queue operations; it performs no network
or flash writes on this core. Core 0 uses the existing priority-1 network worker;
there is no new task or control dependency on InfluxDB. Queue-allocation failure
disables logging. Local control still starts. Existing display, NVS, shared heap,
Wi-Fi interrupts and watchdog limitations remain: this is not a hard real-time
guarantee. Verify boiler timing and memory on the actual hardware under outages.

## Data

Measurement `room_thermostat`, fixed tag `device=room-thermostat`:

| Field | Meaning |
| --- | --- |
| `room_temperature_c` | Calibrated room reading, omitted if invalid/stale |
| `room_setpoint_c` | Requested room temperature |
| `heat_demand_pct` | PID output normalized over the 5–60 °C output range, 0–100%; not measured burner modulation or kW |
| `boiler_setpoint_c` | Calculated OpenTherm CH temperature demand; `ch_enabled` shows whether CH is enabled |
| `dhw_active` | Boiler's valid OpenTherm domestic-hot-water status; omitted while unknown |
| `dhw_enabled` | Thermostat's warm-water enable setting; separate from activity |
| `room_valid`, `status_valid` | Measurement/status quality |
| `ch_enabled` | Thermostat's central-heating enable request |
| `kind` | 0 periodic sample, 1 status/validity transition, 2 DHW enable change |
| `boot_id`, `sequence`, `uptime_ms` | Boot identification, record order and monotonic acquisition time |
| `dropped_records` | Cumulative overflow loss this boot (captured with the record) |

Capture a periodic record every ten seconds; capture valid DHW activity edges
at the status response, enable changes on the next control loop, and status
failures immediately. Status older than 15 seconds becomes unknown. Repeated
identical statuses do not generate extra events. Recovery records the current
state; no edge time is invented inside an observation gap. Resolution is the
existing roughly five-second polling cycle; shorter activations can be missed.

UTC millisecond timestamps use acquisition uptime. Before first NTP sync records
stay buffered; once time is valid their calendar time is estimated from the
current UTC/uptime pair. Established ESP time continues without Wi-Fi. Large
clock corrections affect the estimated times of records not yet prepared for
upload. Timestamps are monotonically increased by at least 1 ms if the clock
moves backward, preventing overwrites within a boot; acquisition `uptime_ms`
and `sequence` preserve the original ordering. This is not a precision event clock.

## Buffer and transport

128 queued records plus up to four records in a pending HTTP batch. On ESP32 a
record is 40 bytes: 5,120 queue payload bytes, FreeRTOS overhead and a 2,048-byte
static upload buffer. The queue uses heap only when enabled with a nonempty token.
At ten-second sampling the queue spans about 21 minutes; extra events reduce it.
On overflow oldest queued records are discarded and counted. The pending batch
is retained. All pending/queued data is lost on reset or power failure. No
filesystem partition or existing configuration is changed.

Core 0 sends up to four records per HTTP request, normally every 30 seconds and
at one-second intervals while draining backlog. Telegram/network blocking can
delay this worker; capture continues independently until the buffer fills.
Connect/read timeouts are one second each, not a total request-time guarantee.
Failures retry after 5, 10, 20, 40, 80, 160, then 300 seconds. Only HTTP 204
acknowledges a batch. A lost acknowledgement retries identical bytes/timestamps;
InfluxDB's point identity makes the retry idempotent. Authorization, retention or
schema failures also retain/retry the batch: inspect serial HTTP status and fix
the configuration; retries alone cannot fix a rejected point. Diagnostic lines
print response code, pending/queued counts and acknowledgements, never the token.

Transport uses the existing LAN HTTP endpoint; token and data are not encrypted
in transit. Automatic redirects are disabled. No router/firewall changes are
needed. For HTTPS a CA-verified TLS client must be implemented separately.

## Enable locally

1. In [InfluxDB](http://192.168.178.63:8086), create a custom API token named
   `room-thermostat-write`, with **Write only** on bucket `Zonstraat35`.
   Do not use the admin token or share the token in chat.
2. Run `python tools/configure_influx.py` from this project. It prompts without
   echo and writes ignored `thermostat/include/influx_config.h`. Alternatively,
   copy `thermostat/include/influx_config.example.h` to `influx_config.h` and
   enter the token locally. Missing header or empty token leaves logging disabled.
3. Run `python tools/verify_influx.py` to check health and send one separate
   `room_thermostat_setup` probe (no fake room readings). Expect HTTP 204.
4. Build the thermostat and upload **firmware only**, using its actual port.
   Preserve pairing secrets and configuration. The remote firmware/protocol is
   unchanged; the CYD does not need uploading for this feature.
5. Serial should show `Influx RAM logging ready`, then `Influx acknowledged=...`.
   Confirm that the records actually appear in InfluxDB before calling it working.

API reference: [InfluxDB OSS v2 write API](https://docs.influxdata.com/influxdb/v2/api/write/).

## Query / Grafana

Use the existing Grafana Flux datasource and its read-only token. For example:

```flux
from(bucket: "Zonstraat35")
  |> range(start: -1h)
  |> filter(fn: (r) => r._measurement == "room_thermostat")
  |> filter(fn: (r) => r._field == "room_temperature_c" or
                       r._field == "room_setpoint_c" or
                       r._field == "boiler_setpoint_c" or
                       r._field == "heat_demand_pct")
```

For DHW use a state timeline of `dhw_active`, with a second row for
`status_valid`; show unknown periods as gaps and do not fill them with “off”.
Keep `dhw_enabled` in a separate row. Do not average away event changes.

## Verification before normal use

- Compare Influx points to serial/local values; check PID percent interpretation.
- Turn hot water on/off and confirm status events and timestamps; change enable
  independently. Disconnect the OpenTherm link: unknown must not become false.
- Stop InfluxDB, then restore it; check queue recovery, original timestamps,
  retries and continuity of five-second OpenTherm cycles.
- Boot without Wi-Fi/time and operate the local UI; reconnect and verify upload.
- Leave Wi-Fi/server unavailable beyond buffer capacity; verify overflow counts
  and responsive control, then backlog recovery. Reboot to verify RAM-loss behavior.
- Exercise Telegram/CYD/UI together with upload and TLS reconnections; inspect
  minimum free heap, largest block, network stack margin and maximum boiler gaps.

Host tests and compilation do not prove these hardware acceptance criteria.
