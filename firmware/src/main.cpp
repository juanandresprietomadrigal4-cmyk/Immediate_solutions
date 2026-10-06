#include <WebServer.h>

#include "http_api.h"
#include "sensors.h"
#include "wifi_manager.h"

WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("Invernadero — arranque");

  sensorsBegin();
  wifiBegin();
  httpApiBegin(server);
  server.begin();

  Serial.print("IP: ");
  Serial.println(wifiIpAddress());
}

void loop() {
  server.handleClient();
  sensorsUpdate();
  wifiServiceLoop();
}
