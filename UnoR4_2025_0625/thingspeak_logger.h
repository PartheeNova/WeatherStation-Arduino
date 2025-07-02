// thingspeak_logger.h
#ifndef THINGSPEAK_LOGGER_H
#define THINGSPEAK_LOGGER_H

#include <Arduino.h>
#include <WiFiS3.h> // Changed from <Ethernet.h>
#include <avr/pgmspace.h> // Keep this include!
#include <ThingSpeak.h> // For ThingSpeak
#include "config.h"
#include "rs485_soil_sensor.h"
#include "rs485_ambient_sensor.h" // Include new RS485 ambient sensor


unsigned long lastThingSpeakSentTime = 0;

extern WiFiClient tsClient;


void logToThingSpeak() {
  Serial.println(F("\n--- Logging to ThingSpeak ---"));

  // Check WiFi status before attempting to connect
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("WiFi not connected. Skipping ThingSpeak log."));
    return;
  }

  Serial.print("Attempting to send data to ThingSpeak... ");
  ThingSpeak.setField(1, soilTemperature);
  ThingSpeak.setField(2, soilMoisture);
  ThingSpeak.setField(3, ambientTemperatureRS485);
  ThingSpeak.setField(4, ambientHumidityRS485);
  ThingSpeak.setField(5, illuminationRS485);
  ThingSpeak.setField(6, currentRainfall); 
      
  // Write to the ThingSpeak channel
  int httpCode = ThingSpeak.writeFields(TS_CH_ID, TS_WRITE_APIKEY);

  if (httpCode == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(httpCode));
    // You can get more specific error information from ThingSpeak if needed:
    // String error = ThingSpeak.getLastReadStatus();
    // Serial.println("ThingSpeak detailed error: " + error);
  }
 
}

#endif // THINGSPEAK_LOGGER_H