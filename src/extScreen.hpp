#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET    -1  // Reset pin (use -1 if not used)
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define I2C_SDA_PIN   5   // Custom SDA pin
#define I2C_SCL_PIN   6   // Custom SCL pin
#define SCREEN_ADDRESS 0x3C

class ExtScreen {
protected:
  TwoWire myWire = TwoWire(1);
  Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &myWire, OLED_RESET);
  bool present = true;
public:
  bool enabled = true;

  ExtScreen() {
  }

  void begin() {
    if (!myWire.begin(I2C_SDA_PIN, I2C_SCL_PIN)) {
      Serial.println("I2C init failed");
      present = false;
    }
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      present = false;
    }
    display.display();
    delay(2000);
    display.clearDisplay();
  }

  void clear() {
    if(!present){ 
      Serial.println("Screen not present, not erasing");
      return;
    }
    display.clearDisplay();
    display.display();
  }

  void displayText(String text, int x, int y) {
    if(!present || !enabled){ 
      Serial.println("Screen not enabled");
      return;
    }
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(x, y);
    display.print(text);
    display.display();
  }
};