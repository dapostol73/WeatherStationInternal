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
const String WDAY_NAMES[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
const String MONTH_NAMES[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

class DisplayWeather : public DisplayControl
{
    private:
        uint16_t palette[4] = {BLACK, WHITE, GOLD, DEEPSKYBLUE};
        NTPClient *m_timeClient;
        time_t m_timeUpdated;
        char m_lastTimeUpdated[40] = "?Unkown";
        bool m_isMetric = true;

        void drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor);
        void drawForecastDetails(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y, int16_t dayIndex);
        void drawWiFiSignal(int16_t x, int16_t y, int16_t size);
        void drawSun(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawCloud(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawThunder(int16_t x, int16_t y, int16_t size, uint16_t color);
        void drawRain(int16_t x, int16_t y, int16_t size, bool heavy, uint16_t color);
    public:
        DisplayWeather();
        void init();
        void drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t scale = 1);
        void drawDateTime(int16_t x, int16_t y);
        void drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y);
        void drawForecast(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y);
        void drawHeader(bool currentWeathersUpdated, bool forecastWeathersUpdated, time_t timeUpdated);
        void drawFooter(OpenWeatherMapCurrentData *currentWeather);
};

#endif