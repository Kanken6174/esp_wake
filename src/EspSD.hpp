#include "esp32-hal-gpio.h"
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <map>

const uint8_t SPI_SS_PIN    = 11;
const uint8_t SPI_MOSI_PIN  = 10;
const uint8_t SPI_MISO_PIN  = 7;
const uint8_t SPI_SCK_PIN   = 8;

const uint8_t SD_DETECT = 12;

class EspSD{
  File config;
  SPIClass Spi02 = SPIClass(HSPI);
protected:
  std::map<String, String> stringConfigs;
  std::map<String, float> floatConfigs;
  std::map<String, int> intConfigs;
  std::map<String, bool> boolConfigs;


public:
  EspSD(){}
  void begin(){
    Spi02.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SS_PIN);

    pinMode(SD_DETECT, INPUT_PULLDOWN);
    if(!digitalRead(SD_DETECT)){
      Serial.println("No SD card inserted?");
      while(1){delay(10);}
    }else {
      Serial.println("Sd card inserted!");
    }
    Serial.println(MOSI);
    Serial.println(MISO);
    Serial.println(SS);
    Serial.println(SCK);

    if(!SD.begin(SPI_SS_PIN,Spi02)){
      Serial.println("Card Mount Failed");
      while(1){}
    }
    Serial.println("Card mount successful");

    Serial.println("\nFiles found on the card (name, date and size in bytes): ");
    listDir(SD, "/", 5);

    if(!folderExists(SD, "/config"))
      createDir(SD,"/config");
    if(!fileExists(SD, "/config/config.kcv"))
      createFile(SD,"/config/config.kcv");

    if(!folderExists(SD, "/logging"))
      createDir(SD,"/logging");
    if(!fileExists(SD, "/logging/temperature.txt"))
      createFile(SD,"/logging/temperature.txt");
    if(!fileExists(SD, "/logging/humidity.txt"))
      createFile(SD,"/logging/humidity.txt");
    if(!fileExists(SD, "/logging/tvoc.txt"))
      createFile(SD,"/logging/tvoc.txt");
    if(!fileExists(SD, "/logging/light.txt"))
      createFile(SD,"/logging/light.txt");
    if(!fileExists(SD, "/logging/accelerometer.txt"))
      createFile(SD,"/logging/accelerometer.txt");
  }

  void listDir(fs::FS &fs, const String& dirname, uint8_t levels);
  void createDir(fs::FS &fs, const String& path);
  void createFile(fs::FS &fs, const String& path);
  void removeDir(fs::FS &fs, const String& path);
  String readFile(fs::FS &fs, const String& path);
  void writeFile(fs::FS &fs, const String& path, const String& message);
  void appendFile(fs::FS &fs, const String& path, const String& message);
  void renameFile(fs::FS &fs, const String& path1, const String& path2);
  void deleteFile(fs::FS &fs, const String& path);
  bool folderExists(fs::FS &fs, const String& path);
  bool fileExists(fs::FS &fs, const String& path);

  void logData(fs::FS &fs,const String& fileName, float data, uint8_t hh, uint8_t mm, uint8_t ss);
  void logData(fs::FS &fs,const String& fileName, int data, uint8_t hh, uint8_t mm, uint8_t ss);

  //bool updateOrAppendKSV(fs::FS& file, const String& key, const String& value, String& updatedContent);

  bool readKeyValueConfig(const String& key, String& out);
  bool readKeyValueConfig(const String& key, float& out);
  bool readKeyValueConfig(const String& key, int& out);
  bool readKeyValueConfig(const String& key, bool& out);

  /*bool writeKeyValueConfig(const String& key, const String& value);
  bool writeKeyValueConfig(const String& key, const float& value);
  bool writeKeyValueConfig(const String& key, const int& value);
  bool writeKeyValueConfig(const String& key, const bool& value);*/
};