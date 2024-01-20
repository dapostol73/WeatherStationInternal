#include "ApplicationSensor.h"

WiFiClient client;

#ifdef SHT_3X
	// SHTSensor SHT3X (0x44) or SHT3X_ALT (0x45)
	Adafruit_SHT31 sht3x;
	// Sensor Home: -4.1
	// Sensor Whistler: -4.6
	#define SHT_TEMPOFFSET -4.6
	// Sensor Home: 9.0
	// Sensor Whistler: 8.0
	#define SHT_HMDOFFSET 8.0
#endif
#ifdef BME_280
	const float SEALEVELPRESSURE_HPA = 1021.1;
	Adafruit_BME280 bme280;
	// The BME sensor temp offset as tested. 
	// Sensor(S) 1: -1.5
	// Sensor(s) 2: -2.2
	#define BME_TEMPOFFSET -2.2
	// Sensor: 1 Low: 31.1 High: 84.6
	// Sensor: 2 Low: 31.7 High: 78.0
	#define BME_HMDLOW_T 32.0
	#define BME_HMDHIGH_T 84.0
	#define BME_HMDLOW_S 31.7
	#define BME_HMDHIGH_S 78.0
#endif
#ifdef DHT_22
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
#endif

void initSensors()
{
	#ifdef SHT_3X
		if (!sht3x.begin())
		{
			Serial.println("Could not find SHT3X-DIS sensor");
		}
	#endif
	#ifdef BME_280
		if (!bme280.begin(BME280_ADDRESS_ALTERNATE))
		{
			Serial.println("Could not find BME280 sensor at 0x76");
		}
	#endif
	#ifdef DHT_22
		dht22.begin();
	#endif
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
	sensorData->Alt = ThingSpeak.readFloatField(channelID, 5);

	// Check the status of the read operation to see if it was successful
	statusCode = ThingSpeak.getLastReadStatus();
	if(statusCode == 200)
	{
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.println("Reading from ThinkSpeak " + String(channelID));
		Serial.println("Temperature: " + String(sensorData->Temp) + " °C");
		Serial.println("Humidity: " + String(sensorData->Hmd) + " %");
		Serial.println("Light: " + String(sensorData->Lux) + " lux");
		Serial.println("Atmosphere: " + String(sensorData->HPa) + " hPa");
		Serial.println("Altitude: " + String(sensorData->Alt) + " m");
		#endif
	}
	else
	{
		sensorData->IsUpdated = false;
		Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
	}
}

#ifdef SHT_3X
void readInternalSensors(SensorData *sensorData)
{
	sensorData->IsUpdated = false;
	sensorData->Temp = roundUpDecimal(sht3x.readTemperature() + SHT_TEMPOFFSET);
	sensorData->Hmd = roundUpDecimal(sht3x.readHumidity() + SHT_HMDOFFSET);
	sensorData->IsUpdated = true;

	#ifdef SERIAL_LOGGING
	Serial.println("Updated Internal Sensor Data");
	Serial.println("Temperature: " + String(sensorData->Temp) + " °C");
	Serial.println("Humidity: " + String(sensorData->Hmd) + " %");
	#endif
}
#endif
#ifdef BME_280
void readInternalSensors(SensorData *sensorData)
{
	// BME simple read, saves 48 bytes RAM and 1,440 bytes of Flash
	//sensorData->Temp = roundUpDecimal(bme280.readTemperature() + BME_TEMPOFFSET);
	//sensorData->Hmd = map(bme280.readHumidity(), BME_HMDLOW_S, BME_HMDHIGH_S, BME_HMDLOW_T, BME_HMDHIGH_T);
	//sensorData->Hmd = roundUpDecimal(sensorData->Hmd);
	//sensorData->HPa = roundUpDecimal(bme280.readPressure() / 100.0F);
	//sensorData->Alt = roundUpDecimal(bme280.readAltitude(SEALEVELPRESSURE_HPA));
	// approx low from https://vancouver.weatherstats.ca/charts/pressure_sea-hourly.html

	sensors_event_t event;
	bme280.getTemperatureSensor()->getEvent(&event);
	if (isnan(event.temperature))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Error reading temperature!"));
		#endif
		sensorData->IsUpdated = false;		
	}
	else
	{
		sensorData->Temp = roundUpDecimal(event.temperature + BME_TEMPOFFSET);
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.print(F("Temperature: "));
		Serial.print(sensorData->Temp);
		Serial.println(F("°C"));
		#endif
	}

  	bme280.getHumiditySensor()->getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Error reading humidity!"));
		#endif
		sensorData->IsUpdated = false;
	}
	else
	{
		sensorData->Hmd = map(event.relative_humidity, BME_HMDLOW_S, BME_HMDHIGH_S, BME_HMDLOW_T, BME_HMDHIGH_T);
		sensorData->Hmd = roundUpDecimal(sensorData->Hmd);
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.print(F("Humidity: "));
		Serial.print(sensorData->Hmd);
		Serial.println(F("%"));
		#endif
	}

	bme280.getPressureSensor()->getEvent(&event);
	if (isnan(event.pressure))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Error reading pressure!"));
		#endif
		sensorData->IsUpdated = false;
	}
	else
	{
		sensorData->HPa = roundUpDecimal(event.pressure / 100.0F);
		// approx low from https://vancouver.weatherstats.ca/charts/pressure_sea-hourly.html
		sensorData->Alt = roundUpDecimal(bme280.readAltitude(SEALEVELPRESSURE_HPA));
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.print(F("Pressure: "));
		Serial.print(sensorData->HPa);
		Serial.println(F(" HPa"));
		#endif
	}
}
#endif
#ifdef DHT_22
void readInternalSensors(SensorData *sensorData)
{
	sensors_event_t event;
	dht22.temperature().getEvent(&event);
	if (isnan(event.temperature))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Error reading temperature!"));
		#endif
		sensorData->IsUpdated = false;
	}
	else
	{
		sensorData->Temp = roundUpDecimal(event.temperature + DHT_TEMPOFFSET);
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.print(F("Temperature: "));
		Serial.print(sensorData->Temp);
		Serial.println(F("°C"));
		#endif
	}

	dht22.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Error reading humidity!"));
		#endif
		sensorData->IsUpdated = false;
	}
	else
	{
		sensorData->Hmd = map(event.relative_humidity, DHT_HMDLOW_S, DHT_HMDHIGH_S, DHT_HMDLOW_T, DHT_HMDHIGH_T);
		sensorData->Hmd = roundUpDecimal(sensorData->Hmd);
		sensorData->IsUpdated = true;
		#ifdef SERIAL_LOGGING
		Serial.print(F("Humidity: "));
		Serial.print(sensorData->Hmd);
		Serial.println(F("%"));
		#endif
	}
}
#endif