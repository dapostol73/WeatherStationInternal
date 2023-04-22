// Firmware for ESP8266
// Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
// This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiEspAT.h>

#include "ApplicationSettings.h"
#include "ApplicationHelper.h"
#include "DisplayWeather.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
//#include "OpenWeatherMapOneCall.h"

//#define DEBUG
#define SERIAL_BAUD_RATE 115200
#ifdef HAVE_SERIAL1
#include "Software//Serial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

ApplicationSettings appSettings;

//******************************//
// Internal Sensor Settings     //
//******************************//
SensorData internalSensorData;
long timeSinceInternalUpdate = LONG_MIN;

//******************************//
// External ThinkSpeak Settings //
//******************************//
SensorData externalSensorData;
long timeSinceExternalUpdate = LONG_MIN;
bool updateExternalSensors = true;

//******************************//
// OpenWeather Map Settings     //
//******************************//
const uint8_t MAX_FORECASTS = 4;
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;
bool currentWeatherUpdated = false;

OpenWeatherMapForecastData forecastWeather[MAX_FORECASTS];
OpenWeatherMapForecast forecastWeatherClient;
bool forecastWeatherUpdated = false;

DisplayWeather displayWeather;
DisplayContolProgress displayProgress;

void drawSensorDataFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawCurrentWeatherFrame, drawForecastFrame, drawSensorDataFrame };
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

/***************************
 * Begin Settings
 **************************/
// Setup
const int INTSENSOR_INTERVAL_SECS = 60; // Sensor query every 15 seconds
const int EXTSENSOR_INTERVAL_SECS = 5 * 60; // Sensor query every 5 minutes
//const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes
const int CURRENT_INTERVAL_SECS = 10 * 60; // Update every 10 minutes
const int FORECAST_INTERVAL_SECS = 60 * 60; // Update every 60 minutes

// flag changed in the ticker function every 10 minutes
time_t lastUpdated;
bool updateCurrentWeather = true;
bool updateForecastWeather = true;
long timeSinceCurrentUpdate = LONG_MIN;
long timeSinceForecastUpdate = LONG_MIN;

void updateData();
void configureWiFi();

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
	//Serial.begin(SERIAL_BAUD_RATE);

	displayWeather.init();
	displayWeather.fillScreen(BLACK);
	touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP);
	touch.setRotation(1);

	//pinMode(IRQ_PIN, INPUT);
	//attachInterrupt(digitalPinToInterrupt(IRQ_PIN), interruptServiceRoutine, CHANGE);
	//displayWeather.drawTemperatureHumidity(0, 0, 21.5, 44.2, 11.2, 35.2);
	displayWeather.drawCurrentWeather(&currentWeather, 0, 0);
	displayWeather.drawFooter(&currentWeather);
}

void loop()
{
	;
}

#else

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);

	displayProgress.x = 0;
	displayProgress.y = 420;
	displayProgress.width = 800;
	displayProgress.height = 60;
	displayProgress.padding = 5;
	displayProgress.corner = 24;
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayProgress.gfxFont = &CalibriRegular16pt7b;
	displayWeather.init();

	displayWeather.setProgress(&displayProgress);
	displayWeather.setFrames(frames, numberOfFrames);
	displayWeather.setOverlays(overlays, numberOfOverlays);

	displayWeather.fillScreen(BLACK);
	displayWeather.drawString("Weather Station", 400, 200, TEXT_CENTER_MIDDLE, TEXT_TITLE_COLOR);
	displayWeather.drawWeatherIcon(100, 100, "00d", true, 2);
	displayWeather.drawWeatherIcon(250, 100, "01d", true, 2);
	displayWeather.drawWeatherIcon(400, 100, "02d", true, 2);
	displayWeather.drawWeatherIcon(550, 100, "03d", true, 2);
	displayWeather.drawWeatherIcon(700, 100, "04d", true, 2);
	displayWeather.drawWeatherIcon(100, 320, "09d", true, 2);
	displayWeather.drawWeatherIcon(250, 320, "10d", true, 2);
	displayWeather.drawWeatherIcon(400, 320, "11d", true, 2);
	displayWeather.drawWeatherIcon(550, 320, "13d", true, 2);
	displayWeather.drawWeatherIcon(700, 320, "50d", true, 2);

	configureWiFi();
	initHelpers();// needs WiFi
}

