#pragma once

struct SensorReadings {
  float temperatureC = NAN;
  float humidityPct = NAN;
  float gasPpmEst = NAN;
  bool simulated = true;
};

void sensorsBegin();
void sensorsUpdate();
SensorReadings sensorsGet();
