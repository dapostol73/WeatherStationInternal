//*****************************************************************************
// Copyright (c) 2014 A12 Studios Inc. and Demetrius Apostolopoulos.
// All rights reserved.
//
// Firmware for ESP8266
// Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
// This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640
//
// If Serial Logging support is required add this build flag to platformio.ini
//   build_flags = -D SERIAL_LOGGING
//*****************************************************************************

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>

#include "UserSettings.h"
#include "ApplicationHelper.h"
#include "ApplicationSensor.h"
#include "NetworkManager.h"
#include "DisplayWeather.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
//#include "OpenWeatherMapOneCall.h"
#ifdef CANADIAN_TIDES
	#include "CanadianHydrograpicTidesHiLo.h"
#endif

#define SERIAL_BAUD_RATE 115200
#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

//#define DEBUG
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
NetworkManager netManager;
bool wiFiCurrentState;
bool wiFiLastState;
long wiFiLastConnection = LONG_MIN;

//******************************//
// Heart Beat Settings          //
//******************************//
long timeSinceHeartBeat = LONG_MIN;
bool stateHeartBeat = false;

//******************************//
// Internal Sensor Settings     //
//******************************//
SensorData internalSensorData;
long timeSinceInternalUpdate = LONG_MIN;

//******************************//
// External ThinkSpeak Settings //
//******************************//
#ifdef THINGSPEAK_SENSOR
	SensorData externalSensorData;
	long timeSinceExternalUpdate = LONG_MIN;
	bool updateExternalSensors = true;
#endif

//********************************//
// Canadian Hyrdographic Settings //
//********************************//
#ifdef CANADIAN_TIDES
	const uint8_t MAX_TIDES = 6;
	CanadianHydrograpicTidesHiLoData tidesHiLoPredictions[MAX_TIDES];
	CanadianHydrograpicTidesHiLo tidesHiLoClient;
	bool updateTidesHiLo = true;
	bool tidesHiLoUpdated = false;
	long timeSinceTidesHiLoUpdate = LONG_MIN;
#endif

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
long timeSinceCurrentUpdate = LONG_MIN;

OpenWeatherMapForecastData forecastWeatherDaily[DAILY_MAX_FORECASTS];
OpenWeatherMapForecastData forecastWeatherHourlyQuerry[HOURLY_MAX_FORECASTS];
OpenWeatherMapForecastData forecastWeatherHourly[HOURLY_MAX_FORECASTS];
OpenWeatherMapForecast forecastWeatherClient;
bool updateForecastDailyWeather = true;
bool forecastWeatherDailyUpdated = false;
bool updateForecastHourlyWeather = true;
bool forecastWeatherHourlyUpdated = false;
long timeSinceForecastHourlyUpdate = LONG_MIN;
long timeSinceForecastDailyUpdate = LONG_MIN;

/***************************
 * Timming Settings
 **************************/
// WiFi time between reconnect attempts, if disconnected.
const uint16_t WIFI_RECONNECT_INTERVAL_SECS = 5 * 60; // connect retry every 5 minute
// Time check setup, slighty off to easy the parsing.
const uint16_t INTSENSOR_INTERVAL_SECS = 60; // Sensor query every minute
#ifdef THINGSPEAK_SENSOR
	const uint16_t EXTSENSOR_INTERVAL_SECS = 11 * 60; // Sensor query every 11 minutes
#endif
#ifdef CANADIAN_TIDES
	const uint16_t CNDTIDES_INTERVAL_SECS = 30 * 60; // Tides HiLo query every 29 minutes
#endif
//const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes
const uint16_t CURRENT_INTERVAL_SECS = 23 * 60; // Update every 23 minutes
const uint16_t FORECAST_HOURLY_INTERVAL_SECS = 65 * 60; // Update every 65 minutes
const uint16_t FORECAST_DAILY_INTERVAL_SECS = 125 * 60; // Update every 125 minutes

bool updateSuccessed = true;
time_t lastUpdated;

DisplayWeather displayWeather;
DisplayContolProgress displayProgress;

