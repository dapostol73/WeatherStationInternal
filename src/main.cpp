// Firmware for ESP8266
// Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
// This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796
// For the screen to work, you need to uncomment the following in MCUFRIEND_kbv Library
// In utility\mcufriend_shield.h uncomment #define USE_SPECIAL
// In utility\mcufriend_special.h uncomment #define USE_MEGA_8BIT_PORTC_SHIELD

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiEspAT.h>
#include <WiFiUdp.h>

// time
#include <TimeLib.h>
#include <NTPClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "DisplayWeather.h"
#include "WifiInfo.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

//#define DEBUG
#define SERIAL_BAUD_RATE 115200

WiFiConnection wifiInfo("Unknown", "Invalid");

/***************************
 * Begin DHT11 Settings
 **************************/
#define DHTTYP DHT22 // DHT 22 (AM2302)
#define DHTPIN 8    // Digital pin connected to the DHT sensor
DHT_Unified dht22(DHTPIN, DHTTYP);
float tempTemp = 0.0; //temperature
float tempHumi = 0.0; //humidity
long readTime = 0;
void readTemperatureHumidity();

// ThingSpeak Settings
WiFiClient client;
const char *host = "api.thingspeak.com";                  //IP address of the thingspeak server
const char *api_key ="EMCNAORN3ZXKCFW1";                  //Your own thingspeak api_key
const int httpPort = 80;

// OpenWeatherMap Settings
// Sign up here to get an API key:
// https://docs.thingpulse.com/how-tos/openweathermap-key/
const boolean IS_METRIC = true;
// Add your own thingpulse ID 
String OPEN_WEATHER_MAP_APP_ID = "6ef72f6a0b2b7849e74ac530ce47d067";
String OPEN_WEATHER_MAP_LOCATION = "6090785";

// Pick a language code from this list:
// Arabic - ar, Bulgarian - bg, Catalan - ca, Czech - cz, German - de, Greek - el,
// English - en, Persian (Farsi) - fa, Finnish - fi, French - fr, Galician - gl,
// Croatian - hr, Hungarian - hu, Italian - it, Japanese - ja, Korean - kr,
// Latvian - la, Lithuanian - lt, Macedonian - mk, Dutch - nl, Polish - pl,
// Portuguese - pt, Romanian - ro, Russian - ru, Swedish - se, Slovak - sk,
// Slovenian - sl, Spanish - es, Turkish - tr, Ukrainian - ua, Vietnamese - vi,
// Chinese Simplified - zh_cn, Chinese Traditional - zh_tw.

String OPEN_WEATHER_MAP_LANGUAGE = "en";
const uint8_t MAX_FORECASTS = 4;

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;
bool currentWeatherUpdated = false;

OpenWeatherMapForecastData forecastWeather[MAX_FORECASTS];
OpenWeatherMapForecast forecastWeatherClient;
bool forecastWeatherUpdated = false;

DisplayWeather displayControl;
DisplayContolProgress displayProgress;

void drawDateTimeFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawForecastFrame, drawCurrentWeatherFrame, drawDateTimeFrame };
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

/***************************
 * Begin Settings
 **************************/
// Setup
const int SENSOR_INTERVAL_SECS = 15; // Sensor query every 15 seconds
const int TIME_INTERVAL_SECS = 10 * 60; // Check time every 10 minutes
const int UPDATE_INTERVAL_SECS = 60 * 60; // Update every 60 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

// flag changed in the ticker function every 10 minutes
bool readyForUpdate = true;
time_t lastUpdated;
long timeSinceLastWUpdate = 0;

void setReadyForUpdate();
void updateSystemTime();
bool updateData();
void configureWifi();
void printConnectInfo();

#ifdef DEBUG

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;

	displayControl.init();
	int16_t size = 2;
	int16_t x = 60;
	int16_t y = 20;
	//displayControl.drawSun(30*size+x, 0+y, 16, size);
	//displayControl.drawCloud(0+x, 5*size+y, size);
	//displayControl.drawThunder(20*size+x, 33*size+y, size);
	//displayControl.drawRainLight(12*size+x, 42*size+y, size);
	//displayControl.drawRainHeavy(14*size+x, 42*size+y, size);
	//displayControl.drawSnow(12*size+x, 44*size+y, size);
	displayControl.drawFog(size+x, size+y, size);
}

