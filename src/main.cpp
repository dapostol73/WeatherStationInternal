// Firmware for ESP8266
// Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
// This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796
// For the screen to work, you need to uncomment the following in MCUFRIEND_kbv Library
// In utility\mcufriend_shield.h uncomment #define USE_SPECIAL
// In utility\mcufriend_special.h uncomment #define USE_MEGA_8BIT_PORTC_SHIELD

#include <Arduino.h>
#include <limits.h>
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
#include <TFT_Touch.h>

#include "ApplicationSettings.h"
#include "DisplayWeather.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

//#define DEBUG
#define SERIAL_BAUD_RATE 115200
#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

ApplicationSettings appSettings; //change to pointer

//************************//
// Begin DHT22 Settings   //
//************************//
#define DHTPIN 13     // Digital pin connected to the DHT sensor 
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT_Unified dht22(DHTPIN, DHTTYPE);
float tempTemp = 0.0; //temperature
float tempHmd = 0.0; //humidity
long readTempHmdTime = LONG_MIN;
void readTemperatureHumidity();

//**************************//
// Begin TFT_Touch Settings //
//**************************//
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
TFT_Touch touch(TP_CS, TP_CLK, TP_IN, TP_OUT);
long lastTouchTime = LONG_MIN;

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

void drawTempratureHumidityFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawCurrentWeatherFrame, drawForecastFrame, drawTempratureHumidityFrame };
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

/***************************
 * Begin Settings
 **************************/
// Setup
const int SENSOR_INTERVAL_SECS = 60; // Sensor query every 15 seconds
const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes
const int CURRENT_INTERVAL_SECS = 10 * 60; // Update every 10 minutes
const int FORECAST_INTERVAL_SECS = 60 * 60; // Update every 60 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

// flag changed in the ticker function every 10 minutes
time_t lastUpdated;
bool updateCurrentWeather = true;
bool updateForecastWeather = true;
long timeSinceCurrentUpdate = LONG_MIN;
long timeSinceForecastUpdate = LONG_MIN;

void updateSystemTime();
bool updateData();
void configureWiFi();
void printConnectInfo();

#ifdef DEBUG
#define IRQ_PIN 44
volatile bool test = false;
void interruptServiceRoutine()
{
  // code to be executed when the interrupt occurs
  test = true;
}

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	while (!Serial)
		;

	displayControl.init();
	displayControl.fillScreen(BLACK);
	touch.setRotation(1);
	touch.setResolution(480, 320);

	pinMode(IRQ_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(IRQ_PIN), interruptServiceRoutine, CHANGE);
}

void loop()
{
	if (test)
	{
		displayControl.drawString("Success!", 240, 160, TEXT_CENTER_MIDDLE, ORANGE);
		test = false;
		delay(2500);
		displayControl.fillScreen(BLACK);
	}
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
	displayProgress.corner = 20;
	displayProgress.foregroundColor = CYAN;
	displayProgress.gfxFont = &CalibriBold8pt7b;
	displayControl.init();

	displayControl.setProgress(&displayProgress);
	displayControl.setFrames(frames, numberOfFrames);
	displayControl.setOverlays(overlays, numberOfOverlays);

	displayControl.fillScreen(BLACK);
	displayControl.drawWeatherIcon(40 ,  80, "01d", true);
	displayControl.drawWeatherIcon(140,  80, "02d", true);
	displayControl.drawWeatherIcon(240,  80, "03d", true);
	displayControl.drawWeatherIcon(340,  80, "04d", true);
	displayControl.drawWeatherIcon(440,  80, "09d", true);
	displayControl.drawWeatherIcon(40 , 200, "10d", true);
	displayControl.drawWeatherIcon(140, 200, "11d", true);
	displayControl.drawWeatherIcon(240, 200, "13d", true);
	displayControl.drawWeatherIcon(340, 200, "50d", true);
	displayControl.drawWeatherIcon(440, 200, "00d", true);

	touch.setRotation(1);
	touch.setResolution(480, 320);
	configureWiFi();
	dht22.begin();
	timeClient.begin();
}

