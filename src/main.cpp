// Firmware for ESP8266
// Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
// This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiEspAT.h>

#include "ApplicationSettings.h"
#include "ApplicationHelper.h"
#include "ApplicationSensor.h"
#include "DisplayWeather.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
//#include "OpenWeatherMapOneCall.h"

#define SERIAL_BAUD_RATE 115200
#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

//#define DEBUG
//#define SERIAL_LOGGING
#ifndef SERIAL_LOGGING
// disable Serial output
#define Serial KillDefaultSerial
static class {
public:
    void begin(...) {}
    void print(...) {}
    void println(...) {}
} Serial;
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
const uint8_t DAILY_MAX_FORECASTS = 4;
const uint8_t HOURLY_MAX_FORECASTS = 3;
const uint8_t DAILY_FORECAST_HOURS[] = { 15 };
const uint8_t HOURLY_FORECAST_HOURS[] = { 3, 9, 15, 21 };
OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;
bool updateCurrentWeather = true;
bool currentWeatherUpdated = false;

OpenWeatherMapForecastData forecastWeatherDaily[DAILY_MAX_FORECASTS];
OpenWeatherMapForecastData forecastWeatherHourlyQuerry[HOURLY_MAX_FORECASTS];
OpenWeatherMapForecastData forecastWeatherHourly[HOURLY_MAX_FORECASTS];
OpenWeatherMapForecast forecastWeatherClient;
bool updateForecastDailyWeather = true;
bool forecastWeatherDailyUpdated = false;
bool updateForecastHourlyWeather = true;
bool forecastWeatherHourlyUpdated = false;

DisplayWeather displayWeather;
DisplayContolProgress displayProgress;

void drawSensorDataFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastHourlyFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastDailyFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawCurrentWeatherFrame, drawForecastHourlyFrame, drawForecastDailyFrame, drawSensorDataFrame };
int numberOfFrames = 4;

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

/***************************
 * Begin Settings
 **************************/
// Time check setup, slighty off to easy the parsing.
const int INTSENSOR_INTERVAL_SECS = 60; // Sensor query every minute
const int EXTSENSOR_INTERVAL_SECS = 11 * 60; // Sensor query every 11 minutes
//const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes
const int CURRENT_INTERVAL_SECS = 23 * 60; // Update every 23 minutes
const int FORECAST_INTERVAL_SECS = 65 * 60; // Update every 65 minutes

bool updateSuccessed = true;
time_t lastUpdated;
long timeSinceCurrentUpdate = LONG_MIN;
long timeSinceForecastUpdate = LONG_MIN;

bool updateData();
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
	Serial.begin(SERIAL_BAUD_RATE);
	displayWeather.init();
	displayWeather.fillScreen(BLACK);

	displayWeather.DisplayGFX->drawFastVLine(300, 0, 480, RED);
	displayWeather.DisplayGFX->drawFastVLine(400, 0, 480, RED);
	displayWeather.DisplayGFX->drawFastVLine(500, 0, 480, RED);
	displayWeather.drawChar(300,  50, 'A', TEXT_LEFT_TOP);
	displayWeather.drawChar(300, 100, 'B', TEXT_CENTER_TOP);
	displayWeather.drawChar(300, 150, 'C', TEXT_RIGHT_TOP);
	displayWeather.drawChar(400, 50, 'D', TEXT_LEFT_MIDDLE);
	displayWeather.drawChar(400, 100, 'E', TEXT_CENTER_MIDDLE);
	displayWeather.drawChar(400, 150, 'F', TEXT_RIGHT_MIDDLE);
	displayWeather.drawChar(500, 50, 'G', TEXT_LEFT_BOTTOM);
	displayWeather.drawChar(500, 100, 'H', TEXT_CENTER_BOTTOM);
	displayWeather.drawChar(500, 150, 'I', TEXT_RIGHT_BOTTOM);
	//pinMode(IRQ_PIN, INPUT);
	//attachInterrupt(digitalPinToInterrupt(IRQ_PIN), interruptServiceRoutine, CHANGE);
	//displayWeather.drawTemperatureHumidity(0, 0, 21.5, 44.2, 11.2, 35.2);

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
	initSensors();
}

