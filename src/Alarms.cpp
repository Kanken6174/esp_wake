#include "Alarms.hpp"
#include "StringUtils.hpp"
#include <TimeLib.h>

void AlarmManager::reloadAlarms(){
  alarms.clear();
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
        //check if alarm is in the past, if so, set it to inactive (older)
        if(alarm.day <= day()){
          if(alarm.hour <= hour()){
            if(alarm.minute <= minute()){
              if(alarm.second <= second()){
                continue; //skip this alarm
              }
            }
          }
        }

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

void AlarmManager::begin(const EspSD& sd){
  //load alarms from SD card
  _sd = sd;
  reloadAlarms();
}

bool AlarmManager::process(){
  int _day = day();
  int _hour = hour();
  int _minute = minute();
  int _second = second();
  bool shouldRing = false;
  
  //check all alarms, if their time is between previous time and current time, and are active, they should ring
  for(Alarm alarm : alarms){
    if(alarm.active){
      if(alarm.day == 0 || alarm.day == _day || alarm.day == 8 || alarm.day == 9){
        if(alarm.hour >= previousHour && alarm.hour <= _hour){
          if(alarm.minute >= previousMinute && alarm.minute <= _minute){
            if(alarm.second >= previousSecond && alarm.second <= _second){
              alarm.ringing = true;
              shouldRing = true;
              alarm.active = false;
              break;
            }
          }
        }
      }
    }
  }
  previousDay = _day;
  previousHour = _hour;
  previousMinute = _minute;

  return shouldRing;
};