void loop()
{
	switch (touchTest())
	{
		case UPDATE:
			displayWeather.DisplayGFX->drawRect(300, 0, 100, 20, ERROR_COLOR);
			updateExternalSensors = updateCurrentWeather = updateForecastWeather = true;
			break;
		case FORWARD:
			displayWeather.navigateForwardFrame();
			break;
		case BACKWARD:
			displayWeather.navigateBackwardFrame();
			break;
		default:
			break;
	}

	if (WiFi.status() != WL_CONNECTED)
	{
		WiFi.disconnect();
		delay(3000);
		WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);
		for (int t; t < 10; t++)
		{
			displayWeather.DisplayGFX->fillCircle(400 - 45 + (t * 10), 10, 3, TEXT_MAIN_COLOR);
			delay(1000);
		}
		displayWeather.DisplayGFX->fillRect(400 - 50, 5, 100, 10, OVERLAY_COLOR);
		return;
	}

	if (millis() - timeSinceForecastUpdate > (1000L*FORECAST_INTERVAL_SECS))
	{
		//Serial.println("Setting updateForecastWeather to true");
		updateForecastWeather = true;
		timeSinceForecastUpdate = millis();
	}

	if (millis() - timeSinceCurrentUpdate > (1000L*CURRENT_INTERVAL_SECS))
	{
		//Serial.println("Setting updateCurrentWeather to true");
		updateCurrentWeather = true;
		timeSinceCurrentUpdate = millis();
	}

	if(millis() - timeSinceExternalUpdate > (1000L*EXTSENSOR_INTERVAL_SECS))
	{
		updateExternalSensors = true;
		timeSinceExternalUpdate = millis();
	}

	//Read sensor values base on Upload interval seconds
	if(millis() - timeSinceInternalUpdate > (1000L*INTSENSOR_INTERVAL_SECS))
	{
		readInternalSensors(&internalSensorData);
		timeSinceInternalUpdate = millis();
	}

	if (updateExternalSensors || updateCurrentWeather || updateForecastWeather) 
	{
		updateData();
	}

	int remainingTimeBudget = displayWeather.update();

	if (remainingTimeBudget > 0) 
	{
		// You can do some work here
		// Don't do stuff if you are below your
		// time budget.
		delay(remainingTimeBudget);
	}
}

#endif

