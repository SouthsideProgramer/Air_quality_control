#include "taskLora.h"
#include <ArduinoJson.h>
#include <LoRa_E22.h>  // thư viện xreef

static LoRa_E22* loraModule = nullptr;
static HardwareSerial* mySerial = nullptr;

// Node & Target address
#define NODE_ADDR_H 0x00
#define NODE_ADDR_L 0x01
#define TARGET_ADDR_H 0x00
#define TARGET_ADDR_L 0x02
#define LORA_CHANNEL 23
#define NET_ID 0x00

// ===== Tạo payload JSON từ sensor =====
String createPayload() {
    StaticJsonDocument<256> doc;

    // Air sensor
    doc["air_hum"]  = getAirHumidity();
    doc["air_temp"] = getAirTemperature();
    doc["air_noise"]= getAirNoise();
    doc["air_pm25"] = getAirPM25();
    doc["air_pm10"] = getAirPM10();

    // Water sensor
    doc["water_temp"] = getWaterTemp();
    doc["ec_raw"]     = getECRaw_mS();
    doc["ec_comp"]    = getECComp_mS();

    String payload;
    serializeJson(doc, payload);
    return payload;
}

// ===== Task gửi LoRa =====
void TaskSendLoRa(void* pvParameters) {
    LoRa_E22* module = (LoRa_E22*) pvParameters;

    for (;;) {
            String payload = createPayload();
            // Gửi tới module Pi
            ResponseStatus rs = module->sendFixedMessage(
                TARGET_ADDR_H,
                TARGET_ADDR_L,
                LORA_CHANNEL,
                payload
            );
            Serial.println("[LoRa] Sent: " + payload);
            Serial.println("[LoRa] Status: " + rs.getResponseDescription());
        vTaskDelay(pdMS_TO_TICKS(5000)); // 5 giây/lần
    }
}

// ===== Khởi tạo LoRa =====
void LoRaSetup(HardwareSerial &serial) {
    mySerial = &serial;

    mySerial->begin(9600, SERIAL_8N1, 7, 6); // RX/TX ESP32
    Serial.println("[LoRa] UART initialized");

    // Khởi tạo module LoRa_E22
    loraModule = new LoRa_E22(mySerial, UART_BPS_RATE_9600);
    if (!loraModule->begin()) {
        Serial.println("[LoRa] Module init failed!");
        return;
    }

    // Cấu hình module point-to-point
    Configuration cfg;
    ResponseStructContainer c = loraModule->getConfiguration();
    cfg = *(Configuration*)c.data;
    cfg.ADDH = NODE_ADDR_H;
    cfg.ADDL = NODE_ADDR_L;
    cfg.NETID = NET_ID;
    cfg.CHAN = LORA_CHANNEL;
    loraModule->setConfiguration(cfg, WRITE_CFG_PWR_DWN_SAVE);
    Serial.println("[LoRa] Module configured (Node/Target/Channel/NetID)");
    delay(2000);

    // Tạo task RTOS gửi JSON
    xTaskCreatePinnedToCore(TaskSendLoRa, "TaskSendLoRa", 4096, (void*)loraModule, 1, NULL, 1);
}

// ===== Getter =====
HardwareSerial* getLoRaSerial() {
    return mySerial;
}