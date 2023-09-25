#include "Freenove_WS2812_Lib_for_ESP32.h"
#include "charDict.hpp"
#define LEDS_PIN 4
#define LEDS_COUNT 75
#define CHANNEL 0

extern Freenove_ESP32_WS2812 _pL;

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
      _pL.setLedColorData(_index, 0, 0, 0);
    else
      _pL.setLedColorData(_index, _r, _g, _b);
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

public:

  bool _sun = false;

  GridMaster() {
    uint8_t startIndex = 0;
    g1 = new GridNumber(startIndex);
    g2 = new GridNumber(startIndex);
    gs = new GridSeparator(startIndex);
    g3 = new GridNumber(startIndex);
    g4 = new GridNumber(startIndex);
    _pL.begin();
    _pL.setBrightness(1);
  }

  void process();

  void displayTime();

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

  void snooze(){
    setColor(255,0,0);
    _sun = false;
  }
};