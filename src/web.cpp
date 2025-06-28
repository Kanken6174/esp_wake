#include "web.hpp"
#include "SoundI2S.hpp"
#include <SD.h>

extern SoundI2S sound;

WebConfigServer::WebConfigServer(EspSD& esd, AlarmManager& alarmManager) : server(80), sd(esd), am(alarmManager) {}

void WebConfigServer::handleClient() {
    server.handleClient();
}

void WebConfigServer::updateSensorData(float t, float h, int v) {
    ct = t;
    ch = h;
    cv = v;
}

void WebConfigServer::begin() {
    server.on("/", HTTP_ANY, [this]() {
        if (server.method() == HTTP_POST) {
            Serial.println("[WEB] Received POST on /");
            if (server.hasArg("action")) {
                String action = server.arg("action");
                Serial.println("[WEB]   - Action: " + action);
                if (action == "save_alarms" && server.hasArg("config")) {
                    String configData = server.arg("config");
                    Serial.println("[WEB]   - Saving alarms...");
                    sd.writeFile(SD, "/config/alarms.kcv", configData);
                    am.reloadAlarms();
                } else if (action == "play" && server.hasArg("track")) {
                    String track = server.arg("track");
                    Serial.println("[WEB]   - Play request for track: " + track);
                    sound.play("/music/"+track, sd);
                } else if (action == "stop") {
                    Serial.println("[WEB]   - Stop request");
                    sound.stop();
                } else if (action == "set_volume" && server.hasArg("volume")) {
                    String volStr = server.arg("volume");
                    float vol = volStr.toFloat() / 100.0f;
                    Serial.println("[WEB]   - Volume set to: " + volStr + "%");
                    sound.setVolume(vol);
                }
            } else {
                Serial.println("[WEB]   - POST request received with no 'action' parameter.");
            }
            server.sendHeader("Location", "/");
            server.send(302, "text/plain", "Redirecting...");
            return;
        }

        Serial.println("[WEB] Received GET on /. Building page...");
        String trackOptions = "";
        File dir = SD.open("/music");
        if (dir) {
            Serial.println("[WEB]   - Reading /music directory...");
            File file = dir.openNextFile();
            while(file){
                if(!file.isDirectory()){
                    String fileName = String(file.name());
                    Serial.println("[WEB]   - Found track: " + fileName);
                    if (fileName == sound.getCurrentTrack() && sound.getState() == "playing") {
                        trackOptions += "<option value=\"" + fileName + "\" selected>" + fileName + "</option>";
                    } else {
                        trackOptions += "<option value=\"" + fileName + "\">" + fileName + "</option>";
                    }
                }
                // --- FIX: Close the file handle before finding the next one ---
                file.close(); 
                file = dir.openNextFile();
            }
            dir.close();
        } else {
            Serial.println("[WEB]   - ERROR: Could not open /music directory!");
        }

        String alarmsConfig = sd.readFile(SD, "/config/alarms.kcv");
        
        String html = "<!DOCTYPE html><html><head><title>ESP32 Control Panel</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
        html += "<style>body{font-family:-apple-system,system-ui,BlinkMacSystemFont,'Segoe UI',Roboto,'Helvetica Neue',Arial,sans-serif;background-color:#121212;color:#e0e0e0;margin:0;padding:15px;}";
        html += ".card{background-color:#1e1e1e;border:1px solid #333;border-radius:8px;padding:20px;margin-bottom:20px;max-width:600px;margin-left:auto;margin-right:auto;}";
        html += "h1,h2{color:#fff;text-align:center;border-bottom:1px solid #444;padding-bottom:10px;margin-top:0;}";
        html += "textarea,select{width:100%;background-color:#252525;color:#ddd;border:1px solid #444;border-radius:4px;padding:10px;box-sizing:border-box;margin-bottom:10px;}";
        html += "button{width:100%;background-color:#007bff;color:white;border:none;padding:12px;border-radius:4px;font-size:16px;cursor:pointer;margin-top:10px;}";
        html += "button.stop{background-color:#dc3545;} button.vol{background-color:#28a745;}";
        html += "p{margin:5px 0 15px 0; text-align:center; font-size:1.1em;} input[type=range]{width:100%;}</style>";
        html += "</head><body><h1>ESP32 Control Panel</h1>";
        
        html += "<div class=\"card\"><h2>Media Player</h2>";
        html += "<p>Status: " + sound.getState();
        if (sound.getState() == "playing") html += " (" + sound.getCurrentTrack() + ")";
        html += "</p>";
        html += "<form action='/' method='post'><select name='track'>" + trackOptions + "</select><input type='hidden' name='action' value='play'><button type='submit'>Play</button></form>";
        html += "<form action='/' method='post'><input type='hidden' name='action' value='stop'><button type='submit' class='stop'>Stop</button></form>";
        html += "<form action='/' method='post'><p>Volume: " + String((int)(sound.getVolume() * 100)) + "%</p><input type='range' name='volume' min='0' max='100' value='" + String((int)(sound.getVolume() * 100)) + "'><input type='hidden' name='action' value='set_volume'><button type='submit' class='vol'>Set Volume</button></form>";
        html += "</div>";

        html += "<div class=\"card\"><h2>Alarms Configuration</h2>";
        html += "<form action='/' method='post'><textarea rows='10' name='config'>" + alarmsConfig + "</textarea><input type='hidden' name='action' value='save_alarms'><button type='submit'>Save Alarms</button></form>";
        html += "</div>";

        html += "</body></html>";
        server.send(200, "text/html", html);
    });

    server.on("/api/sensors", HTTP_GET, [this]() {
        JsonDocument doc;
        doc["temperature"] = this->ct;
        doc["humidity"] = this->ch;
        doc["tvoc"] = this->cv;
        
        String output;
        serializeJson(doc, output);
        
        this->server.send(200, "application/json", output);
    });

    server.begin();
}