void updateData() 
{
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayWeather.drawProgress(20, "Updating time...");
	updateSystemTime();
	if (updateExternalSensors)
	{
		displayWeather.drawProgress(40, "Updating external sensor data...");
		readExternalSensorsData(&appSettings, &externalSensorData);
		updateExternalSensors = false;
	}
	

	if (updateCurrentWeather)
	{
		displayWeather.drawProgress(60, "Updating weather...");
		currentWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		currentWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		currentWeatherUpdated = currentWeatherClient.updateCurrentById(&currentWeather, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location);
		if (!currentWeatherUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateCurrentWeather = !currentWeatherUpdated; 
	}

	if (updateForecastWeather)
	{
		displayWeather.drawProgress(80, "Updating forecasts...");
		forecastWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		forecastWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		uint8_t allowedHours[] = {12};
		forecastWeatherClient.setAllowedHours(allowedHours, sizeof(allowedHours));
		forecastWeatherUpdated = forecastWeatherClient.updateForecastsById(forecastWeather, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location, MAX_FORECASTS);
		if (!forecastWeatherUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateForecastWeather = !forecastWeatherUpdated;
	}

	lastUpdated = now();
	displayWeather.drawProgress(100, "Updating done!");
	delay(1000);
}

void resolveAppSettings()
{
	int8_t numNetworks = WiFi.scanNetworks();
	//Serial.println("Number of networks found: " + String(numNetworks));

	if (numNetworks == 0)
	{
		delay(2500);
		numNetworks = WiFi.scanNetworks();
	}

	for (uint8_t i=0; i<numNetworks; i++)
	{
		String ssid = WiFi.SSID(i);
		//Serial.println(ssid + " (" + String(WiFi.RSSI(i)) + ")");
		for (uint8_t j=0; j < AppSettingsCount; j++)
		{
			const char* appSsid = AppSettings[j].WifiSettings.SSID;
			//Serial.println("Checking: " + String(appSsid));
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

	//Serial.println("Using WiFi " + String(appSettings.WifiSettings.SSID));
	WiFi.disconnect();	
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
	//Serial.println(ssidInfo);
	displayWeather.printLine(ssidInfo, SUCCESS_COLOR);

	// print MAC address
	char macInfo[34] = "";
	sprintf(macInfo, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
	//Serial.println(macInfo);
	displayWeather.printLine(macInfo, TEXT_MAIN_COLOR);

	// print IP address
	char ipInfo[34] = "";
	sprintf(ipInfo, "IP address: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	//Serial.println(ipInfo);
	displayWeather.printLine(ipInfo, TEXT_MAIN_COLOR);
}

bool connectToWiFi(uint8_t retries)
{
	String scanMsg = "Scanning for WiFi SSID.";
	//Serial.println(scanMsg);
	displayWeather.drawProgress(30, scanMsg);
	resolveAppSettings();

	char infoMsg[] = "Waiting for connection to WiFi";
	if (!appSettings.WifiSettings.Avialable)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "No WiFi connections found for %s!", appSettings.WifiSettings.SSID);
		//Serial.println(connectErr);
		displayWeather.fillScreen(ERROR_COLOR);
		displayWeather.drawString(connectErr, 400, 240, TEXT_CENTER_MIDDLE, BLACK, ERROR_COLOR);
		return false;
	}

	WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);
	WiFi.sleepMode(WIFI_NONE_SLEEP);
	//Serial.println(infoMsg);
	displayWeather.drawProgress(50, infoMsg);

	uint8_t attempts = 0;
	uint8_t attemptMax = 3;

	sprintf(infoMsg, "Connecting to %s", appSettings.WifiSettings.SSID);
	while (WiFi.status() != WL_CONNECTED && attempts < attemptMax)
	{
		delay(1000);
		//Serial.print('.');
		displayWeather.drawProgress(65 + (attempts*5), infoMsg);
		++attempts;
	}
	//Serial.println();

	uint8_t retry = 0;
	while(WiFi.status() != WL_CONNECTED)
	{
		if (retry < retries)
		{
			connectToWiFi(0);
			++retry;
		}
		else
		{
			return false;
		}
		
	}

	return true;
}

void configureWiFi()
{
	String intialMsg = "Intializing WiFi module.";
	//Serial.println(intialMsg);
	displayWeather.drawProgress(10, intialMsg);
	Serial3.begin(SERIAL_BAUD_RATE);
	
	WiFi.init(&Serial3);
	delay(1000);
	//WiFi.endAP(true);
	WiFi.setAutoConnect(true);
	WiFi.setPersistent(false);

	if (WiFi.status() == WL_NO_SHIELD)
	{
		String initialErr = "Communication with WiFi module failed!";
		//Serial.println(initialErr);
		displayWeather.fillScreen(ERROR_COLOR);
		displayWeather.drawString(initialErr, 400, 240, TEXT_CENTER_MIDDLE, BLACK, ERROR_COLOR);
		// don't continue
		while (true)
			;
	}

	if (!connectToWiFi(2))
	{
		char connectErr[48] = "";
		sprintf(connectErr, "Failed to connect to %s WiFi!", appSettings.WifiSettings.SSID);
		//Serial.println(connectErr);
		displayWeather.fillScreen(ERROR_COLOR);
		displayWeather.drawString(connectErr, 400, 240, TEXT_CENTER_MIDDLE, BLACK, ERROR_COLOR);
		// don't continue
		while (true)
			;
	}

	char ssidInfo[42] = "";
	sprintf(ssidInfo, "Connected to WiFi: %s", appSettings.WifiSettings.SSID);
	displayWeather.drawProgress(90, ssidInfo);
	delay(500);
		
	printConnectInfo();
	displayWeather.drawProgress(100, "WiFi initialization done!");
	delay(1000);
}

void drawSensorDataFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawSensorData(x, y, &internalSensorData, &externalSensorData);
}

void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawCurrentWeather(&currentWeather, x, y);
}

void drawForecastFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawForecast(forecastWeather, x, y);
}

void drawHeaderOverlay(DisplayControlState* state)
{
	displayWeather.drawHeader(currentWeatherUpdated, forecastWeatherUpdated, lastUpdated);
}

void drawFooterOverlay(DisplayControlState* state)
{
	displayWeather.drawFooter(&externalSensorData, &currentWeather);
}