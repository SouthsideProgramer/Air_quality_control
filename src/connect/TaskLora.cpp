#include "taskLora.h"
#include <ArduinoJson.h>
#include <LoRa_E22.h> // thư viện xreef

static LoRa_E22 *loraModule = nullptr;
static HardwareSerial *mySerial = nullptr;

// Node & Target address
// #define NODE_ADDR_H
// #define NODE_ADDR_L
#define add_h 0x11
#define add_l 0x21
#define chan 23
#define net_id 0x00

// ===== Tạo payload JSON từ sensor =====
String createPayload()
{
    StaticJsonDocument<256> doc;

    // Air sensor
    doc["air_hum"] = getAirHumidity();
    doc["air_temp"] = getAirTemperature();
    doc["air_noise"] = getAirNoise();
    doc["air_pm25"] = getAirPM25();
    doc["air_pm10"] = getAirPM10();

    // Water sensor
    doc["water_temp"] = getWaterTemp();
    doc["ec_raw"] = getECRaw_mS();
    doc["ec_comp"] = getECComp_mS();

    doc["gps_lat"] = latitude;
    doc["gps_lng"] = longitude;
    String payload;
    serializeJson(doc, payload);
    return payload;
}

//===== Task gửi LoRa =====
void TaskSendLoRa(void *pvParameters)
{
    LoRa_E22 *module = (LoRa_E22 *)pvParameters;

    for (;;)
    {
        String payload = createPayload();
        // Gửi tới module Pi
        ResponseStatus rs = module->sendFixedMessage(
            add_h,
            add_l,
            chan,
            payload);
        // Serial.println("[LoRa] Sent: " + payload);
        // Serial.println("[LoRa] Status: " + rs.getResponseDescription());
        vTaskDelay(pdMS_TO_TICKS(10000)); // 5 giây/lần
    }
}

// ===== Khởi tạo LoRa =====
void LoRaSetup(HardwareSerial &serial)
{
    mySerial = &serial;

    Serial.println("[LoRa] UART initialized");

    // Khởi tạo module LoRa_E22
    loraModule = new LoRa_E22(mySerial, UART_BPS_RATE_9600);
    if (!loraModule->begin())
    {
        Serial.println("[LoRa] Module init failed!");
        return;
    }

    // Cấu hình module point-to-point

    ResponseStructContainer c = loraModule->getConfiguration();
    Configuration cfg = *(Configuration *)c.data;
    c.close();

    cfg.ADDH = 0x00;
    cfg.ADDL = 0x02;
    cfg.NETID = net_id;
    cfg.CHAN = chan;

    ResponseStatus rs = loraModule->setConfiguration(
        cfg,
        WRITE_CFG_PWR_DWN_SAVE);

    Serial.println(rs.getResponseDescription());
    Serial.println("[LoRa] Module configured (Node/Target/Channel/NetID)");
    delay(1000);

    // Tạo task RTOS gửi JSON
    xTaskCreatePinnedToCore(TaskSendLoRa, "TaskSendLoRa", 6144, (void *)loraModule, 1, NULL, 1);
}

// ===== Getter =====
HardwareSerial *getLoRaSerial()
{
    return mySerial;
}