#ifdef THINGSPEAK_SENSOR
void drawSensorDataFrame(DisplayControlState* state, int16_t x, int16_t y);
#endif
#ifdef CANADIAN_TIDES
void drawTidesHiLoFrame(DisplayControlState* state, int16_t x, int16_t y);
#endif
void drawCurrentWeatherFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastHourlyFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawForecastDailyFrame(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

#ifdef THINGSPEAK_SENSOR
FrameCallback frames[] = { drawCurrentWeatherFrame, drawForecastHourlyFrame, drawForecastDailyFrame, drawSensorDataFrame };
int numberOfFrames = 4;
#elif CANADIAN_TIDES
FrameCallback frames[] = { drawTidesHiLoFrame, drawCurrentWeatherFrame, drawForecastHourlyFrame, drawForecastDailyFrame };
int numberOfFrames = 4;
#else
FrameCallback frames[] = { drawCurrentWeatherFrame, drawForecastHourlyFrame, drawForecastDailyFrame };
int numberOfFrames = 3;
#endif

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

void initNetwork();
bool updateData();
void heartBeat();

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial3.begin(SERIAL_BAUD_RATE);	

#ifdef DISPLAY_ILI9488
	displayProgress.x = 0;
	displayProgress.y = 300;
	displayProgress.width = 480;
	displayProgress.height = 20;
	displayProgress.padding = 1;
	displayProgress.corner = 6;
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayProgress.gfxFont = &CalibriBold8pt7b;
#else
	displayProgress.x = 0;
	displayProgress.y = 420;
	displayProgress.width = 800;
	displayProgress.height = 60;
	displayProgress.padding = 5;
	displayProgress.corner = 24;
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayProgress.gfxFont = &CalibriRegular16pt7b;
#endif
	displayWeather.init();
	displayWeather.setProgress(&displayProgress);
	displayWeather.setFrames(frames, numberOfFrames);
	displayWeather.setOverlays(overlays, numberOfOverlays);

	displayWeather.fillScreen(BLACK);
#ifdef DISPLAY_ILI9488
	displayWeather.drawString(F("Weather Station"), 240, 160, TEXT_CENTER_MIDDLE, TEXT_TITLE_COLOR);
	displayWeather.drawWeatherIcon(60,  80, "00d", true, 1);
	displayWeather.drawWeatherIcon(150, 80, "01d", true, 1);
	displayWeather.drawWeatherIcon(240, 80, "02d", true, 1);
	displayWeather.drawWeatherIcon(330, 80, "03d", true, 1);
	displayWeather.drawWeatherIcon(420, 80, "04d", true, 1);
	displayWeather.drawWeatherIcon(60,  240, "09d", true, 1);
	displayWeather.drawWeatherIcon(150, 240, "10d", true, 1);
	displayWeather.drawWeatherIcon(240, 240, "11d", true, 1);
	displayWeather.drawWeatherIcon(330, 240, "13d", true, 1);
	displayWeather.drawWeatherIcon(420, 240, "50d", true, 1);
#else
	displayWeather.drawString(F("Weather Station"), 400, 200, TEXT_CENTER_MIDDLE, TEXT_TITLE_COLOR);
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
#endif

	initNetwork();
	initHelpers();// needs WiFi
	initSensors();
}

