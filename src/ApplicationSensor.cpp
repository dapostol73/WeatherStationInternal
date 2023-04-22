#include "ApplicationSensor.h"

WiFiClient client;

//******************************//
// Internal DHT Settings        //
//******************************//
#define DHTPIN 13     // Digital pin connected to the DHT sensor 
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht22(DHTPIN, DHTTYPE);
// The DHT sensor temp offset as tested. 
// Sensor(S) 1: -5.0
// Sensor(s) 2: -6.1
#define DHT_TEMPOFFSET -6.1
// define the DHT mapping for target(T) low 32% and high 84% 
// Sensor(S) 1: Low 35.4, High 85.9
// Sensor(s) 2: Low 34.9, High 87.4
#define DHT_HMDLOW_T 32.0
#define DHT_HMDHIGH_T 84.0
#define DHT_HMDLOW_S 34.9
#define DHT_HMDHIGH_S 97.4

void initSensors()
{
	dht22.begin();
	ThingSpeak.begin(client);    
}

float roundUpDecimal(float value, int decimals = 1) 
{
	float multiplier = powf(10.0, decimals);
	value = roundf(value * multiplier) / multiplier;
	return value;
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void readExternalSensorsData(unsigned long channelID, SensorData *sensorData)
{
	int statusCode = 0;
	// Read in fields of the public channel recording the temperature
	// 1: Temperature °C, 2: Humidity %, 3: Light lux, 4: Atmosphere hPa, 5: Altitude M
	sensorData->Temp = ThingSpeak.readFloatField(channelID, 1);
	sensorData->Hmd = ThingSpeak.readFloatField(channelID, 2); 
	sensorData->Lux = ThingSpeak.readFloatField(channelID, 3); 
	sensorData->HPa = ThingSpeak.readFloatField(channelID, 4); 

	// Check the status of the read operation to see if it was successful
	statusCode = ThingSpeak.getLastReadStatus();
	if(statusCode == 200)
	{
		sensorData->IsUpdated = true;
		//Serial.println("Reading from ThinkSpeak " + String(channelID));
		//Serial.println("Temperature: " + String(externalTemp) + " °C");
		//Serial.println("Humidity: " + String(externalHmd) + " %");
		//Serial.println("Light: " + String(externalLux) + " lux");
		//Serial.println("Atmosphere: " + String(externalHPa) + " hPa");
	}
	else
	{
		sensorData->IsUpdated = false;
		//Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
	}
}

void readInternalSensors(SensorData *sensorData)
{
	sensors_event_t event;
	dht22.temperature().getEvent(&event);
	if (isnan(event.temperature))
	{
		//Serial.println(F("Error reading temperature!"));
		sensorData->IsUpdated = false;
	}
	else
	{
		sensorData->Temp = roundUpDecimal(event.temperature + DHT_TEMPOFFSET);
		sensorData->IsUpdated = true;
		//Serial.print(F("Temperature: "));
		//Serial.print(sensorData->Temp);
		//Serial.println(F("°C"));
	}

	dht22.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		//Serial.println(F("Error reading humidity!"));
		sensorData->IsUpdated = false;
	}
	else
	{
		//internalHmd = roundUpDecimal(event.relative_humidity*0.9341+22.319);
		sensorData->Hmd = map(event.relative_humidity, DHT_HMDLOW_S, DHT_HMDHIGH_S, DHT_HMDLOW_T, DHT_HMDHIGH_T);
		sensorData->Hmd = roundUpDecimal(sensorData->Hmd);
		sensorData->IsUpdated = true;
		//Serial.print(F("Humidity: "));
		//Serial.print(sensorData->Hmd);
		//Serial.println(F("%"));
	}
}