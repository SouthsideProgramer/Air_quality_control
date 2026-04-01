#ifndef TASK_LORA_H
#define TASK_LORA_H

#include <Arduino.h>
#include "globals.h"
// Khởi tạo task LoRa
void LoRaSetup(HardwareSerial &serial);
// Lấy serial LoRa để gửi dữ liệu thủ công (nếu cần)
HardwareSerial *getLoRaSerial();

#endif