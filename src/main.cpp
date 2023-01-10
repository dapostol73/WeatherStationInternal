// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796
// For the screen to work, you need to uncomment the following in MCUFRIEND_kbv Library
// In utility\mcufriend_shield.h uncomment #define USE_SPECIAL
// In utility\mcufriend_special.h uncomment #define USE_MEGA_8BIT_PORTC_SHIELD

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiEspAT.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>

// time
#include <TimeLib.h>
#include <NTPClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "DisplayControl.h"
#include "DisplayFonts.h"
#include "DisplayImages.h"
#include "WifiInfo.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"
#include "Icons/OpenWeatherIcons.h"

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
const uint8_t MAX_FORECASTS = 4;

// Adjust according to your language
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

OpenWeatherMapCurrentData currentWeather;
OpenWeatherMapCurrent currentWeatherClient;

OpenWeatherMapForecastData forecasts[MAX_FORECASTS];
OpenWeatherMapForecast forecastClient;

uint16_t palette[] = {BLACK, WHITE, GOLD, DEEPSKYBLUE};
DisplayControl displayControl;
DisplayContolProgress progress;

void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center = false, int16_t scale = 1);
void drawDateTime(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeather(DisplayControlState* state, int16_t x, int16_t y);
void drawForecast(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);
void drawFooterOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawForecast, drawCurrentWeather, drawDateTime };
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay, drawFooterOverlay };
int numberOfOverlays = 2;

/***************************
 * Begin Settings
 **************************/
// Setup
const int SENSOR_INTERVAL_SECS = 15; // Sensor query every 15 seconds
const int TIME_INTERVAL_SECS = 10 * 60; // Check time every 10 minutes
const int UPDATE_INTERVAL_SECS = 5 * 60; // Update every 60 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

// flag changed in the ticker function every 10 minutes
bool readyForUpdate = false;
char lastUpdate[40] = "?Unkown";
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

	progress.x = 0;
	progress.y = 280;
	progress.width = 480;
	progress.height = 40;
	progress.padding = 5;
	progress.corner = 10;
	progress.foregroundColor = CYAN;
	progress.gfxFont = &CalibriBold8pt7b;
	displayControl.init(1, &CalibriRegular8pt7b);

	displayControl.setProgress(progress);
	displayControl.setFrameAnimation(SLIDE_LEFT);
	displayControl.setFrames(frames, numberOfFrames);
	displayControl.setOverlays(overlays, numberOfOverlays);

	displayControl.fillScreen(BLACK);
	drawWeatherIcon(40 ,  70, "01d", true);
	drawWeatherIcon(140,  70, "02d", true);
	drawWeatherIcon(240,  70, "03d", true);
	drawWeatherIcon(340,  70, "04d", true);
	drawWeatherIcon(440,  70, "09d", true);
	drawWeatherIcon(90 , 190, "10d", true);
	drawWeatherIcon(190, 190, "11d", true);
	drawWeatherIcon(290, 190, "13d", true);
	drawWeatherIcon(390, 190, "50d", true);
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
	currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);

	displayControl.drawProgress(75, "Updating forecasts...");
	forecastClient.setMetric(IS_METRIC);
	forecastClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	uint8_t allowedHours[] = {12};
	forecastClient.setAllowedHours(allowedHours, sizeof(allowedHours));
	forecastClient.updateForecastsById(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
	
	sprintf(lastUpdate, "%02d/%02d/%04d -- %02d:%02d:%02d", month(), day(), year(), hour(), minute(), second());
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

void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t scale)
{
	const OpenWeatherIcon icon = getOpenWeatherCropIconFromProgmem(iconName);
	displayControl.drawBitmap(x, y, icon.width, icon.height, icon.data, center, scale);
}

void drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor)
{
	int16_t x1, y1 = 0;
    uint16_t w, h = 0;
    displayControl.getDisplay()->getTextBounds("0", 0, 0, &x1, &y1, &w, &h);
	String temp = String(currentWeather.temp, 1);
	if (align > 0)
	{
		int sw = w * (temp.length() + 2);
        if (align == TEXT_CENTER)
        {
            x -= sw * 0.5;
            y -= h * 0.5;
        }
        else if (align == TEXT_RIGHT)
        {
            x -= sw + 1;
            //y -= h;
        }
	}

	displayControl.drawString(temp, x, y, TEXT_LEFT, foregroundColor);
	int radius = w * 0.4;
	x = displayControl.getDisplay()->getCursorX() + radius;
	displayControl.getDisplay()->drawCircle(x, y + radius, radius, foregroundColor);
	x += radius;
	if (isMetric)
	{
		displayControl.drawString("C", x, y, TEXT_LEFT, foregroundColor);
	}
	else
	{
		displayControl.drawString("F", x, y, TEXT_LEFT, foregroundColor);
	}
}

void drawDateTime(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.setFont(&CalibriBold24pt7b);
	char buff[16];
	sprintf_P(buff, PSTR("%s, %02d/%02d/%04d"), WDAY_NAMES[weekday()-1].c_str(), day(), month(), year());
	displayControl.drawString(buff, 240, 140, TEXT_CENTER, CYAN);

	sprintf_P(buff, PSTR("%02d:%02d %s"), hourFormat12(), minute(), (isAM() ? "AM" : "PM"));
	displayControl.drawString(buff, 240, 180, TEXT_CENTER, CYAN);
}

void drawCurrentWeather(DisplayControlState* state, int16_t x, int16_t y)
{
	x = 240;
	y = 40;
	drawWeatherIcon(x, y + 90, currentWeather.icon, true, 2);
	displayControl.setFont(&CalibriBold24pt7b);
	displayControl.drawString(currentWeather.cityName, x, y + 5, TEXT_CENTER, YELLOW);
	displayControl.setFont(&CalibriBold16pt7b);
	drawTemperature(currentWeather.temp, IS_METRIC, x, y + 160, TEXT_CENTER, CYAN);
	displayControl.drawString(currentWeather.description, x, y + 200, TEXT_CENTER, ORANGE);
}

void drawForecastDetails(int x, int y, int dayIndex) 
{
	time_t observationTimestamp = forecasts[dayIndex].observationTime;
	int day = weekday(observationTimestamp)-1;
	drawWeatherIcon(x, y + 100, forecasts[dayIndex].icon, true, 2);
	displayControl.setFont(&CalibriBold16pt7b);
	displayControl.drawString(WDAY_NAMES[day], x, y + 10, TEXT_CENTER, YELLOW);
	drawTemperature(forecasts[dayIndex].temp, IS_METRIC, x, y + 170, TEXT_CENTER, CYAN);
	displayControl.setFont(&CalibriBold8pt7b);
	displayControl.drawString(forecasts[dayIndex].description, x, y + 200, TEXT_CENTER, ORANGE);	
}

void drawForecast(DisplayControlState* state, int16_t x, int16_t y) 
{
	//displayControl.setFont(&CalibriBold24pt7b);
	//displayControl.drawString(forecasts[0].cityName, 240, 40, TEXT_CENTER, YELLOW);
	drawForecastDetails(80, 60, 0);
	drawForecastDetails(240, 60, 1);
	drawForecastDetails(400, 60, 2);	
}

void drawHeaderOverlay(DisplayControlState* state)
{
	displayControl.getDisplay()->fillRect(0, 0, 480, 12, CHARCOAL);
	displayControl.getDisplay()->drawFastHLine(0, 13, 480, CYAN);
	displayControl.getDisplay()->drawFastHLine(0, 14, 480, CYAN);
	displayControl.setFont(&CalibriRegular8pt7b);
	displayControl.drawString(lastUpdate, 480, 0, TEXT_RIGHT);
}

void drawFooterOverlay(DisplayControlState* state)
{
	char time[10];
	sprintf_P(time, PSTR("%02d:%02d"), timeClient.getHours(), timeClient.getMinutes());

	displayControl.getDisplay()->fillRect(0, 280, 480, 320, CHARCOAL);
	displayControl.getDisplay()->drawFastHLine(0, 278, 480, CYAN);
	displayControl.getDisplay()->drawFastHLine(0, 279, 480, CYAN);
	displayControl.setFont(&CalibriBold16pt7b);
	displayControl.drawString(time, 120, 300, TEXT_CENTER, ORANGE);
	drawTemperature(currentWeather.temp, IS_METRIC, 360, 300, TEXT_CENTER, ORANGE);
}
