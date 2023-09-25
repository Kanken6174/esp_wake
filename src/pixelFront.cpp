#include "pixelFront.hpp"
#include <TimeLib.h>

Freenove_ESP32_WS2812 _pL = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

void GridMaster::displayTime()
{
    if(_sun){
      g4->setChar(' ');
      g3->setChar(' ');
      g2->setChar(' ');
      g1->setChar(' ');
      _pL.setBrightness(255);
    return;
    }
    _pL.setBrightness(1);
    hardSetNumbers(hour(), minute());
  }

void GridMaster::process(){
    unsigned long ms = millis();

    // Check if it's time to run the code
    if (ms - previousMillis >= interval) {
      previousMillis = millis();
      //print time in one line, format hh:mm:ss
      Serial.print(hour());
      Serial.print(":");
      Serial.print(minute());
      Serial.print(":");
      Serial.println(second());
      Serial.println("GridMaster::process()");
      displayTime();
      gs->toggleDots(tg);
      _pL.show();

      tg = !tg;
    }
}