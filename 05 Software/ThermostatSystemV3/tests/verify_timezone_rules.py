"""Validate firmware POSIX transition policy against IANA Europe/Amsterdam.

Tests the rule/calendar, not the ESP32 C library implementation.
Requires Python's zoneinfo database (or tzdata package on Windows).
"""
from pathlib import Path
from datetime import datetime, timedelta, timezone
from zoneinfo import ZoneInfo
import calendar
import re

text = (Path(__file__).resolve().parents[1] / 'thermostat/src/timezone_rules.h').read_text()
rule = re.search(r'AMSTERDAM_TZ\[\] = "([^"]+)"', text)[1]
m = re.fullmatch(r'CET-1CEST,M(\d+)\.5\.0/(\d+),M(\d+)\.5\.0/(\d+)', rule)
assert m, rule
sm, sh, em, eh = map(int, m.groups())
zone = ZoneInfo('Europe/Amsterdam')
utc = timezone.utc
checked = 0
for year in range(2024, 2031):
    def transition(month, hour, offset):
        day = calendar.monthrange(year, month)[1]
        d = datetime(year, month, day, hour, tzinfo=utc)
        d -= timedelta(days=(d.weekday()+1)%7)
        return d-timedelta(hours=offset)
    start, end = transition(sm, sh, 1), transition(em, eh, 2)
    points = [datetime(year,1,15,tzinfo=utc), datetime(year,7,15,tzinfo=utc)]
    points += [t+timedelta(seconds=s) for t in (start,end) for s in (-1,0,1)]
    for instant in points:
        expected = 2 if start <= instant < end else 1
        assert instant.astimezone(zone).utcoffset() == timedelta(hours=expected), instant
        checked += 1
assert datetime(2026,10,23,12,tzinfo=utc).astimezone(zone).hour == 14
print(f'{checked} winter/summer/transition cases passed for 2024–2030; 23 October 2026 is UTC+2')
