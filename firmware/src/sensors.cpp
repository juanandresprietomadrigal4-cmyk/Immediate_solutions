#include "sensors.h"

#include <Arduino.h>
#include <DHT.h>

#include "config_pins.h"

namespace {

DHT dht(PIN_DHT22, DHT22);
SensorReadings latest;
unsigned long lastReadMs = 0;

float mq135ToPpmEst(int rawAdc) {
  // Estimación lineal simple para demo; calibrar con aire limpio / CO2 conocido.
  const float vRef = 3.3f;
  const float adcMax = 4095.0f;
  float volts = (rawAdc / adcMax) * vRef;
  float ppm = (volts / vRef) * 2000.0f;
  if (ppm < 0.0f) {
    ppm = 0.0f;
  }
  return ppm;
}

void fillSimulated(SensorReadings& out) {
  out.temperatureC = 20.0f + (random(0, 1400) / 100.0f);
  out.humidityPct = 45.0f + (random(0, 4300) / 100.0f);
  out.gasPpmEst = 300.0f + random(0, 1400);
  out.simulated = true;
}

}  // namespace

void sensorsBegin() {
  randomSeed(analogRead(PIN_MQ135));
  dht.begin();
  pinMode(PIN_MQ135, INPUT);
  latest = SensorReadings{};
  sensorsUpdate();
}

void sensorsUpdate() {
  unsigned long now = millis();
  if (now - lastReadMs < SENSOR_POLL_MS) {
    return;
  }
  lastReadMs = now;

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int mqRaw = analogRead(PIN_MQ135);

  bool dhtOk = !isnan(t) && !isnan(h);
  bool mqOk = mqRaw > 0;

  if (dhtOk && mqOk) {
    latest.temperatureC = t;
    latest.humidityPct = h;
    latest.gasPpmEst = mq135ToPpmEst(mqRaw);
    latest.simulated = false;
    return;
  }

  fillSimulated(latest);
}

SensorReadings sensorsGet() {
  return latest;
}
