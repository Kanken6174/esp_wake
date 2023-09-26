#include "web.hpp"
#include "Alarms.hpp"

void WebConfigServer::begin() {
// Define web server routes
server.on("/", HTTP_GET, [this]() {
    String alarmsConfig = sd.readFile(SD, "/config/alarms.kcv");
    String html = "<html><body>";
    html += "<h1>Alarms Configuration</h1>";
    html += "<textarea rows='10' cols='50' name='config'>" + alarmsConfig + "</textarea><br>";
    html += "<input type='submit' value='Save'>";
    html += "</body></html>";
    server.send(200, "text/html", html);
});

server.on("/", HTTP_POST, [this]() {
    if (server.hasArg("config")) {
    String configData = server.arg("config");
    Serial.print("Saving config: ");
    Serial.println(configData);
    sd.writeFile(SD, "/config/alarms.kcv", configData);
    am.reloadAlarms();
    }
    server.send(200, "text/plain", "Configuration saved successfully.");
});

// Start the server
server.begin();
}