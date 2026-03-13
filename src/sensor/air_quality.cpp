#include "air_quality.h"

ModbusMaster node;

// D3 = GPIO6, D4 = GPIO7
#define RXD2 6
#define TXD2 7

// Nếu dùng module RS485 có DE/RE thì bật lên
// #define MAX485_DE_RE 4

uint8_t foundSlaveID = 0;

// ===== Biến lưu dữ liệu mới nhất =====
static float g_humidity = 0.0f;
static float g_temperature = 0.0f;
static uint16_t g_noise = 0;
static uint16_t g_pm25 = 0;
static uint16_t g_pm10 = 0;
static float g_pressure = 0.0f;
static uint32_t g_lux = 0;
static bool g_sensorReady = false;

void preTransmission() {
  // digitalWrite(MAX485_DE_RE, HIGH);
}

void postTransmission() {
  // digitalWrite(MAX485_DE_RE, LOW);
}

bool scanModbusSlave() {
  for (uint8_t id = 1; id <= 247; id++) {
    node.begin(id, Serial1);

    uint8_t result = node.readHoldingRegisters(502, 1);  // test Noise register

    if (result == node.ku8MBSuccess) {
      foundSlaveID = id;
      Serial.print("Found Modbus slave ID: ");
      Serial.println(foundSlaveID);
      return true;
    }
    delay(50);
  }

  Serial.println("No Modbus slave found!");
  return false;
}

void readAirSensorBlock(void *vParameters) {
  while (true) {
    if (foundSlaveID == 0) {
      Serial.println("No slave ID found, skip reading.");
      g_sensorReady = false;
      vTaskDelay(5000 / portTICK_PERIOD_MS);
      continue;
    }

    node.begin(foundSlaveID, Serial1);
    uint8_t result = node.readHoldingRegisters(500, 8);

    if (result == node.ku8MBSuccess) {
      uint16_t humidity_raw = node.getResponseBuffer(0);
      uint16_t temp_raw     = node.getResponseBuffer(1);
      uint16_t noise_raw    = node.getResponseBuffer(2);
      uint16_t pm25_raw     = node.getResponseBuffer(3);
      uint16_t pm10_raw     = node.getResponseBuffer(4);
      uint16_t pressure_raw = node.getResponseBuffer(5);
      uint16_t light_high   = node.getResponseBuffer(6);
      uint16_t light_low    = node.getResponseBuffer(7);

      g_humidity    = humidity_raw / 10.0f;
      g_temperature = temp_raw / 10.0f;
      g_noise       = noise_raw;
      g_pm25        = pm25_raw;
      g_pm10        = pm10_raw;
      g_pressure    = pressure_raw / 10.0f;
      g_lux         = ((uint32_t)light_high << 8) | light_low;
      g_sensorReady = true;

      String msg = "\n========== AIR SENSOR ==========\n";
      msg += "Humidity: " + String(g_humidity, 1) + " %RH\n";
      msg += "Temperature: " + String(g_temperature, 1) + " C\n";
      msg += "Noise: " + String(g_noise) + " dB\n";
      msg += "PM2.5: " + String(g_pm25) + " ug/m3\n";
      msg += "PM10: " + String(g_pm10) + " ug/m3\n";
      msg += "Pressure: " + String(g_pressure, 1) + " kPa\n";
      msg += "Ambient Light: " + String(g_lux) + " Lux\n";
      msg += "================================\n";

      Serial.print(msg);
      node.clearResponseBuffer();
    } else {
      g_sensorReady = false;
      Serial.print("Read failed. Modbus error code: 0x");
      Serial.println(result, HEX);
    }

    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void AIRsetup() {
  Serial.begin(115200);

  /*
  pinMode(MAX485_DE_RE, OUTPUT);
  digitalWrite(MAX485_DE_RE, LOW);
  */

  Serial1.begin(4800, SERIAL_8N1, RXD2, TXD2);

  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  if (scanModbusSlave()) {
    Serial.println("Air sensor Modbus communication successful");
  } else {
    Serial.println("Air sensor Modbus communication failed");
  }

  Serial.println("Air sensor Modbus init done");
  xTaskCreate(readAirSensorBlock, "ReadAirSensor", 8192, NULL, 1, NULL);
}

// ===== Getter =====
float getAirHumidity() { return g_humidity; }
float getAirTemperature() { return g_temperature; }
uint16_t getAirNoise() { return g_noise; }
uint16_t getAirPM25() { return g_pm25; }
uint16_t getAirPM10() { return g_pm10; }
float getAirPressure() { return g_pressure; }
uint32_t getAirLux() { return g_lux; }
bool isAirSensorReady() { return g_sensorReady; }