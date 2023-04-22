#ifndef _APP_SENSOR_H_
#define _APP_SENSOR_H_

#include <Arduino.h>
#include <WiFiEspAT.h>
#include <ThingSpeak.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

struct SensorData
{
	bool IsUpdated = false;
	bool IsMetric = true;
    float Temp = 0.0; //temperature
    float Hmd = 0.0; //humidity
    float Lux = 0.0; //light
    float HPa = 0.0; //atmospheric
};

extern void initSensors();

extern void readExternalSensorsData(unsigned long channelID, SensorData *sensorData);

extern void readInternalSensors(SensorData *sensorData);

#endif