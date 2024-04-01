#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>

#define timeZone 1

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

  
  bool isDST() {
    time_t now = timeClient.getEpochTime();
    tmElements_t localTime;
    breakTime(now, localTime);

    int beginDSTDay = (31 - (5 * localTime.Year / 4 + 4) % 7);
    int beginDSTMonth = 3;
    int endDSTDay = (31 - (5 * localTime.Year / 4 + 1) % 7);
    int endDSTMonth = 10;

    if (localTime.Month > beginDSTMonth && localTime.Month < endDSTMonth) {
      return true;  // DST is in effect
    }
    if (localTime.Month == beginDSTMonth && localTime.Day >= beginDSTDay) {
      return true;  // DST starts
    }
    if (localTime.Month == endDSTMonth && localTime.Day < endDSTDay) {
      return true;  // DST ends
    }
    return false;
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
    int timeZoneOffsetInSeconds = 3600 * 2; // Regular time zone offset (e.g., 2 hours)
    if (!+isDST()) {
      timeZoneOffsetInSeconds += 3600; // Adjust for DST
    }

    timeClient.setTimeOffset(timeZoneOffsetInSeconds);
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