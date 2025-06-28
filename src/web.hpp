#pragma once

#include <WebServer.h>
#include <ArduinoJson.h>
#include "Alarms.hpp"

class WebConfigServer {
private:
  WebServer server;
  EspSD& sd; 
  AlarmManager& am;
  float ct = 0, ch = 0; int cv = 0;

public:
  WebConfigServer(EspSD& esd, AlarmManager& alarmManager);
  void begin();
  void handleClient();
  void updateSensorData(float t, float h, int v);
};
