#include "http_api.h"

#include <ArduinoJson.h>

#include "sensors.h"
#include "wifi_manager.h"

extern const uint8_t embed_index_html_start[] asm("_binary_embed_index_html_start");
extern const uint8_t embed_index_html_end[] asm("_binary_embed_index_html_end");

namespace {

WebServer* http = nullptr;

const char* modeToString(WifiModeKind kind) {
  switch (kind) {
    case WifiModeKind::ApSetup:
      return "ap";
    case WifiModeKind::StaConnected:
      return "sta";
    default:
      return "connecting";
  }
}

template <typename TDoc>
void sendJson(int code, const TDoc& doc) {
  String out;
  serializeJson(doc, out);
  http->send(code, "application/json", out);
}

void handleRoot() {
  const size_t len = embed_index_html_end - embed_index_html_start;
  http->sendHeader("Content-Type", "text/html; charset=utf-8", true);
  http->sendHeader("Connection", "close", true);
  http->sendHeader("Content-Length", String(len), true);
  http->send(200);
  http->client().write(embed_index_html_start, len);
}

void handleSensors() {
  sensorsUpdate();
  SensorReadings r = sensorsGet();

  StaticJsonDocument<256> doc;
  doc["t"] = round(r.temperatureC * 10.0f) / 10.0f;
  doc["h"] = round(r.humidityPct * 10.0f) / 10.0f;
  doc["g"] = round(r.gasPpmEst);
  doc["simulated"] = r.simulated;
  sendJson(200, doc);
}

void handleStatus() {
  SensorReadings r = sensorsGet();
  StaticJsonDocument<384> doc;
  doc["online"] = true;
  doc["mode"] = modeToString(wifiModeKind());
  doc["simulated"] = r.simulated;
  doc["ip"] = wifiIpAddress();
  doc["ssid"] = wifiStoredSsid();
  sendJson(200, doc);
}

void handleWifiGet() {
  String ssid = wifiStoredSsid();
  StaticJsonDocument<192> doc;
  doc["ssid"] = ssid;
  doc["configured"] = !ssid.isEmpty();
  sendJson(200, doc);
}

void handleWifiPost() {
  if (!http->hasArg("plain")) {
    http->send(400, "application/json", "{\"error\":\"body required\"}");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError err = deserializeJson(doc, http->arg("plain"));
  if (err) {
    http->send(400, "application/json", "{\"error\":\"invalid json\"}");
    return;
  }

  const char* ssid = doc["ssid"] | "";
  const char* pass = doc["pass"] | "";
  if (strlen(ssid) == 0) {
    http->send(400, "application/json", "{\"error\":\"ssid required\"}");
    return;
  }

  if (!wifiSaveCredentials(String(ssid), String(pass))) {
    http->send(500, "application/json", "{\"error\":\"save failed\"}");
    return;
  }

  StaticJsonDocument<256> out;
  out["ok"] = true;
  out["ssid"] = ssid;
  out["message"] = "Guardado. Reiniciando ESP32…";
  sendJson(200, out);
  wifiScheduleRestart();
}

void handleNotFound() {
  http->send(404, "application/json", "{\"error\":\"not found\"}");
}

}  // namespace

void httpApiBegin(WebServer& server) {
  http = &server;
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/sensors", HTTP_GET, handleSensors);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/wifi", HTTP_GET, handleWifiGet);
  server.on("/api/wifi", HTTP_POST, handleWifiPost);
  server.onNotFound(handleNotFound);
}
