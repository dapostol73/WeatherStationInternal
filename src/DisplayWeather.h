#ifndef _DISPLAY_WEATHER_H_
#define _DISPLAY_WEATHER_H_

#include <Arduino.h>
#include <WiFiEspAT.h>

#include <TimeLib.h>
#include <NTPClient.h>
#include <MemoryUsage.h>

#include "ApplicationSensor.h"
#include "DisplayControl.h"
#include "DisplayFonts.h"
#include "OpenWeatherMapCurrent.h"
#include "OpenWeatherMapForecast.h"
#include "OpenWeatherMapOneCall.h"

#define BACKGROUND_COLOR 0x0000
#define FOREGROUND_COLOR 0xEF5D
#define OVERLAY_COLOR 0x1082
#define TEXT_TITLE_COLOR 0xFF20
#define TEXT_MAIN_COLOR 0xFD20
#define TEXT_ALT_COLOR 0x07FF

#define RAIN_COLOR 0x05FF
#define SUN_COLOR 0xFA20
#define MOON_COLOR 0xC638
#define CLOUD_LIGHT_COLOR 0xEF5D
#define CLOUD_DARK_COLOR 0x738E
#define SNOW_COLOR 0xEF5D
#define THUNDER_COLOR 0xFD20
#define FOG_COLOR 0xC638
#define WIND_COLOR 0xC638
#define EYE_COLOR 0x05FF

#define TEMP_COLD_COLOR 0x841F
#define TEMP_COOL_COLOR 0x863F
#define TEMP_MILD_COLOR 0x47F0
#define TEMP_WARM_COLOR 0xFC00
#define TEMP_HOT_COLOR 0xF900

#define HMD_AVG_COLOR 0x07FF
#define HMD_DRY_COLOR 0xFEA0
#define HMD_WET_COLOR 0x1C9F

#define SUCCESS_COLOR 0x07E0
#define ERROR_COLOR 0xF800

// Adjust according to your language
const char WDAY1[] PROGMEM = "Sunday";
const char WDAY2[] PROGMEM = "Monday";
const char WDAY3[] PROGMEM = "Tuesday";
const char WDAY4[] PROGMEM = "Wednesday";
const char WDAY5[] PROGMEM = "Thursday";
const char WDAY6[] PROGMEM = "Friday";
const char WDAY7[] PROGMEM = "Saturday";
const char* const WDAY_NAMES[7] PROGMEM = {WDAY1, WDAY2, WDAY3, WDAY4, WDAY5, WDAY6, WDAY7};

const char MON01[] PROGMEM = "Jan";
const char MON02[] PROGMEM = "Feb";
const char MON03[] PROGMEM = "Mar";
const char MON04[] PROGMEM = "Apr";
const char MON05[] PROGMEM = "May";
const char MON06[] PROGMEM = "Jun";
const char MON07[] PROGMEM = "Jul";
const char MON08[] PROGMEM = "Aug";
const char MON09[] PROGMEM = "Sep";
const char MON10[] PROGMEM = "Oct";
const char MON11[] PROGMEM = "Nov";
const char MON12[] PROGMEM = "Dec";
const char* const MONTH_NAMES[12] PROGMEM = {MON01, MON02, MON03, MON04, MON05, MON06, MON07, MON08, MON09, MON10, MON11, MON12};

const char WD01[] PROGMEM = "N";
const char WD02[] PROGMEM = "NNE";
const char WD03[] PROGMEM = "NE";
const char WD04[] PROGMEM = "NEE";
const char WD05[] PROGMEM = "E";
const char WD06[] PROGMEM = "SEE";
const char WD07[] PROGMEM = "SE";
const char WD08[] PROGMEM = "SSE";
const char WD09[] PROGMEM = "S";
const char WD10[] PROGMEM = "SSW";
const char WD11[] PROGMEM = "SW";
const char WD12[] PROGMEM = "SWW";
const char WD13[] PROGMEM = "W";
const char WD14[] PROGMEM = "NWW";
const char WD15[] PROGMEM = "NW";
const char WD16[] PROGMEM = "NNW";
const char* const WIND_DIR[16] PROGMEM = {WD01, WD02, WD03, WD04, WD05, WD06, WD07, WD08, WD09, WD10, WD11, WD12, WD14, WD15, WD16};

