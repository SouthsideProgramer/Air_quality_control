#include "water_sensor.h"

#define EC_PIN 1
#define TE_PIN 2

static float kValue = 1.225f;
DFRobot_ECPRO_PT1000 ecpt;

typedef struct {
    uint16_t ec_mV;
    uint16_t te_mV;
    float ecVoltage;
    float teVoltage;
    float temp;
    float ecRaw_mS;
    float ecComp_mS;
    float ecComp_uS;
} SensorData_t;

static SensorData_t gSensorData;
static SemaphoreHandle_t gSensorMutex = NULL;
static bool gWaterReady = false; // cờ ready

// ==== Đọc ADC ====
uint16_t readMilliVolt(int pin, int samples = 20) {
    long sum = 0;
    for (int i = 0; i < samples; i++) {
        sum += analogRead(pin);
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    float adcAvg = sum / (float)samples;
    float voltage_mV = adcAvg * 3300.0f / 4095.0f;
    return (uint16_t)(voltage_mV + 0.5f);
}

// ==== Task đọc sensor RTOS ====
void TaskReadSensors(void *pvParameters) {
    DFRobot_ECPRO ec(kValue);

    for (;;) {
        uint16_t ec_mV = readMilliVolt(EC_PIN);
        uint16_t te_mV = readMilliVolt(TE_PIN);

        float ecVoltage = ec_mV / 1000.0f;
        float teVoltage = te_mV / 1000.0f;
        float temp = ecpt.convVoltagetoTemperature_C(teVoltage);

        float ecRaw_mS = ec.getEC_us_cm(ecVoltage);
        float ecComp_mS = ec.getEC_us_cm(ecVoltage, temp);
        float ecComp_uS = ecComp_mS * 1000.0f;

        if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
            gSensorData.ec_mV = ec_mV;
            gSensorData.te_mV = te_mV;
            gSensorData.ecVoltage = ecVoltage;
            gSensorData.teVoltage = teVoltage;
            gSensorData.temp = temp;
            gSensorData.ecRaw_mS = ecRaw_mS;
            gSensorData.ecComp_mS = ecComp_mS;
            gSensorData.ecComp_uS = ecComp_uS;
            xSemaphoreGive(gSensorMutex);

            // đánh dấu sensor đã sẵn sàng
            gWaterReady = true;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ==== Task in Serial debug ====
void TaskPrintSerial(void *pvParameters) {
    SensorData_t localData;
    for (;;) {
        if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
            localData = gSensorData;
            xSemaphoreGive(gSensorMutex);

            Serial.print("Water Temp: "); Serial.print(localData.temp, 2); Serial.print(" °C\t");
            Serial.print("EC Voltage: "); Serial.print(localData.ecVoltage, 3); Serial.print(" V\t");
            Serial.print("EC Raw: "); Serial.print(localData.ecRaw_mS, 3); Serial.print(" mS/cm\t");
            Serial.print("EC Comp: "); Serial.print(localData.ecComp_mS, 3); Serial.println(" mS/cm");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ==== Khởi tạo ====
void Watersetup() {
    Serial.println("Water sensor setup...");
    analogReadResolution(12);
    analogSetPinAttenuation(EC_PIN, ADC_11db);
    analogSetPinAttenuation(TE_PIN, ADC_11db);

    gSensorMutex = xSemaphoreCreateMutex();
    if (!gSensorMutex) {
        Serial.println("Failed to create mutex!");
        while (1) delay(1000);
    }

    memset(&gSensorData, 0, sizeof(gSensorData));
    gWaterReady = false;

    xTaskCreatePinnedToCore(TaskReadSensors, "TaskReadSensors", 8192, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(TaskPrintSerial, "TaskPrintSerial", 4096, NULL, 1, NULL, 1);

    Serial.println("Water RTOS tasks started");
}

// ==== Getter ====
float getWaterTemp() {
    float temp = 0;
    if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
        temp = gSensorData.temp;
        xSemaphoreGive(gSensorMutex);
    }
    return temp;
}

float getECVoltage() {
    float val = 0;
    if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
        val = gSensorData.ecVoltage;
        xSemaphoreGive(gSensorMutex);
    }
    return val;
}

float getECRaw_mS() {
    float val = 0;
    if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
        val = gSensorData.ecRaw_mS;
        xSemaphoreGive(gSensorMutex);
    }
    return val;
}

float getECComp_mS() {
    float val = 0;
    if (xSemaphoreTake(gSensorMutex, portMAX_DELAY) == pdTRUE) {
        val = gSensorData.ecComp_mS;
        xSemaphoreGive(gSensorMutex);
    }
    return val;
}

bool isWaterReady() {
    return gWaterReady;
}