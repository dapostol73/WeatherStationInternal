#ifndef _DISPLAY_WEATHER_
#define _DISPLAY_WEATHER_

#include <Arduino.h>
#include <WiFiEspAT.h>

#include <TimeLib.h>
#include <NTPClient.h>

#include "SensorData.h"
#include "DisplayControl.h"
#include "DisplayFonts.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

#define BACKGROUND_COLOR 0x0000
#define FOREGROUND_COLOR 0xFFFF
#define OVERLAY_COLOR 0x2945
#define TEXT_TITLE_COLOR 0xFFE0
#define TEXT_MAIN_COLOR 0xFD20
#define TEXT_ALT_COLOR 0x07FF

#define RAIN_COLOR 0x05FF
#define SUN_COLOR 0xFA20
#define CLOUD_LIGHT_COLOR 0xFFFF
#define CLOUD_DARK_COLOR 0xAD55
#define SNOW_COLOR 0xFFFF
#define THUNDER_COLOR 0xFD20
#define FOG_COLOR 0xAD55
#define WIND_COLOR 0xAD55
#define EYE_COLOR 0x05FF

#define TEMP_COLD_COLOR 0x05FF
#define TEMP_COOL_COLOR 0x1C9F
#define TEMP_WARM_COLOR 0xFA20
#define TEMP_HOT_COLOR 0xF800

#define HMD_AVG_COLOR 0x07FF
#define HMD_DRY_COLOR 0xFEA0
#define HMD_WET_COLOR 0x1C9F

#define SUCCESS_COLOR 0x07E0
#define ERROR_COLOR 0xF800

// Adjust according to your language
const String WDAY_NAMES[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const String MONTH_NAMES[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const String WIND_DIR[] = {"N", "NNE", "NE", "NEE", "E", "SEE", "SE", "SSE", "S", "SSW", "SW", "SWW", "W", "NWW", "NW", "NNW"};

class DisplayWeather : public DisplayControl
{
    private:
        uint16_t palette[4] = {BACKGROUND_COLOR, CLOUD_LIGHT_COLOR, SUN_COLOR, RAIN_COLOR};
        NTPClient *m_timeClient;
        time_t m_timeUpdated;
        char m_lastTimeUpdated[40] = "?Unkown";
        //bool m_isMetric = true;

        void drawRainDrop(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawRainLight(int16_t x, int16_t y, int16_t size = 1, uint16_t color = RAIN_COLOR);
        void drawRainHeavy(int16_t x, int16_t y, int16_t size = 1, uint16_t color = RAIN_COLOR);
        void drawSnowflake(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawSnow(int16_t x, int16_t y, int16_t size = 1, uint16_t color = SNOW_COLOR);
        void drawSun(int16_t x, int16_t y, int16_t radius = 12, int16_t size = 1, uint16_t color = SUN_COLOR);
        void drawCloud(int16_t x, int16_t y, int16_t size = 1, uint16_t color = CLOUD_LIGHT_COLOR);
        void drawThunder(int16_t x, int16_t y, int16_t size = 1, uint16_t color = THUNDER_COLOR);
        void drawFog(int16_t x, int16_t y, int16_t size = 1, uint16_t color = FOG_COLOR);
        void drawWind(int16_t x, int16_t y, int16_t size = 1, uint16_t color = WIND_COLOR);
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
        void drawTemperatureGauge(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
        void drawTemperatureIcon(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
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
        void drawSensorData(int16_t x, int16_t y, SensorData *internalSensorData, SensorData *externalSensorData);
        void drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y);
        void drawForecast(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y);
        void drawHeader(bool currentWeathersUpdated, bool forecastWeathersUpdated, time_t timeUpdated);
        void drawFooter(SensorData *externalSensorData, OpenWeatherMapCurrentData *currentWeather);
};

#endif