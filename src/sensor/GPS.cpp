#include "../include/globals.h"
// #include <M5Station.h>
const char* GPS_TAG = "GPS";

TinyGPSPlus gps;
HardwareSerial GPSSerial(0);

double latitude = 10.773281;
double longitude = 106.660605;

void displayGPSInfo()
{
    static double lastLat = 0.0, lastLng = 0.0;
    if(gps.location.isValid()) {
        Serial.println("gps are valid, ");
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        Serial.println("Latitude: "); Serial.println(latitude, 6); Serial.println(" | Longitude: "); Serial.println(longitude, 6);

        if (latitude != lastLat || longitude != lastLng) {
            lastLat = latitude;
            lastLng = longitude;
        }
    }
    
}

void updateGPSPosition(void *pvParameters)
{
  
    GPSSerial.begin(115200, SERIAL_8N1, GPS_RXD, GPS_TXD); 
        // Serial.println("GPS Serial initialized");
    volatile bool hasValidFix = false;

    TickType_t lastWakeTime = xTaskGetTickCount();
    const TickType_t delay = 1000 / portTICK_PERIOD_MS;

    while (true) {
        while (GPSSerial.available() > 0) {
            if (gps.encode(GPSSerial.read())) {
                if (!hasValidFix && gps.location.isValid()) {
                    hasValidFix = true;
                }
                displayGPSInfo();
            }
        }

        if (GPSSerial.available() < 0 && gps.charsProcessed() < 10) {
            Serial.println("No GPS data received. Please check the GPS connection.");
        }

        vTaskDelayUntil(&lastWakeTime, delay);
    }
    vTaskDelete(NULL);
}
// may ban ABC co bug gi thi bao som nha chu bao sau 1 ngay tui lam hok co noi