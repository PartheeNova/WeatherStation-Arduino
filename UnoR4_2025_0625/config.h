// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h> // For byte, IPAddress, etc.

const char* WIFI_SSID = "Parthee ban 2.4 G"; // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "060319871978"; // Replace with your WiFi Password


// --- ThingSpeak Configuration ---
#define TS_WRITE_APIKEY "3LUEF6XVK7HJRGTH" // Store API Key in PROGMEM
#define TS_CH_ID  2958959 // Replace with your ThingSpeak Channel ID


// --- RS485 Configuration (General) ---
const byte RS485_DE_RE_PIN = 5; // Connect DE and RE of MAX485/SP3485 to this pin
const byte RS485_RX_PIN = 3;    // Connect RO (Receiver Output) to this pin(white)
const byte RS485_TX_PIN = 4;    // Connect DI (Driver Input) to this pin (Greay)

// --- RS485 Soil Sensor Configuration ---
const byte SOIL_SENSOR_MODBUS_ID = 3; // Modbus Slave ID of your soil sensor
const int SOIL_TEMP_REGISTER_START_ADDRESS = 0x0001; // Your sensor's soil temp register address
const int SOIL_MOIST_REGISTER_START_ADDRESS = 0x0000; // Your sensor's soil moisture register address
const int SOIL_TEMP_NUM_REGISTERS = 1; // 1 for 16-bit integer, 2 for 32-bit float
const int SOIL_MOIST_NUM_REGISTERS = 1; // 1 for 16-bit integer, 2 for 32-bit float

// --- RS485 Ambient Sensor Configuration (New) ---
const byte AMBIENT_SENSOR_MODBUS_ID = 1; // Modbus Slave ID of your ambient RS485 sensor
const int AMBIENT_TEMP_REGISTER_ADDRESS = 0x0001; // Example register address, adjust based on your sensor
const int AMBIENT_HUMID_REGISTER_ADDRESS = 0x0000; // Example register address, adjust based on your sensor
const int ILLUMINATION_REGISTER_ADDRESS = 0x0008; // Example register address, adjust based on your sensor
const int AMBIENT_TEMP_NUM_REGISTERS = 1; // Assuming 16-bit integers for all
const int AMBIENT_HUMID_NUM_REGISTERS = 1; // 1 for 16-bit integer, 2 for 32-bit float
const int AMBIENT_ILLU_NUM_REGISTERS = 2; // 1 for 16-bit integer, 2 for 32-bit float


// --- Timed Event Intervals (in milliseconds) ---
const unsigned long SOIL_SENSOR_READ_INTERVAL = 5000;  // Read soil sensor every 5 seconds
const unsigned long LED_INTERVAL = 15000; // Display data evey 15 secs on LED matrix
const unsigned long RainGaugeUpdateInerval = 300000; // Calcualte Rainfall every 5 min 
const unsigned long THINGSPEAK_LOG_INTERVAL = 300000; // Log to ThingSpeak every 5 min (300 secs)
const unsigned long WEB_SERVER_POLL_INTERVAL = 500; // Poll web server client every 100ms

// --- RainGauge parameter
const int rainGaugePin = 2; // Using digital pin 2 for external interrupt
volatile long tipCount = 0; // Variable to store the number of tips, 'volatile' is important for variables modified in an ISR
long tipCountPrevious = 0;
int tipCountInterval = 0;
const float rainfallPerTip = 0.3; // // Calibration value (adjust based on your rain gauge's specifications) in millimeters
float currentRainfall =0;

#endif // CONFIG_H