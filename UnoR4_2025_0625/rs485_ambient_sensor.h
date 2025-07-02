// rs485_ambient_sensor.h
#ifndef RS485_AMBIENT_SENSOR_H
#define RS485_AMBIENT_SENSOR_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include "config.h"

float ambientTemperatureRS485 = 0.0;
float ambientHumidityRS485 = 0.0;
float illuminationRS485 = 0.0;

extern SoftwareSerial rs485Serial; // Using the same SoftwareSerial instance
extern ModbusMaster node;          // Using the same ModbusMaster instance

void initAmbientSensor() {
  // The general RS485 pins are already initialized in initSoilSensor
  // We just need to make sure the ModbusMaster node is ready to communicate with the new slave ID.
  // The 'node' object's begin function sets the slave ID.
  // We will re-initialize 'node' or pass the desired slave ID for each read.
  Serial.println(F("RS485 Ambient Sensor Initialized (Modbus node 3)."));
}

void readAmbientSensorData() {
  // It's crucial to set the slave ID *before* each transaction if you're using one ModbusMaster object
  // for multiple slave devices on the same bus.
  node.begin(AMBIENT_SENSOR_MODBUS_ID, rs485Serial); // Set slave ID for this read
  delay(100); // Small delay between reads
  Serial.println(F("\n--- Reading Ambient Sensor (RS485 - Node 3) ---"));

  // Read Ambient Temperature
  uint8_t result_temp = node.readHoldingRegisters(AMBIENT_TEMP_REGISTER_ADDRESS, AMBIENT_TEMP_NUM_REGISTERS);
  if (result_temp == node.ku8MBSuccess) {
    if (AMBIENT_TEMP_NUM_REGISTERS == 1) {
      uint16_t rawTemp = node.getResponseBuffer(0);
      ambientTemperatureRS485 = (float)rawTemp / 10.0; // Assuming 0.1 degree resolution
    } else {
      // Handle other data types if needed
    }
    //Serial.print(F("Ambient RS485 Temperature: "));
    //Serial.print(ambientTemperatureRS485, 2);
    //Serial.println(F(" °C"));
  } else {
    Serial.print(F("Failed to read Ambient RS485 Temperature. Modbus error: "));
    Serial.println(result_temp, HEX);
  }
  delay(100); // Small delay between reads

  // Read Ambient Humidity
  uint8_t result_humid = node.readHoldingRegisters(AMBIENT_HUMID_REGISTER_ADDRESS, AMBIENT_HUMID_NUM_REGISTERS);
  if (result_humid == node.ku8MBSuccess) {
    if (AMBIENT_HUMID_NUM_REGISTERS == 1) {
      uint16_t rawHumid = node.getResponseBuffer(0);
      ambientHumidityRS485 = (float)rawHumid / 10.0; // Assuming 0.1% resolution
    } else {
      // Handle other data types if needed
    }
    //Serial.print(F("Ambient RS485 Humidity: "));
    //Serial.print(ambientHumidityRS485, 2);
    //Serial.println(F(" %"));
  } else {
    Serial.print(F("Failed to read Ambient RS485 Humidity. Modbus error: "));
    Serial.println(result_humid, HEX);
  }

  delay(100); // Small delay between reads

  // Read Illumination
  uint8_t result_illum = node.readHoldingRegisters(ILLUMINATION_REGISTER_ADDRESS, AMBIENT_ILLU_NUM_REGISTERS);
  if (result_illum == node.ku8MBSuccess) {
      if (AMBIENT_ILLU_NUM_REGISTERS == 1) {
        uint16_t rawIllum = node.getResponseBuffer(0);
        //Serial.print(F("RawIllumination: "));
        //Serial.println(rawIllum, 2);
        illuminationRS485 = (float)rawIllum; // Assuming direct lux value or similar
      } else {
        // Handle other data types if needed (e.g., if it's a 32-bit float)
        uint32_t rawIllum = node.getResponseBuffer(0);
        //Serial.print(F("RawIllumination: "));
        //Serial.println(rawIllum, 2);
        illuminationRS485 = (float)rawIllum; // Assuming direct lux value or similar
      }
    //Serial.print(F("Illumination "));
    //Serial.print(illuminationRS485, 2);
    //Serial.println(F(" Lux")); // Or appropriate unit
  } else {
    Serial.print(F("Failed to read Illumination RS485. Modbus error: "));
    Serial.println(result_illum, HEX);
  }
}

// preTransmission and postTransmission are already defined globally for the ModbusMaster node.
// No need to redefine them here.

#endif // RS485_AMBIENT_SENSOR_H