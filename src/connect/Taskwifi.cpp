#include "TaskWifi.h" // Note: Usually <WiFi.h> with a capital 'F' and angle brackets

const char* ssid = "nongnghiep";
const char* password ="12345678";

IPAddress local_ip(192,168,49,1);
IPAddress gateway(192,168,49,1);
IPAddress subnet(255,255,255,0);

void Wifi_setup()
{
    // 1. Set mode to Access Point
    WiFi.mode(WIFI_AP);
    
    // 2. Configure the IP addresses FIRST
    WiFi.softAPConfig(local_ip, gateway, subnet);
    
    // 3. Start the Access Point
    WiFi.softAP(ssid, password);
    
    Serial.println("Access Point started! Connect to: ");
    Serial.println(ssid);
    Serial.println("IP Address: ");
    Serial.println(WiFi.softAPIP()); 
}