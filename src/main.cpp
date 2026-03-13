#include "lib.h"
#include "globals.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wifi_setup();      // AP mode
  AIRsetup();        // đọc cảm biến
  WebServer_setup(); // web dashboard
}

void loop() {
  WebServer_loop();
}