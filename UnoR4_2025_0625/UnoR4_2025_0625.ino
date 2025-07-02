// arduino_sensors_ethernet_optimized.ino
#include <WiFiS3.h>   
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <ThingSpeak.h> // For ThingSpeak
#include <avr/pgmspace.h> // Required for PROGMEM and FPSTR
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

// Include our custom header files
#include "config.h"
#include "rs485_soil_sensor.h"
#include "rs485_ambient_sensor.h"
#include "thingspeak_logger.h"
#include "web_server.h"

// --- Global Objects (Declared here for extern use in headers) ---
// These are declared in the .ino file because they are objects
// and need to be instantiated once. The 'extern' in the .h files
// simply tells the compiler they exist elsewhere.
SoftwareSerial rs485Serial(RS485_RX_PIN, RS485_TX_PIN);
ModbusMaster node;

int status = WL_IDLE_STATUS; // WiFi connection status

WiFiClient tsClient; // For ThingSpeak
WiFiServer server(80); // For Web Server, port 80

//LED Matrix
ArduinoLEDMatrix matrix;
char textBuffer[48];

// --- Timers for non-blocking operations ---
unsigned long previousSoilSensorMillis = 0;
unsigned long previousLEDMillis = 0;
unsigned long previousRainMillis =0;
unsigned long previousThingSpeakMillis = 0;
unsigned long previousWebServerPollMillis = 0;

extern unsigned long lastThingSpeakSentTime;

void setup() {


  Serial.begin(9600);
  while (!Serial) {
    // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println(F("Starting up Arduino Sensors & Ethernet...")); // F() macro
 
  // Setup RainGauge Sensor 
  pinMode(rainGaugePin, INPUT_PULLUP); //the pin as an input with internal pull-up resistor // FALLING: Triggers when the pin goes from HIGH to LOW (typical for reed switches with pull-up)
  attachInterrupt(digitalPinToInterrupt(rainGaugePin), handleTip, FALLING); // Attach an interrupt to the rain gauge pin
  
  // Check for the WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true); // Don't continue
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the WiFi firmware");
  }

  // Attempt to connect to WiFi network
  connectToWiFi();


  initSoilSensor();
  initAmbientSensor();
  initWebServer();

  // Initialize ThingSpeak client
  ThingSpeak.begin(tsClient);
  Serial.println("ThingSpeak client initialized.");

  //LED Matrix intialize
  matrix.begin();
  matrix.textScrollSpeed(50); // Set scrolling speed (lower value = faster)

  // Set initial previousMillis values to trigger first readings immediately
  previousSoilSensorMillis = millis() - SOIL_SENSOR_READ_INTERVAL;
  previousLEDMillis = millis() - LED_INTERVAL;
  previousRainMillis = millis() - RainGaugeUpdateInerval;  
  previousThingSpeakMillis = millis() - THINGSPEAK_LOG_INTERVAL;
  previousWebServerPollMillis = millis() - WEB_SERVER_POLL_INTERVAL;

  Serial.println(F("Setup complete.")); // F() macro
}


void connectToWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(WIFI_SSID);
  
  IPAddress local_ip(192, 168, 1, 123);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(8, 8, 8, 8);
  WiFi.config(local_ip, dns, gateway, subnet);
  //WiFi.config(local_ip, gateway, subnet);
  status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  unsigned long startTime = millis();
  while (status != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    status = WiFi.status(); // Re-check status
    if (millis() - startTime > 30000) { // 30-second timeout
        Serial.println("\nFailed to connect to WiFi after 30 seconds. Please check credentials and network.");
        // You might want to add a retry mechanism or halt
        return; // Or while(true);
    }
  }
  Serial.println("\nConnected to WiFi");
}


void loop() {
  unsigned long currentMillis = millis();

  // --- Read Soil Sensor Data ---
  if (currentMillis - previousSoilSensorMillis >= SOIL_SENSOR_READ_INTERVAL) {
    previousSoilSensorMillis = currentMillis;
    readSoilSensorData();
    
    readAmbientSensorData();
    Serial.print("Tips Count: ");
    Serial.println(tipCount);
  }

  // --- Display data on LED matrix ---
  if (currentMillis - previousLEDMillis >= LED_INTERVAL) {
    previousLEDMillis = currentMillis;
      //LED Matrix 
    snprintf(textBuffer, sizeof(textBuffer), "ST=%.1f;SM=%.1f;AT=%.1f;AH=%.1f;I=%.1f;R=%.1f",soilTemperature, soilMoisture, ambientTemperatureRS485, ambientHumidityRS485, illuminationRS485, tipCount);
    matrix.beginDraw();
    matrix.stroke(0xFFFFFFFF);
    matrix.textFont(Font_5x7);
    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.println(textBuffer); // Use the char array directly
    matrix.endText(SCROLL_LEFT);
  }

  // --- Calcuate rainfall for defined interval.  ---
  if (currentMillis - previousRainMillis >= RainGaugeUpdateInerval) {
    previousRainMillis = currentMillis;
    tipCountInterval = tipCount - tipCountPrevious;
    tipCountPrevious = tipCount;
    currentRainfall = tipCountInterval * rainfallPerTip;
  }

  // --- Log Data to ThingSpeak ---
  if (currentMillis - previousThingSpeakMillis >= THINGSPEAK_LOG_INTERVAL) {
    previousThingSpeakMillis = currentMillis;
    logToThingSpeak();
  }

  // --- Handle Web Server Clients ---
  if (currentMillis - previousWebServerPollMillis >= WEB_SERVER_POLL_INTERVAL) {
    previousWebServerPollMillis = currentMillis;
    handleWebServer();
    //Serial.print(F("Free RAM: ")); Serial.println(freeRam()); //periodically
  }

  // No other blocking code here, so the Arduino remains responsive to serial input

}

// Interrupt Service Routine (ISR)
void handleTip() {
  tipCount++;
}