#include "wifi_manager.h"

#include <Preferences.h>
#include <WiFi.h>

#include "config_pins.h"

namespace {

Preferences prefs;
bool restartPending = false;
unsigned long restartAtMs = 0;
WifiModeKind mode = WifiModeKind::StaConnecting;

void startAp() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASS);
  mode = WifiModeKind::ApSetup;
}

bool connectSta(const String& ssid, const String& pass) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  mode = WifiModeKind::StaConnecting;

  const unsigned long timeoutMs = 15000;
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
    delay(250);
  }
  if (WiFi.status() == WL_CONNECTED) {
    mode = WifiModeKind::StaConnected;
    return true;
  }
  return false;
}

}  // namespace

void wifiBegin() {
  prefs.begin("greenhouse", false);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");

  if (ssid.isEmpty()) {
    startAp();
    return;
  }

  if (!connectSta(ssid, pass)) {
    startAp();
  }
}

WifiModeKind wifiModeKind() {
  if (mode == WifiModeKind::StaConnecting && WiFi.status() == WL_CONNECTED) {
    mode = WifiModeKind::StaConnected;
  }
  return mode;
}

String wifiIpAddress() {
  if (wifiModeKind() == WifiModeKind::ApSetup) {
    return WiFi.softAPIP().toString();
  }
  if (WiFi.status() == WL_CONNECTED) {
    return WiFi.localIP().toString();
  }
  return "";
}

String wifiStoredSsid() {
  return prefs.getString("ssid", "");
}

bool wifiSaveCredentials(const String& ssid, const String& pass) {
  if (ssid.isEmpty()) {
    return false;
  }
  prefs.putString("ssid", ssid);
  if (!pass.isEmpty()) {
    prefs.putString("pass", pass);
  }
  return true;
}

void wifiScheduleRestart() {
  restartPending = true;
  restartAtMs = millis() + 1500;
}

void wifiLoop() {
  if (restartPending && millis() >= restartAtMs) {
    ESP.restart();
  }
}

void wifiServiceLoop() {
  wifiLoop();
}
