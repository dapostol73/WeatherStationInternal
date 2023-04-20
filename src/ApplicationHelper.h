#ifndef _APP_HELPER_
#define _APP_HELPER_

#include <Arduino.h>
#include <limits.h>
#include <TimeLib.h>
#include <TFT_Touch.h>
#include <WiFiEspAT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ThingSpeak.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "ApplicationSettings.h"

// LCDWIKI_TOUCH my_touch(53,52,50,51,44); // tcs, tclk, tdout, tdin, tirq
//Set the pins to the correct ones for your development shield or breakout board.
//This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
//pin usage as follow:
//             CS  CD  WR  RD  RST  D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10  D11  D12  D13  D14  D15 
//Arduino Mega 40  38  39  43  41   /   /   /   /   /   /   /   /   22  23  24   25   26   27   28   29
//             TP_IRQ  MOSI  MISO  TP_CS  EX_CLK
//Arduino Mega   44     51    50    53      52
#define TP_CS  53  /* Chip select pin (T_CS) of touch screen */
#define TP_CLK 52  /* Clock pin (T_CLK) of touch screen */
#define TP_IN  51  /* Data in pin (T_DIN) of touch screen */
#define TP_OUT 50  /* Data out pin (T_DO) of touch screen */
#define TP_IRQ 44  /* Interupt pin (T_IRQ) of touch screen */
#define HMIN 0
#define HMAX 4095
#define VMIN 0
#define VMAX 4095
#define HRES 800 /* Default screen resulution for X axis */
#define VRES 480 /* Default screen resulution for Y axis */
#define XYSWAP 1 // 0 or 1 (true/false)
TFT_Touch touch(TP_CS, TP_CLK, TP_IN, TP_OUT);
long lastTouchTime = LONG_MIN;

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

//******************************//
// Time Client Settings         //
//******************************//
const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

WiFiUDP ntpUDP;
WiFiClient client;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

enum TouchResult
{
	NONE,
	UPDATE,
	FORWARD,
	BACKWARD
};

struct SensorData
{
    float Temp = 0.0; //temperature
    float Hmd = 0.0; //humidity
    float Lux = 0.0; //light
    float HPa = 0.0; //atmospheric
};

void initHelpers()
{
	touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP);
	touch.setRotation(1);
	dht22.begin();
	timeClient.begin();
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

TouchResult touchTest()
{
    if (touch.Pressed() && millis() - lastTouchTime > 100)
	{
		uint16_t xValue = touch.X();
		uint16_t yValue = touch.Y();
		//Serial.println("Touch at X,Y: (" + String(xValue) + "," + String(yValue) +")" );

		if (xValue > 300 && xValue < 500 && yValue < 20)
		{
			return UPDATE;
		}
		else if (xValue > 500)
		{
			return FORWARD;
		}
		else if (xValue < 300)
		{
			return BACKWARD;		
		}
		lastTouchTime = millis();
	}

    return NONE;
}

void updateSystemTime()
{
	timeClient.update();
	setTime((time_t)timeClient.getEpochTime());
	int m = month();
	int d = day();
	int w = weekday();
	int h = hour();

	// Check to see if we are in Daylight Saving Time (DST)
	bool inDST = false;
	// Between April to October 
	if (m > 3 && m < 11) 
	{
		inDST = true;
  	}
	// After second Sunday in March
	if (m == 3 && (h + 24UL * d) >= (3 + 24UL * (14 - w))) 
	{
		inDST = true;
	}
	// After first Sunday in November
	if (m == 11 && (h + 24UL * d) < (1 + 24UL * (7 - w))) {
		inDST = true;
	}

	if (inDST)
	{
		adjustTime(3600);
	}
}

void readExternalSensorsData(ApplicationSettings *appSettings, SensorData *sensorData)
{
	int statusCode = 0;
	// Read in fields of the public channel recording the temperature
	// 1: Temperature °C, 2: Humidity %, 3: Light lux, 4: Atmosphere hPa, 5: Altitude M
	sensorData->Temp = ThingSpeak.readFloatField(appSettings->ThingSpeakSettings.ChannelID, 1);
	sensorData->Hmd = ThingSpeak.readFloatField(appSettings->ThingSpeakSettings.ChannelID, 2); 
	sensorData->Lux = ThingSpeak.readFloatField(appSettings->ThingSpeakSettings.ChannelID, 3); 
	sensorData->HPa = ThingSpeak.readFloatField(appSettings->ThingSpeakSettings.ChannelID, 4); 

	// Check the status of the read operation to see if it was successful
	statusCode = ThingSpeak.getLastReadStatus();
	if(statusCode == 200)
	{
		//Serial.println("Reading from ThinkSpeak " + String(appSettings->ThingSpeakSettings.ChannelID));
		//Serial.println("Temperature: " + String(externalTemp) + " °C");
		//Serial.println("Humidity: " + String(externalHmd) + " %");
		//Serial.println("Light: " + String(externalLux) + " lux");
		//Serial.println("Atmosphere: " + String(externalHPa) + " hPa");
	}
	else
	{
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
	}
	else
	{
		sensorData->Temp = roundUpDecimal(event.temperature + DHT_TEMPOFFSET);
		//Serial.print(F("Temperature: "));
		//Serial.print(sensorData->Temp);
		//Serial.println(F("°C"));
	}

	dht22.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		//Serial.println(F("Error reading humidity!"));
	}
	else
	{
		//internalHmd = roundUpDecimal(event.relative_humidity*0.9341+22.319);
		sensorData->Hmd = map(event.relative_humidity, DHT_HMDLOW_S, DHT_HMDHIGH_S, DHT_HMDLOW_T, DHT_HMDHIGH_T);
		sensorData->Hmd = roundUpDecimal(sensorData->Hmd);
		//Serial.print(F("Humidity: "));
		//Serial.print(sensorData->Hmd);
		//Serial.println(F("%"));
	}
}

#endif