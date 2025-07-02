# WeatherStation-Arduino
WeatherStation Arduino Sketch

Project Scope: 
  To collect following weather data
    1. Ambient Temperature, 
    2. Ambient Humidity,
    3. Light intensity,
    4. Soil Temperature,
    5. Soil Moisture
    6. Rain fall.

Project description:

Project build on Arduino IoT board to collect to the above weather data from 3 sensors. 
1. Senosr to detect Ambient Temperature, Humidity and Light intensity via RS485 with slave id 1
2. Sensor to detect Soil Temperature, Soil Moisture via RS485 with slave id 3
3. Rain gauge sensor which detect the number of ticks for every 0.3mm rain fall and accumulate to every 15min interval.

Above data's were collected via Arduino and logged via messages to ThinkSpeak. Collected data shall be able to downloaded from Thingpeak as raw data or customized via google sheet. 

Sepeate web based Dashboard has been build to visuallize the logged data in Thingspeak. 
