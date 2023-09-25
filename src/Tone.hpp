#include <Arduino.h>

class DacToneGenerator {
public:
  DacToneGenerator() {}

  // Initialize the DAC and set the output pin
  void begin() {
    pinMode(17,OUTPUT);
  }

  // Generate and output a tone with a given frequency (in Hz) and duration (in milliseconds)
  void playTone(uint8_t frequency, unsigned long duration) {
    unsigned long startTime = millis();
    while(millis() < startTime + duration){
      dacWrite(17, 0);
      dacWrite(17, 100);
    }
    delay(duration);
    dacDisable(17); // Turn off the DAC after the tone
  }
};