#pragma once
// Copy to influx_config.h (ignored by Git). Use a token with Write only on this bucket.
#define THERMOSTAT_INFLUX_ENABLED 1
#define THERMOSTAT_INFLUX_URL "http://192.168.178.63:8086/api/v2/write?org=Zonstraat35&bucket=Zonstraat35&precision=ms"
#define THERMOSTAT_INFLUX_TOKEN ""
