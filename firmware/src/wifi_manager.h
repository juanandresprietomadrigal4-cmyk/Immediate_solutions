#pragma once

#include <Arduino.h>

enum class WifiModeKind { ApSetup, StaConnected, StaConnecting };

void wifiBegin();
WifiModeKind wifiModeKind();
String wifiIpAddress();
String wifiStoredSsid();
bool wifiSaveCredentials(const String& ssid, const String& pass);
void wifiScheduleRestart();
void wifiServiceLoop();