void loop()
{
	switch (touchTest())
	{
		case UPDATE:
		#ifdef DISPLAY_ILI9488
			displayWeather.DisplayGFX->fillRoundRect(190, 5, 100, 10, 5, SUCCESS_COLOR);
		#else
			displayWeather.DisplayGFX->fillRoundRect(350, 5, 100, 10, 5, SUCCESS_COLOR);
		#endif
			#ifdef THINGSPEAK_SENSOR
			updateExternalSensors = true;
			#endif
			#ifdef CANADIAN_TIDES
			tidesHiLoUpdated = true;
			#endif
			updateCurrentWeather = true;
			updateForecastHourlyWeather = true;
			updateForecastDailyWeather = true;// force update
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

	if (!netManager.isConnected() && millis() - wiFiLastConnection > (1000L*WIFI_RECONNECT_INTERVAL_SECS))
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("WiFi disconnect, waiting for reconnect."));
		#endif
		// All updates will be invalid at this point.
		#ifdef THINGSPEAK_SENSOR
		externalSensorData.IsUpdated = false;
		#endif
		#ifdef CANADIAN_TIDES
		tidesHiLoUpdated = false;
		#endif
		currentWeatherUpdated = false;
		forecastWeatherHourlyUpdated = false;
		forecastWeatherDailyUpdated = false;

		if (!netManager.connectWiFi(appSettings.WifiSettings, 3, 10))
		{
			#ifdef SERIAL_LOGGING
			Serial.println(F("WiFi reconnect failed."));
			#endif			
		}
		wiFiLastConnection = millis();
	}
	else
	{
		if (millis() - timeSinceForecastDailyUpdate > (1000L*FORECAST_DAILY_INTERVAL_SECS))
		{
			#ifdef SERIAL_LOGGING
			Serial.println(F("Setting updateForecastDailyWeather to true"));
			#endif
			updateForecastDailyWeather = true;
			timeSinceForecastDailyUpdate = millis();
		}

		if (millis() - timeSinceForecastHourlyUpdate > (1000L*FORECAST_HOURLY_INTERVAL_SECS))
		{
			#ifdef SERIAL_LOGGING
			Serial.println(F("Setting updateForecastHourlyWeather to true"));
			#endif
			updateForecastHourlyWeather = true;
			timeSinceForecastHourlyUpdate = millis();
		}

		if (millis() - timeSinceCurrentUpdate > (1000L*CURRENT_INTERVAL_SECS))
		{
			#ifdef SERIAL_LOGGING
			Serial.println(F("Setting updateCurrentWeather to true"));
			#endif
			updateCurrentWeather = true;
			timeSinceCurrentUpdate = millis();
		}
		
		#ifdef CANADIAN_TIDES
		if(millis() - timeSinceTidesHiLoUpdate > (1000L*CNDTIDES_INTERVAL_SECS))
		{
			updateTidesHiLo = true;
			timeSinceTidesHiLoUpdate = millis();
		}
		#endif

		#ifdef THINGSPEAK_SENSOR
		if(millis() - timeSinceExternalUpdate > (1000L*EXTSENSOR_INTERVAL_SECS))
		{
			updateExternalSensors = true;
			timeSinceExternalUpdate = millis();
		}
		#endif
	}

	//Read sensor values base on Upload interval seconds
	if(millis() - timeSinceInternalUpdate > (1000L*INTSENSOR_INTERVAL_SECS))
	{
		readInternalSensors(&internalSensorData);
		timeSinceInternalUpdate = millis();
	}

	#ifdef THINGSPEAK_SENSOR
	if (updateExternalSensors || updateCurrentWeather || updateForecastHourlyWeather || updateForecastDailyWeather)
	#else
	if (updateCurrentWeather || updateForecastHourlyWeather || updateForecastDailyWeather)
	#endif
	{
		updateSuccessed = updateData();
	}

	// Force an update if we were offline and just came online
	wiFiLastState = wiFiCurrentState;
	wiFiCurrentState = netManager.isConnected();
	if (wiFiLastState == false && wiFiCurrentState == true)
	{
		// force update
		#ifdef THINGSPEAK_SENSOR
		updateExternalSensors = true;
		#endif
		#ifdef CANADIAN_TIDES
		tidesHiLoUpdated = true;
		#endif
		updateCurrentWeather = true;
		updateForecastHourlyWeather = true;
		updateForecastDailyWeather = true;
	}

	int8_t remainingTimeBudget = displayWeather.update();
	if (remainingTimeBudget > 0) 
	{
		// You can do some work here
		// Don't do stuff if you are below your
		// time budget.
		heartBeat();
		delay(remainingTimeBudget);
	}
}

void initNetwork()
{
	char info[48] = "";
	displayWeather.drawProgress(25, F("Intializing WiFi module"));
	netManager.init();
	uint8_t appSetID = 0 ;
	displayWeather.drawProgress(50, F("Scanning for WiFi SSID"));
	appSetID = netManager.scanSettingsID(AppSettings, AppSettingsCount);
	appSettings = AppSettings[appSetID];
	sprintf(info, "Connecting to: %s", appSettings.WifiSettings.SSID);
	displayWeather.drawProgress(75, info);
	netManager.connectWiFi(appSettings.WifiSettings);
	displayWeather.printWiFiInfo();
	sprintf(info, "Connected IP: %s", netManager.getLocalIP().c_str());
	displayWeather.drawProgress(100, info);
	delay(2500);
	wiFiLastConnection = millis();
}

