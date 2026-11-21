"""Generate a credential-free Grafana Classic dashboard for the existing datasource."""
from pathlib import Path
import json

DS = {"type": "influxdb", "uid": "dg2wdkyp71lvkc"}
BASE = '''from(bucket: "Zonstraat35")
  |> range(start: v.timeRangeStart, stop: v.timeRangeStop)
  |> filter(fn: (r) => r._measurement == "room_thermostat" and r.device == "room-thermostat")'''


def query(fields, aggregate=True):
    result = BASE + '\n  |> filter(fn: (r) => ' + ' or '.join(f'r._field == "{f}"' for f in fields) + ')'
    if aggregate:
        result += '\n  |> aggregateWindow(every: v.windowPeriod, fn: last, createEmpty: false)'
    return result + '\n  |> keep(columns: ["_time", "_value", "_field"])'


def panel(id, title, type, x, y, w, h, flux, unit="none", description=""):
    result = {"id": id, "title": title, "type": type, "datasource": DS,
            "description": description, "gridPos": {"x": x, "y": y, "w": w, "h": h},
            "targets": [{"refId": "A", "datasource": DS, "query": flux}],
            "fieldConfig": {"defaults": {"unit": unit, "decimals": 1,
                "color": {"mode": "palette-classic"}, "custom": {"spanNulls": False}}, "overrides": []},
            "options": {"legend": {"displayMode": "list", "placement": "bottom", "calcs": ["lastNotNull"]},
                        "tooltip": {"mode": "multi", "sort": "none"}}}
    if type == "timeseries":
        result["interval"] = "10s"
        result["fieldConfig"]["defaults"]["custom"].update({"drawStyle": "line", "lineInterpolation": "linear",
            "lineWidth": 2, "fillOpacity": 5, "showPoints": "never", "insertNulls": 30000})
    return result


def rename(p, fields):
    p["fieldConfig"]["overrides"] += [{"matcher": {"id": "byName", "options": f},
        "properties": [{"id": "displayName", "value": label}]} for f, label in fields.items()]