void loop()
{
	switch (touchTest())
	{
		case UPDATE:
			displayWeather.DisplayGFX->fillRoundRect(350, 5, 100, 10, 5, SUCCESS_COLOR);
			updateExternalSensors = updateCurrentWeather = updateForecastHourlyWeather = updateForecastDailyWeather = true;// force update
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

	if (WiFi.status() != WL_CONNECTED || !updateSuccessed)
	{
		WiFi.disconnect(true);
		WiFi.reset();
		delay(3000);
		WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);
		WiFi.sleepMode(WIFI_NONE_SLEEP);
		int t = 0;//tries
		while (WiFi.status() != WL_CONNECTED && t < 10)
		{
			displayWeather.DisplayGFX->fillCircle(400 - 45 + (t * 10), 10, 3, TEXT_MAIN_COLOR);
			delay(1000);
			t++;
		}
		updateExternalSensors = updateCurrentWeather = updateForecastHourlyWeather = updateForecastDailyWeather = true;// force update
	}

	if (millis() - timeSinceForecastUpdate > (1000L*FORECAST_INTERVAL_SECS))
	{
		#ifdef SERIAL_LOGGING
		Serial.println("Setting updateForecastDailyWeather to true");
		#endif
		updateForecastDailyWeather = true;
		timeSinceForecastUpdate = millis();

		// do the update every 3 hours the hour before.
		// querry change on the 3 and is 9 hours ahead.
		if (hour() + 1 % 3 == 0)
		{
			#ifdef SERIAL_LOGGING
			Serial.println("Setting updateForecastHourlyWeather to true");
			#endif
			updateForecastHourlyWeather = true;
		}
	}

	if (millis() - timeSinceCurrentUpdate > (1000L*CURRENT_INTERVAL_SECS))
	{
		#ifdef SERIAL_LOGGING
		Serial.println("Setting updateCurrentWeather to true");
		#endif
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

	if (updateExternalSensors || updateCurrentWeather || updateForecastHourlyWeather || updateForecastDailyWeather) 
	{
		updateSuccessed = updateData();
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

bool updateData() 
{
	bool success = true;
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayWeather.drawProgress(20, "Updating time...");
	updateSystemTime();
	if (updateExternalSensors)
	{
		displayWeather.drawProgress(35, "Updating external sensor data...");
		readExternalSensorsData(appSettings.ThingSpeakSettings.ChannelID, &externalSensorData);
		updateExternalSensors = false;
		success = success && externalSensorData.IsUpdated;
	}
	

	if (updateCurrentWeather)
	{
		displayWeather.drawProgress(50, "Updating current weather...");
		currentWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		currentWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		currentWeatherUpdated = currentWeatherClient.updateCurrentById(&currentWeather, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location);
		if (!currentWeatherUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateCurrentWeather = false;
		success = success && currentWeatherUpdated;
	}

	if (updateForecastHourlyWeather)
	{
		displayWeather.drawProgress(65, "Updating hourly forecasts...");
		forecastWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		forecastWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		forecastWeatherClient.setAllowedHours(HOURLY_FORECAST_HOURS, sizeof(HOURLY_FORECAST_HOURS));
		forecastWeatherClient.setMaxDays(1);
		forecastWeatherHourlyUpdated = forecastWeatherClient.updateForecastsById(forecastWeatherHourlyQuerry, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location, HOURLY_MAX_FORECASTS);

		for(int i = 0; i < HOURLY_MAX_FORECASTS; i++)
		{
			// observationTime is equal to zero on first pass.
			if (forecastWeatherHourly[i].observationTime == 0)
			{
				forecastWeatherHourly[i] = forecastWeatherHourlyQuerry[i];
			}
		}

		// the first slot is 9 hours pass...and we are look for 12th hour ahead.
		// so when the current hour == our hours do a check.
		time_t observationTimestamp = forecastWeatherHourly[0].observationTime;
		if (hour() > hour(observationTimestamp))
		{
			forecastWeatherHourly[0] = forecastWeatherHourly[1];
			forecastWeatherHourly[1] = forecastWeatherHourly[2];
			forecastWeatherHourly[2] = forecastWeatherHourlyQuerry[0];
		}

		if (!forecastWeatherHourlyUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateForecastHourlyWeather = false;
		success = success && forecastWeatherHourlyUpdated;
	}

	if (updateForecastDailyWeather)
	{
		displayWeather.drawProgress(80, "Updating daily forecasts...");
		forecastWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		forecastWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		forecastWeatherClient.setAllowedHours(DAILY_FORECAST_HOURS, sizeof(DAILY_FORECAST_HOURS));
		forecastWeatherClient.setMaxDays(4);
		forecastWeatherDailyUpdated = forecastWeatherClient.updateForecastsById(forecastWeatherDaily, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location, DAILY_FORECAST_HOURS);
		if (!forecastWeatherDailyUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateForecastDailyWeather = false;
		success = success && forecastWeatherDailyUpdated;
	}

	lastUpdated = now();
	displayWeather.drawProgress(100, "Updating done!");
	delay(1000);
	return success;
}

void resolveAppSettings()
{
	int8_t numNetworks = WiFi.scanNetworks();
	#ifdef SERIAL_LOGGING
	Serial.println("Number of networks found: " + String(numNetworks));
	#endif

	if (numNetworks == 0)
	{
		delay(2500);
		numNetworks = WiFi.scanNetworks();
	}

	for (uint8_t i=0; i<numNetworks; i++)
	{
		String ssid = WiFi.SSID(i);
		#ifdef SERIAL_LOGGING
		Serial.println(ssid + " (" + String(WiFi.RSSI(i)) + ")");
		#endif
		for (uint8_t j=0; j < AppSettingsCount; j++)
		{
			const char* appSsid = AppSettings[j].WifiSettings.SSID;
			#ifdef SERIAL_LOGGING
			Serial.println("Checking: " + String(appSsid));
			#endif
			if (strcasecmp(appSsid, ssid.c_str()) == 0)
			{
				#ifdef SERIAL_LOGGING
				Serial.println("Found: " + String(ssid));
				#endif
				AppSettings[j].WifiSettings.Avialable = true;
				AppSettings[j].WifiSettings.Strength = WiFi.RSSI(i);

				if (AppSettings[j].WifiSettings.Strength > appSettings.WifiSettings.Strength)
				{
					appSettings = AppSettings[j];
				}
			}
		}
	}

	#ifdef SERIAL_LOGGING
	Serial.println("Using WiFi " + String(appSettings.WifiSettings.SSID));
	#endif
	WiFi.disconnect();	
}

void printConnectInfo()
{
	// get your MAC address
	byte mac[6];
	WiFi.macAddress(mac);
	IPAddress ip = WiFi.localIP();
	
	// SSID
	char ssidInfo[34] = "";
	sprintf(ssidInfo, "WiFi SSID: %s", WiFi.SSID());

	// MAC address
	char macInfo[34] = "";
	sprintf(macInfo, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

	// IP address
	char ipInfo[34] = "";
	sprintf(ipInfo, "IP address: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	
	displayWeather.printLine(ssidInfo, SUCCESS_COLOR);
	displayWeather.printLine(ipInfo, TEXT_MAIN_COLOR);
	displayWeather.printLine(macInfo, TEXT_MAIN_COLOR);

	#ifdef SERIAL_LOGGING
	Serial.println(ssidInfo);
	Serial.println(ipInfo);	
	Serial.println(macInfo);
	#endif
}

bool connectToWiFi(uint8_t retries)
{
	String scanMsg = "Scanning for WiFi SSID.";
	#ifdef SERIAL_LOGGING
	Serial.println(scanMsg);
	#endif
	displayWeather.drawProgress(30, scanMsg);
	resolveAppSettings();

	char infoMsg[] = "Waiting for connection to WiFi";
	if (!appSettings.WifiSettings.Avialable)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "No WiFi connections found for %s!", appSettings.WifiSettings.SSID);
		#ifdef SERIAL_LOGGING
		Serial.println(connectErr);
		#endif
		displayWeather.fillScreen(ERROR_COLOR);
		displayWeather.drawString(connectErr, 400, 240, TEXT_CENTER_MIDDLE, BLACK, ERROR_COLOR);
		return false;
	}

	WiFi.begin(appSettings.WifiSettings.SSID, appSettings.WifiSettings.Password);
	WiFi.sleepMode(WIFI_NONE_SLEEP);
	#ifdef SERIAL_LOGGING
	Serial.println(infoMsg);
	#endif
	displayWeather.drawProgress(50, infoMsg);

	uint8_t attempts = 0;
	uint8_t attemptMax = 3;

	sprintf(infoMsg, "Connecting to %s", appSettings.WifiSettings.SSID);
	while (WiFi.status() != WL_CONNECTED && attempts < attemptMax)
	{
		delay(1000);
		#ifdef SERIAL_LOGGING
		Serial.print('.');
		#endif
		displayWeather.drawProgress(65 + (attempts*5), infoMsg);
		++attempts;
	}
	#ifdef SERIAL_LOGGING
	Serial.println();
	#endif

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
	#ifdef SERIAL_LOGGING
	Serial.println(intialMsg);
	#endif
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
		#ifdef SERIAL_LOGGING
		Serial.println(initialErr);
		#endif
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
		#ifdef SERIAL_LOGGING
		Serial.println(connectErr);
		#endif
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

void drawForecastHourlyFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawForecastHourly(forecastWeatherHourly, x, y);
}

void drawForecastDailyFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawForecastDaily(forecastWeatherDaily, x, y);
}

void drawHeaderOverlay(DisplayControlState* state)
{
	displayWeather.drawHeader(externalSensorData.IsUpdated, currentWeatherUpdated, forecastWeatherHourlyUpdated, forecastWeatherDailyUpdated, lastUpdated);
}

void drawFooterOverlay(DisplayControlState* state)
{
	displayWeather.drawFooter(&internalSensorData, &currentWeather);
}