void loop()
{
	
}

#else

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;

	displayProgress.x = 0;
	displayProgress.y = 280;
	displayProgress.width = 480;
	displayProgress.height = 40;
	displayProgress.padding = 5;
	displayProgress.corner = 10;
	displayProgress.foregroundColor = CYAN;
	displayProgress.gfxFont = &CalibriBold8pt7b;
	displayControl.init();

	displayControl.setProgress(&displayProgress);
	displayControl.setFrameAnimation(SLIDE_LEFT);
	displayControl.setFrames(frames, numberOfFrames);
	displayControl.setOverlays(overlays, numberOfOverlays);

	displayControl.fillScreen(BLACK);
	displayControl.drawWeatherIcon(40 ,  70, "01d", true);
	displayControl.drawWeatherIcon(140,  70, "02d", true);
	displayControl.drawWeatherIcon(240,  70, "03d", true);
	displayControl.drawWeatherIcon(340,  70, "04d", true);
	displayControl.drawWeatherIcon(440,  70, "09d", true);
	displayControl.drawWeatherIcon(90 , 190, "10d", true);
	displayControl.drawWeatherIcon(190, 190, "11d", true);
	displayControl.drawWeatherIcon(290, 190, "13d", true);
	displayControl.drawWeatherIcon(390, 190, "50d", true);
	displayControl.getDisplay()->drawFastHLine(0, 278, 480, CYAN);
	displayControl.getDisplay()->drawFastHLine(0, 279, 480, CYAN);

	configureWifi();
	timeClient.begin();
	//updateData();
	//displayControl.fillScreen(BLACK);
}

void loop()
{	
	//Read sensor values base on Upload interval seconds
	if(millis() - readTime > SENSOR_INTERVAL_SECS){
		//readTemperatureHumidity();
		readTime = millis();
	}

	if (millis() - timeSinceLastWUpdate > (1000L*UPDATE_INTERVAL_SECS)) {
		setReadyForUpdate();
		timeSinceLastWUpdate = millis();
	}

	if (readyForUpdate && !updateData()) 
	{
		Serial.println("Update failed, try again with reseting the WiFi.");
		WiFi.reset();
		updateData();
	}

	int remainingTimeBudget = displayControl.update();

	if (remainingTimeBudget > 0) 
	{
		// You can do some work here
		// Don't do stuff if you are below your
		// time budget.
		delay(remainingTimeBudget);
	}
}

#endif

float roundUpDecimal(float value, int decimals = 1) 
{
	float multiplier = powf(10.0, decimals);
	value = roundf(value * multiplier) / multiplier;
	return value;
}

void setReadyForUpdate() 
{
	Serial.println("Setting readyForUpdate to true");
	readyForUpdate = true;
}

void updateSystemTime()
{
	timeClient.forceUpdate();
	setTime((time_t)timeClient.getEpochTime());
}

bool updateData() 
{
	displayControl.drawProgress(25, "Updating time...");
	updateSystemTime();

	displayControl.drawProgress(50, "Updating weather...");
	currentWeatherClient.setMetric(IS_METRIC);
	currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	currentWeatherUpdated = currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);
	if (!currentWeatherUpdated)
	{
		displayProgress.foregroundColor = RED;
	}

	displayControl.drawProgress(75, "Updating forecasts...");
	forecastWeatherClient.setMetric(IS_METRIC);
	forecastWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	uint8_t allowedHours[] = {12};
	forecastWeatherClient.setAllowedHours(allowedHours, sizeof(allowedHours));
	forecastWeatherUpdated = forecastWeatherClient.updateForecastsById(forecastWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
	if (!forecastWeatherUpdated)
	{
		displayProgress.foregroundColor = RED;
	}
	
	lastUpdated = now();
	readyForUpdate = false;
	displayControl.drawProgress(100, "Updating done!");
	delay(1000);
	return currentWeatherUpdated && forecastWeatherUpdated;
}

