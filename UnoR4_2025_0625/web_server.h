// web_server.h
#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WiFiS3.h> // Changed from <Ethernet.h>
#include <avr/pgmspace.h> // Keep this include!
#include "config.h"
#include "rs485_soil_sensor.h"
#include "rs485_ambient_sensor.h" // Include new RS485 ambient sensor

// Define HTML content in PROGMEM to save SRAM
const char HTML_PAGE_START[] PROGMEM =
  "<!DOCTYPE HTML>\n"
  "<html>\n"
  "<head>\n"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
  "<style>\n"
  "body { font-family: Arial, Helvetica, sans-serif; text-align: center; background-color: #f0f0f0; }\n"
  "h1 { color: #333; }\n"
  ".container { background-color: #fff; margin: 20px auto; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); max-width: 500px; }\n"
  "p { font-size: 1.2em; margin: 10px 0; }\n"
  "strong { color: #007bff; }\n"
  "</style>\n"
  "</head>\n"
  "<body>\n"
  "<div class='container'>\n"
  "<h1>Arduino Sensor Data</h1>\n";

const char HTML_PAGE_END[] PROGMEM =
  "</div>\n"
  "</body>\n"
  "</html>\n";

extern WiFiServer server; // Changed from EthernetServer

void initWebServer() {
  server.begin();
  Serial.print(F("Web server started at IP: "));
  Serial.println(WiFi.localIP()); // Use WiFi.localIP()
}

void handleWebServer() {
  // Check WiFi status before attempting to handle clients
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(F("WiFi not connected. Skipping web server handling."));
    return;
  }

  WiFiClient client = server.available(); // Changed from EthernetClient

  if (client) {
    Serial.println(F("New web client connected."));

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (client.peek() == '\r' || client.peek() == -1) {
            client.print(F("HTTP/1.1 200 OK\r\n"));
            client.print(F("Content-Type: text/html\r\n"));
            client.print(F("Connection: close\r\n"));
            client.print(F("Refresh: 10\r\n"));
            client.print(F("\r\n")); // End of HTTP headers

            // Send HTML content from PROGMEM using the correct cast
            client.print((__FlashStringHelper*)HTML_PAGE_START);

            client.print(F("<h2>Soil Sensor Data:</h2>"));
            client.print(F("<p>Soil Temperature: <strong>"));
            client.print(soilTemperature, 2);
            client.print(F(" &#8451;</strong></p>"));

            client.print(F("<p>Soil Moisture: <strong>"));
            client.print(soilMoisture, 2);
            client.print(F(" %</strong></p>"));

            client.print(F("<h2>Ambient Sensor Data:</h2>"));
            client.print(F("<p>Ambient Temperature: <strong>"));
            client.print(ambientTemperatureRS485, 2); // Use RS485 ambient variable
            client.print(F(" &#8451;</strong></p>"));

            client.print(F("<p>Ambient Humidity: <strong>"));
            client.print(ambientHumidityRS485, 2); // Use RS485 ambient variable
            client.print(F(" %</strong></p>"));

            client.print(F("<p>Illumination : <strong>"));
            client.print(illuminationRS485, 2); // Use RS485 illumination variable
            client.print(F(" Lux</strong></p>"));

            client.print(F("<h2>Rain Sensor Data:</h2>"));
            client.print(F("<p>Tips count : <strong>"));
            client.print(tipCount); // Use RS485 illumination variable
            client.print(F(" count</strong></p>"));

            client.print(F("<p>Last updated: "));
            client.print(millis() / 1000);
            client.println(F("s ago</p>"));

            client.print((__FlashStringHelper*)HTML_PAGE_END);

            break; // Break out of the while client.connected() loop
          }
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println(F("Web client disconnected."));
  }
}

#endif // WEB_SERVER_H