void loop()
{
	if (touch.Pressed() && millis() - lastTouchTime > 100)
	{
		unsigned int X_Raw = touch.RawX();
		//unsigned int Y_Raw = touch.RawY();

		if (X_Raw > 2048)
		{
			displayControl.navigateForwardFrame();
		}
		else
		{
			displayControl.navigateBackwardFrame();			
		}
		lastTouchTime = millis();
	}

	//Read sensor values base on Upload interval seconds
	if(millis() - readTempHmdTime > (1000L*SENSOR_INTERVAL_SECS))
	{
		readTemperatureHumidity();
		readTempHmdTime = millis();
	}

	if (millis() - timeSinceForecastUpdate > (1000L*FORECAST_INTERVAL_SECS))
	{
		Serial.println("Setting updateForecastWeather to true");
		updateForecastWeather = true;
		timeSinceForecastUpdate = millis();
	}

	if (millis() - timeSinceCurrentUpdate > (1000L*CURRENT_INTERVAL_SECS))
	{
		Serial.println("Setting updateCurrentWeather to true");
		updateCurrentWeather = true;
		timeSinceCurrentUpdate = millis();
	}

	if ((updateCurrentWeather || updateForecastWeather) && !updateData()) 
	{
		Serial.println("Update failed, refreshing WiFi connection.");
		WiFi.disconnect();
		delay(3000);
		WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);

		int timeout = 0;
		int timeoutMax = 30;
		Serial.print("Connecting to WiFi");
		while (WiFi.status() != WL_CONNECTED && timeout < timeoutMax)
		{
			delay(1000);
			Serial.print('.');
			++timeout;
		}
		Serial.println();

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

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

bool updateData() 
{
	displayProgress.foregroundColor = CYAN;
	displayControl.drawProgress(25, "Updating time...");
	updateSystemTime();

	if (updateCurrentWeather)
	{
		displayControl.drawProgress(50, "Updating weather...");
		currentWeatherClient.setMetric(IS_METRIC);
		currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
		currentWeatherUpdated = currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);
		if (!currentWeatherUpdated)
		{
			displayProgress.foregroundColor = RED;
		}
		updateCurrentWeather = !currentWeatherUpdated; 
	}

	if (updateForecastWeather)
	{
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
		updateForecastWeather = !forecastWeatherUpdated;
	}

	lastUpdated = now();
	displayControl.drawProgress(100, "Updating done!");
	delay(1000);
	return currentWeatherUpdated && forecastWeatherUpdated;
}

void resolveAppSettings()
{
	int8_t numNetworks = WiFi.scanNetworks();
	Serial.println("Number of networks found: " + String(numNetworks));

	for (uint8_t i=0; i<numNetworks; i++)
	{
		String ssid = WiFi.SSID(i);
		Serial.println(ssid + " (" + String(WiFi.RSSI(i)) + ")");
		for (uint8_t j=0; j < AppSettingsCount; j++)
		{
			const char* appSsid = AppSettings[j].WifiSettings.SSID;
			Serial.println("Checking: " + String(appSsid));
			if (strcasecmp(appSsid, ssid.c_str()) == 0)
			{
				//Serial.println("Found: " + String(ssid));
				AppSettings[j].WifiSettings.Avialable = true;
				AppSettings[j].WifiSettings.Strength = WiFi.RSSI(i);

				if (AppSettings[j].WifiSettings.Strength > appSettings.WifiSettings.Strength)
				{
					appSettings = AppSettings[j];
				}
			}
		}
	}

	Serial.println("Using WiFi " + String(appSettings.WifiSettings.SSID));	
}

void configureWiFi()
{
	String intialMsg = "Intializing WiFi module.";
	Serial.println(intialMsg);
	displayControl.drawProgress(10, intialMsg);
	Serial3.begin(SERIAL_BAUD_RATE);
	
	WiFi.init(&Serial3);

	if (WiFi.status() == WL_NO_SHIELD)
	{
		String initialErr = "Communication with WiFi module failed!";
		Serial.println(initialErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(initialErr, 240, 160, TEXT_CENTER_MIDDLE, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	String scanMsg = "Scanning for WiFi SSID.";
	Serial.println(scanMsg);
	displayControl.drawProgress(30, scanMsg);
	resolveAppSettings();

	String infoMsg = "Waiting for connection to WiFi";
	if (!appSettings.WifiSettings.Avialable)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "No WiFi connecttion found %s!", appSettings.WifiSettings.SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER_MIDDLE, BLACK, RED);
		while (true)
			;
	}

	WiFi.setAutoConnect(true);
	WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);
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
		sprintf(connectErr, "WiFi failed to connect to %s access point!", appSettings.WifiSettings.SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER_MIDDLE, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	WiFi.sleepMode(WIFI_NONE_SLEEP);

	char ssidInfo[42] = "";
	sprintf(ssidInfo, "Connected to WiFi: %s", appSettings.WifiSettings.SSID);
	displayControl.drawProgress(90, ssidInfo);
	delay(500);
		
	printConnectInfo();
	displayControl.drawProgress(100, "WiFi initialization done!");
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
	sprintf(ssidInfo, "WiFi SSID: %s", appSettings.WifiSettings.SSID);
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
		tempTemp = roundUpDecimal(event.temperature-4.8);
		//Serial.print(F("Temperature: "));
		//Serial.print(tempTemp);
		//Serial.println(F("°C"));
	}

	dht22.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		Serial.println(F("Error reading humidity!"));
	}
	else
	{
		//tempHmd = roundUpDecimal(event.relative_humidity*0.9341+22.319);
		tempHmd = map(event.relative_humidity, 20.6, 69.5, 40.0, 75.0);
		tempHmd = roundUpDecimal(tempHmd);
		//Serial.print(F("Humidity: "));
		//Serial.print(tempHmd);
		//Serial.println(F("%"));
	}
}

void drawTempratureHumidityFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.drawTempratureHumidity(x, y, tempTemp, tempHmd, 0.0, 0.0);
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