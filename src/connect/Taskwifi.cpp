#include "TaskWifi.h"
#include "../server/Taskserver.h"
#include <WiFi.h>

const char *ssid = "nongnghiep";
const char *password = "12345678";

IPAddress local_ip(192, 168, 49, 1);
IPAddress gateway(192, 168, 49, 1);
IPAddress subnet(255, 255, 255, 0);

TaskHandle_t wifiTaskHandle = NULL;

void Wifi_Task(void *pvParameters)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);

    if (WiFi.softAP(ssid, password))
    {
        Serial.println("AP started");
        Serial.println(WiFi.softAPIP());

        WebServer_setup();
    }

    while (true)
    {
        WebServer_loop();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Wifi_setup()
{
    xTaskCreatePinnedToCore(
        Wifi_Task,
        "WiFi Task",
        8192,
        NULL,
        3,
        &wifiTaskHandle,
        1);
}