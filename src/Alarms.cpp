#include "Alarms.hpp"

void AlarmManager::begin(const EspSD& sd){
  //load alarms from SD card
  _sd = sd;
}