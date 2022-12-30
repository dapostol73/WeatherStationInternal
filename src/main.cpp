// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include <NTPClient.h>

// time
#include <time.h>                       // time() ctime()

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "DisplayControl.h"
#include "WifiInfo.h"
#include "WeatherStationFonts.h"
#include "WeatherStationImages.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

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
WiFiEspClient client;
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

DisplayControl displayControl;

void drawDateTime(DisplayControlState* state, int16_t x, int16_t y);
void drawCurrentWeather(DisplayControlState* state, int16_t x, int16_t y);
void drawForecast(DisplayControlState* state, int16_t x, int16_t y);
void drawHeaderOverlay(DisplayControlState* state);

FrameCallback frames[] = { drawDateTime, drawCurrentWeather, drawForecast };
int numberOfFrames = 3;

OverlayCallback overlays[] = { drawHeaderOverlay };
int numberOfOverlays = 1;

/***************************
 * Begin Settings
 **************************/
// Setup
const int SENSOR_INTERVAL_SECS = 15; // Sensor query every 15 seconds
const int TIME_INTERVAL_SECS = 10 * 60; // Check time every 10 minutes
const int UPDATE_INTERVAL_SECS = 20 * 60; // Update every 20 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)
time_t now;
WiFiEspUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

// flag changed in the ticker function every 10 minutes
bool readyForWeatherUpdate = false;
String lastUpdate = "--";
long timeSinceLastWUpdate = 0;

void setReadyForWeatherUpdate();
void updateData();
void configureWifi();
void printConnectInfo();

void setup()
{
	Serial.begin(115200);
	while (!Serial)
		;

	displayControl.init(1);

	configureWifi();
	
	timeClient.begin();

	displayControl.setFrameAnimation(SLIDE_LEFT);
	displayControl.setFrames(frames, numberOfFrames);
	displayControl.setOverlays(overlays, numberOfOverlays);

	updateData();
	//showMainMenu();
}