void resolveWifiInfo()
{
	int8_t numNetworks = WiFi.scanNetworks();

	for (int8_t i=0; i<numNetworks; i++)
	{
		const char* ssid = WiFi.SSID(i);
		//Serial.println("SSID: " + String(ssid) + ": " + String(WiFi.RSSI(i)));
		for (unsigned int j=0; j < sizeof(WiFiConnections); j++)
		{
			if (strcasecmp(WiFiConnections[j].SSID, ssid) == 0)
			{
				//Serial.println("Found: " + String(ssid));
				WiFiConnections[j].Avialable = true;
				WiFiConnections[j].Strength = WiFi.RSSI(i);

				if (WiFiConnections[j].Strength > wifiInfo.Strength)
				{
					wifiInfo = WiFiConnections[j];
				}
			}
		}
	}

	
}

void configureWifi()
{
	String intialMsg = "Intializing Wifi module.";
	Serial.println(intialMsg);
	displayControl.drawProgress(10, intialMsg);
	Serial3.begin(SERIAL_BAUD_RATE);
	
	WiFi.init(&Serial3);
	WiFi.setAutoConnect(true);

	if (WiFi.status() == WL_NO_SHIELD)
	{
		String initialErr = "Communication with WiFi module failed!";
		Serial.println(initialErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(initialErr, 240, 160, TEXT_CENTER, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	String scanMsg = "Scanning for Wifi SSID.";
	Serial.println(scanMsg);
	displayControl.drawProgress(30, scanMsg);
	resolveWifiInfo();

	String infoMsg = "Waiting for connection to WiFi";
	if (!wifiInfo.Avialable)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "No Wifi connecttion found %s!", wifiInfo.SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER, BLACK, RED);
		while (true)
			;
	}

	WiFi.begin(wifiInfo.SSID, wifiInfo.Password);
	Serial.println(infoMsg);
	displayControl.drawProgress(50, infoMsg);

	int counter = 0;
	int timeout = 0;
	int timeoutMax = 30;

	while (WiFi.status() != WL_CONNECTED && timeout < timeoutMax)
	{
		delay(1000);
		Serial.print('.');
		displayControl.drawProgress(70, "Connecting to WiFi");
		counter++;
		++timeout;
	}
	Serial.println();

	if (WiFi.status() == WL_DISCONNECTED)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "Wifi failed to connect to %s access point!", wifiInfo.SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	WiFi.sleepMode(WIFI_NONE_SLEEP);

	char ssidInfo[42] = "";
	sprintf(ssidInfo, "Connected to Wifi: %s", wifiInfo.SSID);
	displayControl.drawProgress(90, ssidInfo);
	delay(500);
		
	printConnectInfo();
	displayControl.drawProgress(100, "Wifi initialization done!");
	delay(1000);
}

void printConnectInfo()
{
	// get your MAC address
	byte mac[6];
	WiFi.macAddress(mac);
	IPAddress ip = WiFi.localIP();

	// print MAC address
	char ssidInfo[34] = "";
	sprintf(ssidInfo, "Wifi SSID: %s", wifiInfo.SSID);
	Serial.println(ssidInfo);
	displayControl.printLine(ssidInfo, GREEN);

	// print MAC address
	char macInfo[34] = "";
	sprintf(macInfo, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
	Serial.println(macInfo);
	displayControl.printLine(macInfo, YELLOW);

	// print IP address
	char ipInfo[34] = "";
	sprintf(ipInfo, "IP address: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	Serial.println(ipInfo);
	displayControl.printLine(ipInfo, YELLOW);
}

void readTemperatureHumidity()
{
	sensors_event_t event;
	dht22.temperature().getEvent(&event);
	if (isnan(event.temperature))
	{
		Serial.println(F("Error reading temperature!"));
	}
	else
	{
		tempTemp = roundUpDecimal(event.temperature);
	}

	dht22.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		Serial.println(F("Error reading humidity!"));
	}
	else
	{
		tempHumi = roundUpDecimal(event.relative_humidity);
	}
}

void drawDateTimeFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.drawDateTime(x, y);
}

void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.drawCurrentWeather(&currentWeather, x, y);
}

void drawForecastFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.drawForecast(forecastWeather, x, y);
}

void drawHeaderOverlay(DisplayControlState* state)
{
	displayControl.drawHeader(currentWeatherUpdated, forecastWeatherUpdated, lastUpdated);
}

void drawFooterOverlay(DisplayControlState* state)
{
	displayControl.drawFooter(&currentWeather);
}