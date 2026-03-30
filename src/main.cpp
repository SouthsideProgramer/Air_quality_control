#include "lib.h"
#include "globals.h"

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 7, 6); // RX/TX ESP32
  delay(1000);

  Wifi_setup();      // AP mode
  WebServer_setup(); // web dashboard
  AIRsetup();        // đọc cảm biến
  Watersetup();
  LoRaSetup(Serial2);
}

void loop() {
  WebServer_loop();
      // Serial.println("WebServer_loop called");
    // delay(1000);
}