class DisplayWeather : public DisplayControl
{
    private:
        uint16_t palette[4] = {BACKGROUND_COLOR, CLOUD_LIGHT_COLOR, SUN_COLOR, RAIN_COLOR};
        NTPClient *m_timeClient;
        time_t m_timeUpdated;
        char m_lastTimeUpdated[40] = "?Unkown";
        //bool m_isMetric = true;

        void readDaysString(char* buffer, int16_t index)
        {
            if (index < 7)
            {
                strcpy_P(buffer, (char*)pgm_read_word(&(WDAY_NAMES[index])));
            }            
        }

        void readMonthString(char* buffer, int16_t index)
        {
            if (index < 12)
            {
                strncpy_P(buffer, (char*)pgm_read_ptr(&(MONTH_NAMES[index])), 4);
            }            
        }

        void readWindDirectionString(char* buffer, int16_t index)
        {
            if (index < 16)
            {
                strncpy_P(buffer, (char*)pgm_read_ptr(&(WIND_DIR[index])), 4);
            }            
        }

        void drawRainDrop(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawRainLight(int16_t x, int16_t y, int16_t size = 1, uint16_t color = RAIN_COLOR);
        void drawRainHeavy(int16_t x, int16_t y, int16_t size = 1, uint16_t color = RAIN_COLOR);
        void drawSnowflake(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawSnow(int16_t x, int16_t y, int16_t size = 1, uint16_t color = SNOW_COLOR);
        void drawSun(int16_t x, int16_t y, int16_t radius = 12, int16_t size = 1, uint16_t color = SUN_COLOR);
        void drawMoon(int16_t x, int16_t y, int16_t radius = 12, int16_t size = 1, uint16_t color = MOON_COLOR);
        void drawAsterial(int16_t x, int16_t y, int16_t radius = 12, int16_t size = 1, bool day = true);
        void drawCloud(int16_t x, int16_t y, int16_t size = 1, uint16_t color = CLOUD_LIGHT_COLOR);
        void drawThunder(int16_t x, int16_t y, int16_t size = 1, uint16_t color = THUNDER_COLOR);
        void drawFog(int16_t x, int16_t y, int16_t size = 1, uint16_t color = FOG_COLOR);
        void drawWind(int16_t x, int16_t y, int16_t size = 1, uint16_t color = WIND_COLOR);
        void drawVisibility(int16_t x, int16_t y, int16_t size = 1);
        void drawCompassArrow(int16_t x, int16_t y, int16_t start_angle, int16_t size = 1);
        void draw00Unknown(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw01Clear(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw02FewClouds(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw03ScatteredClouds(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw04BrokenClouds(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw09ShowerRain(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw10Rain(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw11ThunderStorm(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw13Snow(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void draw50Mist(int16_t x, int16_t y, int16_t size = 1, bool day = true);
        void drawTemperatureGauge(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
        void drawTemperatureIcon(float temperature, bool isMetric, int16_t x, int16_t y, int16_t size = 1);
        void drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor);
        void drawHumidityGauge(float humidity, int16_t x, int16_t y, int16_t size = 1);
        void drawHumidityIcon(float humidity, int16_t x, int16_t y, int16_t size = 1);
        void drawHumidity(float humidity, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor);
        void drawForecastDetails(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y, int16_t index, bool showTime);        
        void drawMemoryBar(int16_t x, int16_t y, int16_t size);
        void drawWiFiSignal(int16_t x, int16_t y, int16_t size, uint16_t backgroundColor = UINT16_MAX);        
    public:
        DisplayWeather();
        void init();
        void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t scale = 1);
        void drawSensorData(int16_t x, int16_t y, SensorData *internalSensorData, SensorData *externalSensorData);
        void drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y);
        void drawForecastHourly(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y);        
        void drawForecastDaily(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y);
        void drawHeader(bool externalUpdated, bool currentUpdated, bool forecastHourlyUpdated, bool forecastDailyUpdated, time_t timeUpdated);
        void drawFooter(SensorData *externalSensorData, OpenWeatherMapCurrentData *currentWeather);
};

#endif