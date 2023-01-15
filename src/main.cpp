// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54
// need to see if we can flash to this version to fix issues https://github.com/espressif/ESP8266_NONOS_SDK/
// This looks correct: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640
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

#define SERIAL_BAUD_RATE 115200

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
const uint8_t MAX_FORECASTS = 3;

/*
// Adjust according to your language
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
*/
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;
bool currentWeatherUpdated = false;

OpenWeatherMapForecastData forecastWeather[MAX_FORECASTS];
OpenWeatherMapForecast forecastWeatherClient;
bool forecastWeatherUpdated = false;

DisplayWeather displayControl;
DisplayContolProgress displayProgress;
/*
uint16_t palette[] = {BLACK, WHITE, GOLD, DEEPSKYBLUE};
DisplayControl displayControl;
DisplayContolProgress displayProgress;

void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center = false, int16_t scale = 1);
*/
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
bool readyForUpdate = false;
time_t lastUpdated;
long timeSinceLastWUpdate = 0;

void setReadyForUpdate();
void updateSystemTime();
void updateData();
void configureWifi();
void printConnectInfo();

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
	updateData();
	displayControl.fillScreen(BLACK);
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

	if (readyForUpdate) 
	{
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

void updateData() 
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
	//delay(1000);
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
}

void configureWifi()
{
	String intialMsg = "Intializing Wifi module.";
	Serial.println(intialMsg);
	displayControl.drawProgress(20, intialMsg);
	Serial3.begin(SERIAL_BAUD_RATE);
	WiFi.init(&Serial3);

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

	String infoMsg = "Waiting for connection to WiFi";
	WiFi.begin(NAME_OF_SSID, PASSWORD_OF_SSID);
	Serial.println(infoMsg);
	displayControl.drawProgress(40, infoMsg);

	int counter = 0;
	int timeout = 0;
	int timeoutMax = 30;

	while (WiFi.status() != WL_CONNECTED && timeout < timeoutMax)
	{
		delay(1000);
		Serial.print('.');
		displayControl.drawProgress(60, "Connecting to WiFi");
		counter++;
		++timeout;
	}
	Serial.println();

	if (WiFi.status() == WL_DISCONNECTED)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "Wifi failed to connect to %s acces point!", NAME_OF_SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	WiFi.sleepMode(WIFI_NONE_SLEEP);

	char wifiInfo[42] = "";
	sprintf(wifiInfo, "Connected to Wifi: %s", NAME_OF_SSID);
	displayControl.fillScreen(BLACK);
	displayControl.drawProgress(80, wifiInfo);
		
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
	char wifiInfo[34] = "";
	sprintf(wifiInfo, "Wifi: %s", NAME_OF_SSID);
	Serial.println(wifiInfo);
	displayControl.printLine(wifiInfo, GREEN);

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