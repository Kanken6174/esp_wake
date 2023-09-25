#include "EspSD.hpp"
#include "StringUtils.hpp"

void EspSD::listDir(fs::FS &fs, const String& dirname, uint8_t levels){
      Serial.printf("Listing directory: %s\n", dirname.c_str());

      File root = fs.open(dirname);
      if(!root){
          Serial.println("Failed to open directory");
          return;
      }
      if(!root.isDirectory()){
          Serial.println("Not a directory");
          return;
      }

      File file = root.openNextFile();
      while(file){
          if(file.isDirectory()){
              Serial.print("  DIR : ");
              Serial.println(file.name());
              if(levels){
                  listDir(fs, file.path(), levels -1);
              }
          } else {
              Serial.print("  FILE: ");
              Serial.print(file.name());
              Serial.print("  SIZE: ");
              Serial.println(file.size());
          }
          file = root.openNextFile();
      }
  }

void EspSD::createDir(fs::FS &fs, const String& path){
    Serial.printf("Creating Dir: %s\n", path.c_str());
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void EspSD::createFile(fs::FS &fs, const String& path){
      Serial.printf("Creating file: %s\n", path.c_str());
      writeFile(SD, path, "");
}

void EspSD::removeDir(fs::FS &fs, const String& path){
    Serial.printf("Removing Dir: %s\n", path.c_str());
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

String EspSD::readFile(fs::FS &fs, const String& path){
    Serial.printf("Reading file: %s\n", path.c_str());
    
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return "";
    }

    String data = "";

    Serial.print("Read from file: ");
    while(file.available()){
        data += file.readString();
    }
    Serial.println(data);
    file.close();
    return data;
}

void EspSD::writeFile(fs::FS &fs, const String& path, const String& message){
    Serial.printf("Writing file: %s\n", path.c_str());

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

  void EspSD::appendFile(fs::FS &fs, const String& path, const String& message){
    Serial.printf("Appending to file: %s, data: %s\n", path.c_str(), message.c_str());
    File file = fs.open(path.c_str(), FILE_APPEND);
    Serial.println("opened file");
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message.c_str())){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void EspSD::renameFile(fs::FS &fs, const String& path1, const String& path2){
    Serial.printf("Renaming file %s to %s\n", path1.c_str(), path2.c_str());
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void EspSD::deleteFile(fs::FS &fs, const String& path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

  bool EspSD::folderExists(fs::FS &fs, const String& path){
    return fs.exists(path);
  }
  bool EspSD::fileExists(fs::FS &fs, const String& path){
    return fs.exists(path);
  }

  void EspSD::logData(fs::FS &fs,const String& fileName, float data, uint8_t hh, uint8_t mm, uint8_t ss){
    String line = "["+String(hh)+String(mm)+String(ss)+"] "+String(data)+"\n";
    appendFile(SD,String("/logging/")+fileName,line);
  }

  void EspSD::logData(fs::FS &fs,const String& fileName, int data, uint8_t hh, uint8_t mm, uint8_t ss){
    String line = "["+String(hh)+String(mm)+String(ss)+"] "+String(data)+"\n";
    appendFile(SD,String("/logging/")+fileName,line);
  }

  bool EspSD::readKeyValueConfig(const String& key, String& out) {
      auto it = stringConfigs.find(key);
      if (it != stringConfigs.end()) {
          out = it->second;
          return true;
      }
      //not found in memory, time to look in the microSD
      String configFile = readFile(SD, "/config/config.kcv");
      //check if the file contains the key at all
      if(extractKeyFromKSV(configFile, key, out)){
        stringConfigs[key] = out;
        return true;
      }
      out = "";
      return false;
  }

  bool EspSD::readKeyValueConfig(const String& key, float& out) {
    auto it = floatConfigs.find(key);
    if (it != floatConfigs.end()) {
      out = it->second;
      return true;
    }
    
    // Not found in memory, let's try to read it from the microSD
    String configFile = readFile(SD,"/config/config.kcv");
    String rawValue = "";

    // Check if the file contains the key at all
    if (extractKeyFromKSV(configFile, key, rawValue)) {
      // Try to convert the rawValue to a float
      if (rawValue.toFloat()) {
        out = rawValue.toFloat();
        floatConfigs[key] = out; // Store the value in memory
        return true;
      }else{
        out = 0.0;
        return true;  //exists but wrong value format
      }
    }
    
    out = 0.0; // Set a default value if conversion fails or key not found
    return false;
  }

bool EspSD::readKeyValueConfig(const String& key, int& out) {
    auto it = intConfigs.find(key);
    if (it != intConfigs.end()) {
        out = it->second;
        return true;
    }
    
    String configFile = readFile(SD,"/config/config.kcv");
    String rawValue = "";

    if (extractKeyFromKSV(configFile, key, rawValue)) {
        out = rawValue.toInt(); // Convert rawValue to an integer
        intConfigs[key] = out; // Store the value in memory
        return true;
    }

    out = 0; // Set a default value if conversion fails or key not found
    return false;
}

bool EspSD::readKeyValueConfig(const String& key, bool& out) {
    auto it = boolConfigs.find(key);
    if (it != boolConfigs.end()) {
        out = it->second;
        return true;
    }
    
    String configFile = readFile(SD,"/config/config.kcv");
    String rawValue = "";

    if (extractKeyFromKSV(configFile, key, rawValue)) {
        rawValue.toLowerCase(); // Convert to lowercase for comparison
        if (rawValue == "true" || rawValue == "1") {
            out = true;
        } else if (rawValue == "false" || rawValue == "0") {
            out = false;
        } else {
            return false; // Unable to convert to boolean
        }

        boolConfigs[key] = out; // Store the value in memory
        return true;
    }

    out = false; // Set a default value if conversion fails or key not found
    return false;
}
/*
bool EspSD::writeKeyValueConfig(const String& key, const String& value) {
    stringConfigs[key] = value; // Update the in-memory configuration
    
    // Open the file for reading and writing
    File file = SD.open(CONFIG_PATH, FILE_READ | FILE_WRITE);
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", CONFIG_PATH);
        return false;
    }

    String updatedContent;
    
    // Use the updateOrAppendKSV function to update or append key-value pairs
    bool found = updateOrAppendKSV(file, key, value, updatedContent);

    // Close the file
    file.close();

    return found;
}

bool EspSD::updateOrAppendKSV(fs::FS& file, const String& key, const String& value, String& updatedContent) {
    bool found = false;

    // Go to the beginning of the file
    file.seek(0);

    while (file.available()) {
        String line = file.readStringUntil('\n');
        std::vector<String> parts = splitStringBy(line, ';');

        if (parts.size() == 2 && parts[0] == key) {
            // Replace the existing key-value pair
            updatedContent += key + ";" + value + "\n";
            found = true;
        } else {
            // Keep the original line
            updatedContent += line + "\n";
        }
    }

    // If the key was not found, append a new key-value pair
    if (!found) {
        updatedContent += key + ";" + value + "\n";
    }

    // Close the file
    file.close();

    // Reopen the file and truncate it to update the content
    file = SD.open(CONFIG_PATH, FILE_WRITE | FILE_TRUNCATE);
    if (!file) {
        Serial.printf("Failed to reopen file for writing: %s\n", CONFIG_PATH);
        return false;
    }

    // Write the updated content back to the file
    if (file.print(updatedContent.c_str())) {
        Serial.printf("Data written to file: %s\n", CONFIG_PATH);
    } else {
        Serial.printf("Write to file failed: %s\n", CONFIG_PATH);
        file.close();
        return false;
    }

    // Close the file
    file.close();

    return found;
}
*/