#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

#include <Arduino.h>
#include "../src/sensor/DFRobot_ECPRO.h"

// ===== Setup =====
void Watersetup();

// Getter
float getWaterTemp();
float getECVoltage();
float getECRaw_mS();
float getECComp_mS();

// Kiểm tra sensor đã sẵn sàng chưa
bool isWaterReady();

#endif