bool updateData() 
{
	bool success = true;
	displayProgress.foregroundColor = TEXT_ALT_COLOR;
	displayWeather.drawProgress(20, F("Updating time..."));
	updateSystemTime();
	printCurrentTime();

	#ifdef THINGSPEAK_SENSOR
	if (updateExternalSensors)
	{
		displayWeather.drawProgress(35, F("Updating external sensor data..."));
		readExternalSensorsData(appSettings.ThingSpeakSettings.ChannelID, &externalSensorData);
		updateExternalSensors = false;
		success = success && externalSensorData.IsUpdated;
	}
	#endif

	#ifdef CANADIAN_TIDES
	if (updateTidesHiLo)
	{
		displayWeather.drawProgress(45, F("Updating Tides HiLo data..."));
		tidesHiLoUpdated = tidesHiLoClient.updateTidesById(tidesHiLoPredictions, 
								appSettings.CanadianHydrographicSettings.StationID, 
								formatTimeISO8601UTC(now()), 
								formatTimeISO8601UTC(now() + 18L * 3600), 
								MAX_TIDES);
		if (!tidesHiLoUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateTidesHiLo = false;
		success = success && tidesHiLoUpdated;
	}
	#endif

	if (updateCurrentWeather)
	{
		displayWeather.drawProgress(50, F("Updating current weather..."));
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
		displayWeather.drawProgress(65, F("Updating hourly forecasts..."));
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
		if (difftime(now(), observationTimestamp) > -3600)
		{
			forecastWeatherHourly[0] = forecastWeatherHourly[1];
			forecastWeatherHourly[1] = forecastWeatherHourly[2];

			// Find the next entry after our current forcast.
			bool found = false;
			for(int i = 0; i < HOURLY_MAX_FORECASTS || !found; i++)
			{
				if (difftime(forecastWeatherHourlyQuerry[i].observationTime, forecastWeatherHourly[1].observationTime) > 0)
				{
					forecastWeatherHourly[2] = forecastWeatherHourlyQuerry[i];
					found = true;
					break;
				}
			}

			// if we didn't find one, then just use the first...this should never happen.
			if (!found)
			{
				forecastWeatherHourly[2] = forecastWeatherHourlyQuerry[0];
				#ifdef SERIAL_LOGGING
				Serial.println(F("Failed to find the next valid forcast, this shouldn't happen"));
				#endif
			}
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
		displayWeather.drawProgress(80, F("Updating daily forecasts..."));
		forecastWeatherClient.setMetric(appSettings.OpenWeatherSettings.IsMetric);
		forecastWeatherClient.setLanguage(appSettings.OpenWeatherSettings.Language);
		forecastWeatherClient.setAllowedHours(DAILY_FORECAST_HOURS, sizeof(DAILY_FORECAST_HOURS));
		forecastWeatherClient.setMaxDays(4);
		forecastWeatherDailyUpdated = forecastWeatherClient.updateForecastsById(forecastWeatherDaily, appSettings.OpenWeatherSettings.AppID, appSettings.OpenWeatherSettings.Location, DAILY_MAX_FORECASTS);
		if (!forecastWeatherDailyUpdated)
		{
			displayProgress.foregroundColor = ERROR_COLOR;
		}
		updateForecastDailyWeather = false;
		success = success && forecastWeatherDailyUpdated;
	}

	lastUpdated = now();
	displayWeather.drawProgress(100, F("Updating done!"));
	delay(1000);
	return success;
}

void heartBeat()
{
#ifdef DISPLAY_ILI9488
	uint8_t s = 4;
	uint16_t x = 6;
	uint16_t y = 28;
#else
	uint8_t s = 6;
	uint16_t x = 8;
	uint16_t y = 30;
#endif
	if (millis() - timeSinceHeartBeat > 1000)
	{
		timeSinceHeartBeat = millis();
		if (stateHeartBeat)
		{
			displayWeather.DisplayGFX->fillCircle(x, y, s, BLACK);
			displayWeather.DisplayGFX->drawCircle(x, y, s, GREEN);
		}
		else
		{
			displayWeather.DisplayGFX->fillCircle(x, y, s, GREEN);
		}
		stateHeartBeat = !stateHeartBeat;
	}
}

#ifdef THINGSPEAK_SENSOR
void drawSensorDataFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawSensorData(x, y, &internalSensorData, &externalSensorData);
}
#endif

#ifdef CANADIAN_TIDES
void drawTidesHiLoFrame(DisplayControlState* state, int16_t x, int16_t y)
{
	displayWeather.drawTidesHiLo(tidesHiLoPredictions, x, y);
}
#endif

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
	#ifdef THINGSPEAK_SENSOR
	displayWeather.drawHeader(externalSensorData.IsUpdated, currentWeatherUpdated, forecastWeatherHourlyUpdated, forecastWeatherDailyUpdated, lastUpdated);
	#else
	displayWeather.drawHeader(false, currentWeatherUpdated, forecastWeatherHourlyUpdated, forecastWeatherDailyUpdated, lastUpdated);
	#endif
}

void drawFooterOverlay(DisplayControlState* state)
{
	displayWeather.drawFooter(&internalSensorData, &currentWeather);
}