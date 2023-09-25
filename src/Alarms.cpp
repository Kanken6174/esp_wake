#include "Alarms.hpp"
#include "StringUtils.hpp"

void AlarmManager::begin(const EspSD& sd){
  //load alarms from SD card
  _sd = sd;
  String fileData = _sd.readFile(SD, "/config/alarms.kcv");
  std::vector<String> alarmLines = splitStringBy(fileData, '\n');
  for(String line : alarmLines){
    std::vector<String> parts = splitStringBy(line, ';');
    if(parts.size() == 5){
      try{
        Alarm alarm;
        alarm.day = parts[0].toInt();
        alarm.hour = parts[1].toInt();
        alarm.minute = parts[2].toInt();
        alarm.second = parts[3].toInt();
        alarm.active = parts[4].toInt();
        alarm.ringing = false;
        alarms.push_back(alarm);
        Serial.print("Alarm loaded:");
        Serial.print(alarm.day);
        Serial.print(";");
        Serial.print(alarm.hour);
        Serial.print(";");
        Serial.print(alarm.minute);
        Serial.print(";");
        Serial.print(alarm.second);
        Serial.print(";");
        Serial.println(alarm.active);
      }catch(...){}
    }
  }
}