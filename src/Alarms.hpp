#include "EspSD.hpp"
#include <vector>

class Alarm{
  public:
    int hour;
    int minute;
    int second;
    int day;  //1-7 week days; 0 = any day; 8 - any week day; 9 - any weekend day
    bool active;
    bool ringing;
};

class AlarmManager{
  protected:
    std::vector<Alarm> alarms;
    EspSD& _sd;
  public:
    void begin(const EspSD& sd); // load alarms from SD
    bool process(); //check alarms, true if alarm should be ringing
    void snooze();
};