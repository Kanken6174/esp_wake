#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define timeZone 0

class WiFiTimeClient {
public:
  WiFiTimeClient() :udp(), timeClient(udp) {}

  void setSSidPass(String Nssid, String Npasswd){
    Serial.print("set credentials Nssid: ");
    Serial.print(Nssid);
    Serial.print(", Npasswd: ");
    Serial.println(Npasswd);
    _ssid = Nssid;
    _password = Npasswd;
  }

  bool connect() {
    // Connect to Wi-Fi
    Serial.print("Connecting with credentials ssid: ");
    Serial.print(_ssid);
    Serial.print(", password: ");
    Serial.println(_password);
    WiFi.begin(_ssid.c_str(), _password.c_str());
    int i = 0;

    while (WiFi.status() != WL_CONNECTED) {
      i++;
      delay(500);
      Serial.println("Connecting to WiFi...");
      if(i > 3) return false;
    }
    Serial.println("Connected to WiFi");

    // Initialize NTP Client
    timeClient.begin();
    timeClient.setTimeOffset(3600*2);  // Timezone offset in seconds (1 hour = 3600 seconds)
    return true;
  }

  void update() {
    timeClient.update();
  }

  String getFormattedTime() {
    return timeClient.getFormattedTime();
  }

void setGTimeFromNTP() {
    unsigned long time_epoch = timeClient.getEpochTime()- (timeZone * SECS_PER_HOUR);
    Serial.print("time_epoch: ");
    Serial.println(time_epoch);
    if(time_epoch > 0)
      setTime(time_epoch);
}

private:
  String _ssid;
  String _password;
  WiFiUDP udp;
  NTPClient timeClient;
};