def main():
    panels = []
    for id, field, title, unit, x in [(1, "room_temperature_c", "Kamertemperatuur", "celsius", 0),
                                     (2, "room_setpoint_c", "Ingestelde temperatuur", "celsius", 6),
                                     (3, "heat_demand_pct", "Warmtevraag", "percent", 12)]:
        p = panel(id, title, "stat", x, 0, 6, 4, query([field], False) + '\n  |> last()', unit)
        p["options"] = {"reduceOptions": {"calcs": ["lastNotNull"], "fields": "", "values": False},
                        "colorMode": "value", "graphMode": "none", "textMode": "value"}
        panels.append(p)
    age = BASE.replace('range(start: v.timeRangeStart, stop: v.timeRangeStop)', 'range(start: -7d)') + '''
  |> filter(fn: (r) => r._field == "sequence")
  |> last()
  |> map(fn: (r) => ({_time: r._time, _field: "Leeftijd", _value: float(v: uint(v: now()) - uint(v: r._time)) / 1000000000.0}))'''
    p = panel(4, "Laatste meting geleden", "stat", 18, 0, 6, 4, age, "s", "Leeftijd van het laatst opgeslagen meetpunt; normale verzending circa elke 30 seconden.")
    p["fieldConfig"]["defaults"].update({"color": {"mode": "thresholds"}, "thresholds": {"mode": "absolute", "steps": [
        {"color": "green", "value": None}, {"color": "orange", "value": 60}, {"color": "red", "value": 180}]}})
    p["options"] = {"reduceOptions": {"calcs": ["lastNotNull"], "fields": "", "values": False}, "colorMode": "value", "graphMode": "none"}
    panels.append(p)
    p = panel(5, "Kamertemperatuur en instelling", "timeseries", 0, 4, 12, 8,
              query(["room_temperature_c", "room_setpoint_c"]), "celsius", "Gaten blijven zichtbaar; geen verzonnen temperatuur bij ongeldige sensormeting.")
    rename(p, {"room_temperature_c": "Gemeten", "room_setpoint_c": "Ingesteld"})
    panels.append(p)
    p = panel(6, "Berekende warmtevraag", "timeseries", 12, 4, 12, 8,
              query(["heat_demand_pct"]), "percent", "PID-uitgang 0–100%, omgerekend uit het keteltemperatuurbereik 5–60 °C. Geen gemeten brandervermogen of kW.")
    p["fieldConfig"]["defaults"].update({"min": 0, "max": 100})
    rename(p, {"heat_demand_pct": "Warmtevraag"})
    panels.append(p)
    p = panel(7, "Gevraagde keteltemperatuur", "timeseries", 0, 12, 12, 8,
              query(["boiler_setpoint_c"]), "celsius", "Berekende OpenTherm-aanvoertemperatuur; geen gemeten watertemperatuur. CV-vrijgave staat in de tijdlijn onderaan.")
    rename(p, {"boiler_setpoint_c": "Gevraagd"})
    panels.append(p)
    dhw = BASE + '''
  |> filter(fn: (r) => r._field == "status_valid" or r._field == "dhw_active" or r._field == "dhw_enabled")
  |> pivot(rowKey: ["_time"], columnKey: ["_field"], valueColumn: "_value")
  |> map(fn: (r) => ({_time: r._time, Activiteit: if r.status_valid and exists r.dhw_active then (if r.dhw_active then 1 else 0) else -1, Vrijgave: if r.dhw_enabled then 1 else 0}))
  |> group()
  |> sort(columns: ["_time"])'''
    p = panel(8, "Warm water: activiteit en vrijgave", "state-timeline", 12, 12, 12, 8, dhw,
              description="Activiteit komt uit het ketelantwoord; vrijgave uit de thermostaatinstelling. Onbekend wordt apart weergegeven. Resolutie circa vijf seconden.")
    states = {"-1": {"text": "Onbekend", "color": "orange"}, "0": {"text": "Uit", "color": "dark-blue"}, "1": {"text": "Aan", "color": "green"}}
    p["fieldConfig"]["defaults"].update({"mappings": [{"type": "value", "options": states}],
        "custom": {"spanNulls": False, "insertNulls": 30000, "lineWidth": 0, "fillOpacity": 85}})
    p["options"] = {"mergeValues": True, "showValue": "auto", "alignValue": "center", "rowHeight": .8,
                    "legend": {"displayMode": "list", "placement": "bottom"}, "tooltip": {"mode": "single"}}
    panels.append(p)
    quality = BASE + '''
  |> filter(fn: (r) => r._field == "status_valid" or r._field == "room_valid" or r._field == "ch_enabled")
  |> map(fn: (r) => ({r with _value: if r._value then 1 else 0}))
  |> keep(columns: ["_time", "_value", "_field"])'''
    p = panel(9, "Datakwaliteit en CV-vrijgave", "state-timeline", 0, 20, 24, 5, quality,
              description="Bij communicatiefouten is de ketelstatus onbekend; een ongeldige sensorwaarde is geen temperatuurmeting.")
    p["fieldConfig"]["defaults"].update({"mappings": [{"type": "value", "options": {"0": {"text": "Nee", "color": "orange"}, "1": {"text": "Ja", "color": "green"}}}],
        "custom": {"spanNulls": False, "insertNulls": 30000, "lineWidth": 0, "fillOpacity": 80}})
    rename(p, {"status_valid": "Geldig ketelantwoord", "room_valid": "Geldige kamertemperatuur", "ch_enabled": "CV vrijgegeven"})
    p["options"] = {"mergeValues": True, "showValue": "auto", "rowHeight": .8, "tooltip": {"mode": "single"}, "legend": {"displayMode": "list", "placement": "bottom"}}
    panels.append(p)
    dashboard = {"id": None, "uid": "room-thermostat-v3", "title": "Kamerthermostaat", "tags": ["thermostaat", "AsusHome"],
                 "timezone": "Europe/Amsterdam", "schemaVersion": 39, "version": 1, "refresh": "30s", "editable": True,
                 "time": {"from": "now-1h", "to": "now"}, "panels": panels, "templating": {"list": []},
                 "description": "Zelfstandige V3-thermostaat; metingen elke tien seconden, warmwaterevents per geldig ketelantwoord. Logging beïnvloedt de regeling niet."}
    target = Path(__file__).resolve().parents[1] / "docs/grafana/kamerthermostaat.json"
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_text(json.dumps(dashboard, ensure_ascii=False, separators=(",", ":")), encoding="utf-8")
    print("Generated", target, "with", len(panels), "panels; no credentials.")


if __name__ == "__main__":
    main()
