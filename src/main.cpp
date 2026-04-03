#include "lib.h"
#include "globals.h"

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 7, 6);
  delay(1000);

  Wifi_setup();     
  WebServer_setup(); 
  AIRsetup();        
  Watersetup();
   xTaskCreate(updateGPSPosition, "updateGPSPosition", 8192, NULL, 1, NULL);
  LoRaSetup(Serial2);
}

void loop() {
  // Serial.println("Main loop running...");
  // delay(1000);
  WebServer_loop();
      // Serial.println("WebServer_loop called");
    // delay(1000);
}