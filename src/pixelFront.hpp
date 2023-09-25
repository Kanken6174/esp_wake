#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "charDict.hpp"
#define LEDS_PIN 4
#define LEDS_COUNT 75
#define CHANNEL 0

Freenove_ESP32_WS2812 p = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);

class GridPixel {
protected:
  uint8_t _index = 0;
  uint8_t _r = 0;
  uint8_t _g = 0;
  uint8_t _b = 0;
  bool on = false;

public:
  GridPixel(uint8_t index) {
    _index = index;
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b) {
    _r = r;
    _g = g;
    _b = b;
  }

  void setOn(bool condition) {
    on = condition;
    if (!on)
      p.setLedColorData(_index, 0, 0, 0);
    else
      p.setLedColorData(_index, _r, _g, _b);
  }
};

class GridNumber {
protected:
  GridPixel* pixels[5][3]; // Use parentheses to declare arrays
  bool onMask[5][3];

public:
  GridNumber(uint8_t& startIndex) {
    for (int i = 0; i < 5; i++)
      for (int u = 0; u < 3; u++) { // Use 'u' here instead of 'i'
        pixels[i][u] = new GridPixel(startIndex);
        pixels[i][u]->setColor(1, 0, 0); // Add a semicolon here
        startIndex++;
      }
  }

void setChar(char c) {
  const bool* charMask = nullptr; // Pointer to the character mask

  switch (c) {
    case '0': charMask = (const bool *)CZERO; break;
    case '1': charMask = (const bool *)CONE; break;
    case '2': charMask = (const bool *)CTWO; break;
    case '3': charMask = (const bool *)CTHREE; break;
    case '4': charMask = (const bool *)CFOUR; break;
    case '5': charMask = (const bool *)CFIVE; break;
    case '6': charMask = (const bool *)CSIX; break;
    case '7': charMask = (const bool *)CSEVEN; break;
    case '8': charMask = (const bool *)CEIGHT; break;
    case '9': charMask = (const bool *)CNINE; break;
    case ' ': charMask = (const bool *)CNINE; break;
    default: charMask = (const bool *)CEMPTY; break;
  }

  // Apply the character mask to the onMask array
  for (int i = 0; i < 5; i++) {
    for (int u = 0; u < 3; u++) {
      onMask[i][u] = charMask[i * 3 + (u==0 ? 2 : (u==1) ? 1 : 0)]; //weird correction but it works
    }
  }

  // Set the pixels based on the updated onMask
  for (int i = 0; i < 5; i++) {
    for (int u = 0; u < 3; u++) {
      pixels[i][u]->setOn(onMask[i][u]);
      }
    }
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b){
    for (int i = 0; i < 5; i++) {
      for (int u = 0; u < 3; u++) {
        pixels[i][u]->setColor(r,g,b);
        }
      }
    }
};


class GridSeparator {
protected:
  GridPixel* pixels[5]; // Use parentheses to declare arrays
  bool onMask[5];

public:
  GridSeparator(uint8_t& startIndex) {
    for (int i = 0; i < 5; i++){
        pixels[i] = new GridPixel(startIndex);
        pixels[i]->setColor(1, 0, 0);
        startIndex++;
    }
  }

  void toggleDots(bool on){
    if(on){
      pixels[1]->setOn(true);
      pixels[3]->setOn(true);
    }else{
      for (int i = 0; i < 5; i++){
        pixels[i]->setOn(false);
      }
    }
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b){
    for (int i = 0; i < 5; i++)
      pixels[i]->setColor(r, g, b);
  }
};

class GridMaster {
protected:
  GridNumber* g1;
  GridNumber* g2;
  GridNumber* g3;
  GridNumber* g4;
  GridSeparator* gs;

  unsigned long previousMillis = 0;
  uint64_t lastUpdateMillis = 0;
  const unsigned long interval = 1000;
  bool tg = false;

  uint8_t hours = 0;
  uint8_t minutes = 0;
  uint8_t seconds = 0;
  uint8_t dayOfWeek = 0; //1->7

  bool _sun = false;

public:
  uint8_t Chours = 0;
  uint8_t Cminutes = 0;
  uint8_t Cseconds = 0;

  GridMaster() {
    uint8_t startIndex = 0;
    g1 = new GridNumber(startIndex);
    g2 = new GridNumber(startIndex);
    gs = new GridSeparator(startIndex);
    g3 = new GridNumber(startIndex);
    g4 = new GridNumber(startIndex);
    p.begin();
    p.setBrightness(1);
  }

  void setTime(uint8_t hh, uint8_t mm, uint8_t ss) {
    lastUpdateMillis = millis();
    hours = hh;
    minutes = mm;
    seconds = ss;
  }

  void process() {
    unsigned long ms = millis();

    // Check if it's time to run the code
    if (ms - previousMillis >= interval) {
      previousMillis = ms; // Save the last execution time
      uint64_t timeElapsedSinceLastTimeSet = ms - lastUpdateMillis;

      uint64_t _Cseconds = timeElapsedSinceLastTimeSet / 1000;  // 1000 milliseconds = 1 second
      timeElapsedSinceLastTimeSet -= _Cseconds * 1000; //remove seconds
      _Cseconds += seconds;  //add calibrated seconds
      if(_Cseconds >= 60){  //remove any excess
        timeElapsedSinceLastTimeSet += (floor(_Cseconds/60)*(1000*60));
        _Cseconds %= 60;
      }
      
      uint64_t _Cminutes = timeElapsedSinceLastTimeSet / 60000;  // 60000 milliseconds = 1 minute
      timeElapsedSinceLastTimeSet -= _Cminutes * 60000;  //remove minutes
      _Cminutes += minutes;  //add calibrated minutes
      if(_Cminutes >= 60){ //remove excess
        timeElapsedSinceLastTimeSet += (floor(_Cminutes/60)*(60*60*1000));
        _Cminutes %= 60;
      }
      
      uint64_t _Chours = timeElapsedSinceLastTimeSet / 3600000;  // 3600000 milliseconds = 1 hour
      timeElapsedSinceLastTimeSet -= _Chours * 3600000;
      _Chours += hours;  //add calibrated hours
      if(_Chours >= 24){
        _Chours %= 24;
      }

      Chours = _Chours;
      Cminutes = _Cminutes;
      Cseconds = _Cseconds;
      
      displayTime();
      gs->toggleDots(tg);
      p.show();

      tg = !tg;
    }
  }

  void displayTime() {
    if(_sun){
      g4->setChar(' ');
      g3->setChar(' ');
      g2->setChar(' ');
      g1->setChar(' ');
    return;
    }
    // Convert hours, minutes, and seconds to characters

    hardSetNumbers(Chours, Cminutes);
  }

  void hardSetNumbers(uint8_t n1, uint8_t n2){
      char hours_digits[3];   // 2 characters for hours
      char minutes_digits[3]; // 2 characters for minutes

      snprintf(hours_digits, sizeof(hours_digits), "%02d", n1);
      snprintf(minutes_digits, sizeof(minutes_digits), "%02d", n2);

      // Display the time on the grid
      g4->setChar(hours_digits[0]);
      g3->setChar(hours_digits[1]);
      g2->setChar(minutes_digits[0]);
      g1->setChar(minutes_digits[1]);
  }

  void setColor(uint8_t r, uint8_t g, uint8_t b){
    g1->setColor(r,g,b);
    g2->setColor(r,g,b);
    g3->setColor(r,g,b);
    g4->setColor(r,g,b);
    gs->setColor(r,g,b);
  }

  void sun(){
    setColor(252,229,112);
    _sun = true;
  }
};