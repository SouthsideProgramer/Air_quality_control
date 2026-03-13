#ifndef AIR_QUALITY_H
#define AIR_QUALITY_H

#include "lib.h"
#include <ModbusMaster.h>

extern void AIRsetup();

extern float getAirHumidity();
extern float getAirTemperature();
extern uint16_t getAirNoise();
extern uint16_t getAirPM25();
extern uint16_t getAirPM10();
extern float getAirPressure();
extern uint32_t getAirLux();
extern bool isAirSensorReady();

#endif