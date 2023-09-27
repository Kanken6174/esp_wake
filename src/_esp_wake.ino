#include <Wire.h>
#include "pixelFront.hpp"
#include "wifiTime.hpp"
#include "Tone.hpp"
#include "AGS10.hpp"
#include "ATH20.hpp"
#include "I2C_utils.hpp"
#include <cmath>
#include "Alarms.hpp"
#define VAL 255
#define TEMP_OFFSET 5.0f
#include <esp_adc_cal.h>
#include "extScreen.hpp"
#include "web.hpp"

#define ADC_WIDTH          12 // ADC resolution
#define ADC_ATTENUATION    ADC_11db // Attenuation for full-scale voltage
#define ADC_MAX_VALUE      4095 // Maximum ADC value for 12-bit resolution

#include "StringUtils.hpp"

AGS10 ags10;

Adafruit_AHTX0 aht;

GridMaster g;

DacToneGenerator toneGenerator;

EspSD esd;

WiFiTimeClient timeClient;

AlarmManager alarmManager;

ExtScreen extScreen;

WebConfigServer web1(esd, alarmManager);

void setup() {
  Serial.begin(9600);
  delay(5000);
  Serial.println("\nInit I2C...");

  Wire.begin(33, 34);

  Serial.println("done!");
  scanI2CDevices();

  esd.begin();

  String str = esd.readFile(SD,"/config/wifiCredentials.kcv");
  std::vector<String> lines = splitStringBy(str, '\n');
  for(auto a : lines){
    std::vector<String> keyValuePair= splitStringBy(a,';');
    timeClient.setSSidPass(keyValuePair.at(0),keyValuePair.at(1));
    if(timeClient.connect()){
        timeClient.update();
        break;  //success
    }
  }
  timeClient.setGTimeFromNTP();

  //print full date from timeLib
  Serial.print("Time set to: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.print(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.println(year());
  Serial.print("Day of week: ");
  Serial.println(weekday());

  
  g.setColor(VAL,0,0);

  ags10.begin();
  aht.begin();

  pinMode(38, INPUT_PULLUP);
  pinMode(37, INPUT_PULLUP);

  pinMode(9,ANALOG);

  alarmManager.begin(esd);
  extScreen.begin();
  g.snooze();
  web1.begin();
  ColorTVOCSet();

  //print ip address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

uint64_t lastAGS10 = 0;
uint64_t lastSwitch = 0;
uint16_t AGS10Delay = 1600;
uint16_t switchDelay = 3000;
sensors_event_t humidity, temp;

bool on = true;
int tvoc = 0;

void ColorTVOCSet(){
    if(g._sun)
      return;

      if(!on)
      g.setColor(0,0,0);
    else if(tvoc < 220)
      g.setColor(0,VAL,0);
    else if (tvoc < 650)
      g.setColor(VAL,VAL,0);
    else if (tvoc < 1430)
      g.setColor(VAL,0,0);
    else if (tvoc < 2200)
      g.setColor(0,0,VAL);
    else if (tvoc < 3300)
      g.setColor(VAL,0,VAL);
    else
      g.setColor(VAL,VAL,VAL);
}

void loop() {
  if(millis()-lastAGS10 > AGS10Delay){
    lastAGS10 = millis();
    tvoc = ags10.readTVOC();
    esd.logData(SD,"tvoc.txt",tvoc,hour(),minute(),second());
    ColorTVOCSet();

    aht.getEvent(&humidity, &temp);
    temp.temperature -= TEMP_OFFSET;
    Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
    Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

    esd.logData(SD,"temperature.txt",temp.temperature,hour(),minute(),second());
    esd.logData(SD,"humidity.txt",humidity.relative_humidity,hour(),minute(),second());

    if(alarmManager.process()){
      g.sun();
    }
    extScreen.clear();
    //print full day, week day (letters) and month + year from on top line, and sensor data on bottom line
    extScreen.displayText("    "+String(day()) + "/" + String(month()) + "/" + String(year()), 0, 0);
    extScreen.displayText("T:" + String(temp.temperature) + "C H:" + String(humidity.relative_humidity) + "%\nTVOC:" + String(tvoc) +" ppb", 0, 10);

    web1.handleClient();
  }

  if(!digitalRead(38)){
    if(!g._sun){
      on = false;
      extScreen.enabled = false;
      g.setColor(0,0,0);
    }else{
        g.snooze();
        on = true;
        ColorTVOCSet();
    }
  }
  if(!digitalRead(37)){
    if(!g._sun){
    on = true;
    extScreen.enabled = true;
    ColorTVOCSet();
    }else{
        g.snooze();
        on = true;
        ColorTVOCSet();
    }
  }
    g.process();
}