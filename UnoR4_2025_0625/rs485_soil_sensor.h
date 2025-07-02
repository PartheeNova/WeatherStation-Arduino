// rs485_soil_sensor.h
#ifndef RS485_SOIL_SENSOR_H
#define RS485_SOIL_SENSOR_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include "config.h"

float soilTemperature = 0.0;
float soilMoisture = 0.0;

extern SoftwareSerial rs485Serial;
extern ModbusMaster node;

void preTransmission();
void postTransmission();

void initSoilSensor() {
  pinMode(RS485_DE_RE_PIN, OUTPUT);
  digitalWrite(RS485_DE_RE_PIN, LOW);
  rs485Serial.begin(9600);
  node.begin(SOIL_SENSOR_MODBUS_ID, rs485Serial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
  Serial.println(F("RS485 Soil Sensor Initialized.")); // F() macro
}

void readSoilSensorData() {
    // It's crucial to set the slave ID *before* each transaction if you're using one ModbusMaster object
  // for multiple slave devices on the same bus.
  node.begin(SOIL_SENSOR_MODBUS_ID, rs485Serial); // Set slave ID for this read

  Serial.println(F("\n--- Reading Ambient Sensor (RS485 - Node 1) ---"));
  

  uint8_t result_temp = node.readHoldingRegisters(SOIL_TEMP_REGISTER_START_ADDRESS, SOIL_TEMP_NUM_REGISTERS);
  if (result_temp == node.ku8MBSuccess) {
    if (SOIL_TEMP_NUM_REGISTERS == 1) {
      uint16_t rawTemp = node.getResponseBuffer(0);
      soilTemperature = (float)rawTemp / 10.0;
    } else if (SOIL_TEMP_NUM_REGISTERS == 2) {
      union { uint32_t u32; float f; } data;
      data.u32 = ((uint32_t)node.getResponseBuffer(1) << 16) | node.getResponseBuffer(0);
      soilTemperature = data.f;
    }
    //Serial.print(F("Soil Temperature: ")); // F() macro
    //Serial.print(soilTemperature, 2);
    //Serial.println(F(" °C")); // F() macro
  } else {
    Serial.print(F("Failed to read Soil Temperature. Modbus error: ")); // F() macro
    Serial.println(result_temp, HEX);
  }

  delay(100);

  uint8_t result_moist = node.readHoldingRegisters(SOIL_MOIST_REGISTER_START_ADDRESS, SOIL_MOIST_NUM_REGISTERS);
  if (result_moist == node.ku8MBSuccess) {
    // --- ADD THIS DEBUGGING BLOCK ---
    //Serial.print(F("Raw Moisture Registers: "));
    //for (int i = 0; i < SOIL_MOIST_NUM_REGISTERS; i++) {
    //  Serial.print(node.getResponseBuffer(i), HEX); // Print in hexadecimal
    //  Serial.print(F(" "));
    //}
    //Serial.println();
    // --- END DEBUGGING BLOCK ---
    if (SOIL_MOIST_NUM_REGISTERS == 1) {
      uint16_t rawMoist = node.getResponseBuffer(0);
      soilMoisture = (float)rawMoist / 10.0;
    } else if (SOIL_MOIST_NUM_REGISTERS == 2) {
      union { uint32_t u32; float f; } data;
      data.u32 = ((uint32_t)node.getResponseBuffer(1) << 16) | node.getResponseBuffer(0);
      soilMoisture = data.f;
    }
    //Serial.print(F("Soil Moisture: ")); // F() macro
    //Serial.print(soilMoisture, 2);
    //Serial.println(F(" %")); // F() macro
  } else {
    Serial.print(F("Failed to read Soil Moisture. Modbus error: ")); // F() macro
    Serial.println(result_moist, HEX);
  }
}

void preTransmission() {
  digitalWrite(RS485_DE_RE_PIN, HIGH);
  delayMicroseconds(100);
}

void postTransmission() {
  digitalWrite(RS485_DE_RE_PIN, LOW);
  delayMicroseconds(100);
}

#endif // RS485_SOIL_SENSOR_H