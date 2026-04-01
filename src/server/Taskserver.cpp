#include "Taskserver.h"
#include "../src/connect/TaskWifi.h"
#include "../src/sensor/air_quality.h"
#include "../src/sensor/water_sensor.h"

#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <FS.h>

WebServer server(80);

static String getContentType(const String& path) {
  if (path.endsWith(".html")) return "text/html";
  if (path.endsWith(".css"))  return "text/css";
  if (path.endsWith(".js"))   return "application/javascript";
  if (path.endsWith(".json")) return "application/json";
  if (path.endsWith(".png"))  return "image/png";
  if (path.endsWith(".jpg"))  return "image/jpeg";
  if (path.endsWith(".svg"))  return "image/svg+xml";
  return "text/plain";
}

static bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";

  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}

static void handleRoot() {
  if (!handleFileRead("/index.html")) {
    server.send(404, "text/plain", "index.html not found");
  }
}


static void handleStatus() {
    String json = "{";

    json += "\"ap_name\":\"seal_AP\",";
    json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
    json += "\"clients\":" + String(WiFi.softAPgetStationNum()) + ",";
    json += "\"heap\":" + String(ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String(millis() / 1000) + ",";

    // Air sensor
    json += "\"sensor_ready\":" + String(isAirSensorReady() ? "true" : "false") + ",";
    json += "\"temperature\":" + String(getAirTemperature(), 1) + ",";
    json += "\"humidity\":" + String(getAirHumidity(), 1) + ",";
    json += "\"noise\":" + String(getAirNoise()) + ",";
    json += "\"pm25\":" + String(getAirPM25()) + ",";
    json += "\"pm10\":" + String(getAirPM10()) + ",";
    json += "\"pressure\":" + String(getAirPressure(), 1) + ",";
    json += "\"lux\":" + String(getAirLux()) + ",";

    // Water sensor
    json += "\"water_temp\":" + String(getWaterTemp(), 1) + ",";
    json += "\"ec_voltage\":" + String(getECVoltage(), 3) + ",";
    json += "\"ec_raw\":" + String(getECRaw_mS(), 3) + ",";
    json += "\"ec_comp\":" + String(getECComp_mS(), 3);

    json += "}";

    server.send(200, "application/json", json);
}

static void handleNotFound() {
  if (!handleFileRead(server.uri())) {
    server.send(404, "text/plain", "404: Not Found");
  }
}

static void handleGetKValue() {
    String json = "{";
    json += "\"k_value\":" + String(getKValue(), 3);
    json += "}";
    server.send(200, "application/json", json);
}

static void handleSetKValue() {
    if (server.hasArg("plain")) {
        String body = server.arg("plain");

        StaticJsonDocument<128> doc;
        DeserializationError err = deserializeJson(doc, body);

        if (!err && doc.containsKey("k_value")) {
            float val = doc["k_value"];
            setKValue(val);  // cập nhật kValue

            server.send(200, "application/json", "{\"status\":\"ok\"}");
            Serial.print("[Server] KValue updated: "); Serial.println(val,3);
            return;
        }
    }
    server.send(400, "application/json", "{\"status\":\"error\"}");
}

void WebServer_setup() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed");
    return;
  }

  server.on("/", handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.onNotFound(handleNotFound);
server.on("/api/kvalue", HTTP_GET, handleGetKValue);
server.on("/api/kvalue", HTTP_POST, handleSetKValue);
  server.begin();

  Serial.println("WebServer started");
  Serial.print("Open browser: http://");
  Serial.println(WiFi.softAPIP());
}

void WebServer_loop() {
  server.handleClient();
}