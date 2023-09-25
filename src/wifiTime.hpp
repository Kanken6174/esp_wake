#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ctime>

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

void setGTimeFromNTP(int& hours, int& minutes, int& seconds, int& dayOfWeek, int& dayOfMonth, int& month, int& year) {
    // Get the current time from the timeClient
    unsigned long epochTime = timeClient.getEpochTime();

    // Calculate current hours, minutes, seconds
    seconds = epochTime % 60;
    epochTime /= 60;
    minutes = epochTime % 60;
    epochTime /= 60;
    hours = epochTime % 24;
    
    // Convert epochTime to days since 1970-01-01 (Unix epoch)
    unsigned long daysSinceEpoch = epochTime / 86400; // 86400 seconds in a day

    // Calculate day of the week (0 = Sunday, 1 = Monday, etc.)
    dayOfWeek = (daysSinceEpoch + 4) % 7; // January 1, 1970, was a Thursday (4)

    // Calculate day of the month and year
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int yearCounter = 1970;

    while (true) {
        int daysInYear = 365;
        if ((yearCounter % 4 == 0 && yearCounter % 100 != 0) || (yearCounter % 400 == 0)) {
            // Leap year, 366 days
            daysInYear = 366;
        }

        if (daysSinceEpoch < daysInYear) {
            year = yearCounter;
            break;
        }

        daysSinceEpoch -= daysInYear;
        yearCounter++;
    }

    // Determine the month and day within that year
    for (int i = 0; i < 12; i++) {
        if (daysSinceEpoch < daysInMonth[i]) {
            month = i + 1;
            dayOfMonth = daysSinceEpoch + 1; // Days are 1-based
            break;
        }

        daysSinceEpoch -= daysInMonth[i];
    }
}

private:
  String _ssid;
  String _password;
  WiFiUDP udp;
  NTPClient timeClient;
};