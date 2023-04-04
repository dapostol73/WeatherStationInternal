#ifndef _DISPLAY_WEATHER_
#define _DISPLAY_WEATHER_

#include <Arduino.h>
#include <WiFiEspAT.h>

#include <TimeLib.h>
#include <NTPClient.h>

#include "DisplayControl.h"
#include "DisplayFonts.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

// Adjust according to your language
const String WDAY_NAMES[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const String WIND_DIR[] = {"N", "NNE", "NE", "NEE", "E", "SEE", "SE", "SSE", "S", "SSW", "SW", "SWW", "W", "NWW", "NW", "NNW"};

class DisplayWeather : public DisplayControl
{
    private:
        uint16_t palette[4] = {BLACK, WHITE, GOLD, DEEPSKYBLUE};
        NTPClient *m_timeClient;
        time_t m_timeUpdated;
        char m_lastTimeUpdated[40] = "?Unkown";
        bool m_isMetric = true;

        void drawRainDrop(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawRainLight(int16_t x, int16_t y, int16_t size = 1, uint16_t color = DODGERBLUE);
        void drawRainHeavy(int16_t x, int16_t y, int16_t size = 1, uint16_t color = DODGERBLUE);
        void drawSnowflake(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawSnow(int16_t x, int16_t y, int16_t size = 1, uint16_t color = WHITE);
        void drawSun(int16_t x, int16_t y, int16_t radius = 12, int16_t size = 1, uint16_t color = ORANGERED);
        void drawCloud(int16_t x, int16_t y, int16_t size = 1, uint16_t color = WHITE);
        void drawThunder(int16_t x, int16_t y, int16_t size = 1, uint16_t color = ORANGE);
        void drawFog(int16_t x, int16_t y, int16_t size = 1, uint16_t color = DIMGRAY);
        void drawWind(int16_t x, int16_t y, int16_t size = 1, uint16_t color = DIMGRAY);
        void drawVisibility(int16_t x, int16_t y, int16_t size = 1);
        void drawCompassArrow(int16_t x, int16_t y, int16_t start_angle, int16_t size = 1);
        void draw00Unknown(int16_t x, int16_t y, int16_t size = 1);
        void draw01Clear(int16_t x, int16_t y, int16_t size = 1);
        void draw02FewClouds(int16_t x, int16_t y, int16_t size = 1);
        void draw03ScatteredClouds(int16_t x, int16_t y, int16_t size = 1);
        void draw04BrokenClouds(int16_t x, int16_t y, int16_t size = 1);
        void draw09ShowerRain(int16_t x, int16_t y, int16_t size = 1);
        void draw10Rain(int16_t x, int16_t y, int16_t size = 1);
        void draw11ThunderStorm(int16_t x, int16_t y, int16_t size = 1);
        void draw13Snow(int16_t x, int16_t y, int16_t size = 1);
        void draw50Mist(int16_t x, int16_t y, int16_t size = 1);
        void drawTempratureGauge(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
        void drawTempratureIcon(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
        void drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor);
        void drawHumidityGauge(float humidity, int16_t x, int16_t y, int16_t size = 1);
        void drawHumidityIcon(float humidity, int16_t x, int16_t y, int16_t size = 1);
        void drawHumidity(float humidity, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor);
        void drawForecastDetails(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y, int16_t dayIndex);
        void drawWiFiSignal(int16_t x, int16_t y, int16_t size);        
    public:
        DisplayWeather();
        void init();
        void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t scale = 1);
        void drawTempratureHumidity(int16_t x, int16_t y, float internalTemp, float internalHmd, float externalTemp, float externalHmd);
        void drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y);
        void drawForecast(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y);
        void drawHeader(bool currentWeathersUpdated, bool forecastWeathersUpdated, time_t timeUpdated);
        void drawFooter(OpenWeatherMapCurrentData *currentWeather);
};

#endif