#include <WiFi.h>
#include <WebServer.h>

class EspSD;
class AlarmManager;

class WebConfigServer {
private:
  WebServer server;
  EspSD& sd; // Initialize an instance of your EspSD class
  AlarmManager& am;
public:
  WebConfigServer(EspSD& esd, AlarmManager& alarmManager) : sd(esd), am(alarmManager) {}

  void begin();

  void handleClient() {
    server.handleClient();
  }
};