void loop()
{
	//Read sensor values base on Upload interval seconds
	if(millis() - readTime > SENSOR_INTERVAL_SECS){
		//readTemperatureHumidity();
		readTime = millis();
	}

	if (millis() - timeSinceLastWUpdate > (1000L*UPDATE_INTERVAL_SECS)) {
		setReadyForWeatherUpdate();
		timeSinceLastWUpdate = millis();
	}

	if (readyForWeatherUpdate) 
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

void setReadyForWeatherUpdate() 
{
	Serial.println("Setting readyForUpdate to true");
	readyForWeatherUpdate = true;
}

void updateData() 
{
	displayControl.drawProgress(5, 148, 470, 24, 10, "Updating time...", 2, CYAN);
	displayControl.drawProgress(5, 148, 470, 24, 30, "Updating weather...", 2, CYAN);
	currentWeatherClient.setMetric(IS_METRIC);
	currentWeatherClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	currentWeatherClient.updateCurrentById(&currentWeather, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION);
	displayControl.drawProgress(5, 148, 470, 24, 50, "Updating forecasts...", 2, CYAN);
	forecastClient.setMetric(IS_METRIC);
	forecastClient.setLanguage(OPEN_WEATHER_MAP_LANGUAGE);
	uint8_t allowedHours[] = {12};
	forecastClient.setAllowedHours(allowedHours, sizeof(allowedHours));
	forecastClient.updateForecastsById(forecasts, OPEN_WEATHER_MAP_APP_ID, OPEN_WEATHER_MAP_LOCATION, MAX_FORECASTS);
	readyForWeatherUpdate = false;
	displayControl.drawProgress(5, 148, 470, 24, 100, "Updating done!", 2, GREEN);
	delay(1000);
	displayControl.fillScreen(BLACK);
}

void configureWifi()
{
	displayControl.fillScreen(BLACK);
	char intialMsg[] = "Intializing Wifi module.";
	Serial.println(intialMsg);
	displayControl.drawProgress(5, 148, 470, 14, 20, intialMsg, 2, CYAN);
	Serial3.begin(115200);
	WiFi.init(&Serial3);

	if (WiFi.status() == WL_NO_SHIELD)
	{
		char initialErr[] = "Communication with WiFi module failed!";
		Serial.println(initialErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(initialErr, 240, 160, TEXT_CENTER, 2, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	char infoMsg[] = "Waiting for connection to WiFi";
	WiFi.begin(NAME_OF_SSID, PASSWORD_OF_SSID);
	Serial.println(infoMsg);
	displayControl.drawProgress(5, 148, 470, 14, 40, infoMsg, 2, CYAN);

	int counter = 0;
	int timeout = 0;
	int timeoutMax = 30;

	while (WiFi.status() != WL_CONNECTED && timeout < timeoutMax)
	{
		delay(1000);
		Serial.print('.');
		displayControl.drawProgress(5, 148, 470, 14, 60, "Connecting to WiFi", 2, CYAN);
		displayControl.drawBitMap(46, 30, 8, 8, counter % 3 == 0 ? activeSymbole : inactiveSymbole, 1);
		displayControl.drawBitMap(60, 30, 8, 8, counter % 3 == 1 ? activeSymbole : inactiveSymbole, 1);
		displayControl.drawBitMap(74, 30, 8, 8, counter % 3 == 2 ? activeSymbole : inactiveSymbole, 1);
		counter++;
		++timeout;
	}

	Serial.println();
	displayControl.printLine();

	if (WiFi.status() == WL_DISCONNECTED)
	{
		char connectErr[48] = "";
		sprintf(connectErr, "Wifi failed to connect to %s acces point!", NAME_OF_SSID);
		Serial.println(connectErr);
		displayControl.fillScreen(RED);
		displayControl.drawString(connectErr, 240, 160, TEXT_CENTER, 2, BLACK, RED);
		// don't continue
		while (true)
			;
	}

	char wifiInfo[42] = "";
	sprintf(wifiInfo, "Connected to Wifi: %s", NAME_OF_SSID);
	displayControl.fillScreen(BLACK);
	displayControl.drawProgress(5, 148, 470, 14, 80, wifiInfo, 2, CYAN);
		
	printConnectInfo();
	displayControl.drawProgress(5, 148, 470, 14, 100, "Wifi initialization done!", 2, GREEN);
	delay(1000);

	if (WiFi.ping("www.google.com"))
	{
		Serial.println("Successfully pinged www.google.com");
	}
	else
	{
		Serial.println("Failed to pinged www.google.com");
	}
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

void drawDateTime(DisplayControlState* state, int16_t x, int16_t y)
{
	if (timeClient.update())
	{
		displayControl.drawString(timeClient.getFormattedTime(), 240, 160, TEXT_CENTER, 4, PURPLE);
	}
/*   now = time(nullptr);
  struct tm* timeInfo;
  timeInfo = localtime(&now);
  char buff[16];


  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  String date = WDAY_NAMES[timeInfo->tm_wday];

  sprintf_P(buff, PSTR("%s, %02d/%02d/%04d"), WDAY_NAMES[timeInfo->tm_wday].c_str(), timeInfo->tm_mday, timeInfo->tm_mon+1, timeInfo->tm_year + 1900);
  display->drawString(64 + x, 5 + y, String(buff));
  display->setFont(ArialMT_Plain_24);

  sprintf_P(buff, PSTR("%02d:%02d:%02d"), timeInfo->tm_hour, timeInfo->tm_min, timeInfo->tm_sec);
  display->drawString(64 + x, 15 + y, String(buff));
  display->setTextAlignment(TEXT_ALIGN_LEFT); */
}

void drawCurrentWeather(DisplayControlState* state, int16_t x, int16_t y)
{
	displayControl.drawString("Current Weather", 240, 150, TEXT_CENTER, 2, ORANGE);
/*   display->setFont(ArialMT_Plain_10);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 38 + y, currentWeather.description);

  display->setFont(ArialMT_Plain_24);
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");
  display->drawString(60 + x, 5 + y, temp);

  display->setFont(Meteocons_Plain_36);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(32 + x, 0 + y, currentWeather.iconMeteoCon); */
}

void drawForecastDetails(int x, int y, int dayIndex) 
{
	int tempx = 15+(150*(dayIndex+1));
	char index[14];
	sprintf(index, "I:%02d", dayIndex);
	displayControl.drawString(index, tempx, 150, TEXT_CENTER, 2, YELLOW);
/*   time_t observationTimestamp = forecasts[dayIndex].observationTime;
  struct tm* timeInfo;
  timeInfo = localtime(&observationTimestamp);
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y, WDAY_NAMES[timeInfo->tm_wday]);

  display->setFont(Meteocons_Plain_21);
  display->drawString(x + 20, y + 12, forecasts[dayIndex].iconMeteoCon);
  String temp = String(forecasts[dayIndex].temp, 0) + (IS_METRIC ? "°C" : "°F");
  display->setFont(ArialMT_Plain_10);
  display->drawString(x + 20, y + 34, temp);
  display->setTextAlignment(TEXT_ALIGN_LEFT); */
}

void drawForecast(DisplayControlState* state, int16_t x, int16_t y) 
{
	drawForecastDetails(x, y, 0);
	drawForecastDetails(x + 44, y, 1);
	drawForecastDetails(x + 88, y, 2);
}

void drawHeaderOverlay(DisplayControlState* state)
{
	timeClient.update();
	char time[14];
	sprintf_P(time, PSTR("%02d:%02d"), timeClient.getHours(), timeClient.getMinutes());
	String temp = String(currentWeather.temp, 1) + (IS_METRIC ? "°C" : "°F");

	displayControl.getDisplay()->Set_Draw_color(BLACK);
	displayControl.getDisplay()->Draw_Rectangle(0, 280, 480, 320);
	displayControl.getDisplay()->Set_Draw_color(CYAN);
	displayControl.getDisplay()->Draw_Fast_HLine(0, 280, 480);
	displayControl.drawString(time, 120, 300, TEXT_CENTER, 3, CYAN);
	displayControl.drawString(temp, 360, 300, TEXT_CENTER, 3, CYAN);
	
}