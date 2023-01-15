#include "DisplayWeather.h"
#include "Icons/OpenWeatherIcons.h"

DisplayWeather::DisplayWeather()
{

}

void DisplayWeather::init()
{
    DisplayControl::init(1, &CalibriRegular8pt7b);
}

void DisplayWeather::drawWeatherIcon(int16_t x, int16_t y, String iconName, bool center, int16_t scale)
{
	const OpenWeatherIcon icon = getOpenWeatherCropIconFromProgmem(iconName);
	DisplayControl::drawBitmap(x, y, icon.width, icon.height, icon.data, center, scale);
}

void DisplayWeather::drawTemperature(float temperature, bool isMetric, int16_t x, int16_t y, TextAlignment align, uint16_t foregroundColor)
{
	int16_t x1, y1 = 0;
    uint16_t w, h = 0;
    DisplayControl::getDisplay()->getTextBounds("0", 0, 0, &x1, &y1, &w, &h);
	String temp = String(temperature, 1);
	if (align > 0)
	{
		int16_t sw = w * (temp.length() + 2);
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

	DisplayControl::drawString(temp, x, y, TEXT_LEFT, foregroundColor);
	int16_t radius = w * 0.4;
	x = DisplayControl::getDisplay()->getCursorX() + radius;
	DisplayControl::getDisplay()->drawCircle(x, y + radius, radius, foregroundColor);
	x += radius;
	if (isMetric)
	{
		DisplayControl::drawString("C", x, y, TEXT_LEFT, foregroundColor);
	}
	else
	{
		DisplayControl::drawString("F", x, y, TEXT_LEFT, foregroundColor);
	}
}

void DisplayWeather::drawDateTime(int16_t x, int16_t y)
{
	DisplayControl::setFont(&CalibriBold24pt7b);
	char buff[16];
	sprintf_P(buff, PSTR("%s, %02d/%02d/%04d"), WDAY_NAMES[weekday()-1].c_str(), day(), month(), year());
	DisplayControl::drawString(buff, 240, 140, TEXT_CENTER, CYAN);

	sprintf_P(buff, PSTR("%02d:%02d %s"), hourFormat12(), minute(), (isAM() ? "AM" : "PM"));
	DisplayControl::drawString(buff, 240, 180, TEXT_CENTER, CYAN);
}

void DisplayWeather::drawCurrentWeather(OpenWeatherMapCurrentData *currentWeather, int16_t x, int16_t y)
{
	x = 240;
	y = 40;
	drawWeatherIcon(x, y + 90, currentWeather->icon, true, 2);
	DisplayControl::setFont(&CalibriBold24pt7b);
	DisplayControl::drawString(currentWeather->cityName, x, y + 5, TEXT_CENTER, YELLOW);
	DisplayControl::setFont(&CalibriBold16pt7b);
	drawTemperature(currentWeather->temp, m_isMetric, x, y + 160, TEXT_CENTER, CYAN);
	DisplayControl::drawString(currentWeather->description, x, y + 200, TEXT_CENTER, ORANGE);
}

void DisplayWeather::drawForecastDetails(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y, int16_t dayIndex) 
{
	time_t observationTimestamp = forecastWeather[dayIndex].observationTime;
	int16_t day = weekday(observationTimestamp)-1;
	drawWeatherIcon(x, y + 100, forecastWeather[dayIndex].icon, true, 2);
	DisplayControl::setFont(&CalibriBold16pt7b);
	DisplayControl::drawString(WDAY_NAMES[day], x, y + 10, TEXT_CENTER, YELLOW);
	drawTemperature(forecastWeather[dayIndex].temp, m_isMetric, x, y + 170, TEXT_CENTER, CYAN);
	DisplayControl::setFont(&CalibriBold8pt7b);
	DisplayControl::drawString(forecastWeather[dayIndex].description, x, y + 200, TEXT_CENTER, ORANGE);	
}

void DisplayWeather::drawForecast(OpenWeatherMapForecastData *forecastWeather, int16_t x, int16_t y) 
{
	//DisplayControl::setFont(&CalibriBold24pt7b);
	//DisplayControl::drawString(forecastWeather[0].cityName, 240, 40, TEXT_CENTER, YELLOW);
	drawForecastDetails(forecastWeather, 80, 60, 0);
	drawForecastDetails(forecastWeather, 240, 60, 1);
	drawForecastDetails(forecastWeather, 400, 60, 2);	
}

/// @brief Default size when set to 1 is 12x12
/// @param x 
/// @param y 
/// @param size 
void DisplayWeather::drawWiFiSignal(int16_t x, int16_t y, int16_t size)
{
	size = max(size, 1);
	if (WiFi.status() == WL_CONNECTED)
	{
		int32_t strength = WiFi.RSSI();
		DisplayControl::getDisplay()->fillRect(x,        y+size*9, size*2, size*3,  strength > -80 ? WHITE : DIMGREY);
		DisplayControl::getDisplay()->fillRect(x+size*3, y+size*6, size*2, size*6,  strength > -70 ? WHITE : DIMGREY);
		DisplayControl::getDisplay()->fillRect(x+size*6, y+size*3, size*2, size*9,  strength > -60 ? WHITE : DIMGREY);
		DisplayControl::getDisplay()->fillRect(x+size*9, y,        size*2, size*12, strength > -45 ? WHITE : DIMGREY);
	}
	else
	{
		int16_t ly = y + size * 5;
		int16_t lw = size*10;
		for (int16_t t=0; t < size * 2; t++)
		{
			DisplayControl::getDisplay()->drawFastHLine(x+2, ly+t, lw, ORANGERED);
		}
		int16_t r = 6*size;
		int16_t cx = x+r;
		int16_t cy = y+r;
		for (int16_t t=0; t < size * 2; t++)
		{
			DisplayControl::getDisplay()->drawCircle(cx, cy, r-t, ORANGERED);
		}		
	}
}

void DisplayWeather::drawHeader(bool currentWeathersUpdated, bool forecastWeathersUpdated, time_t timeUpdated)
{
	DisplayControl::getDisplay()->fillRect(0, 0, 480, 12, CHARCOAL);
	DisplayControl::getDisplay()->drawFastHLine(0, 13, 480, CYAN);
	DisplayControl::getDisplay()->drawFastHLine(0, 14, 480, CYAN);

	DisplayControl::setFont(&CalibriBold8pt7b);
	DisplayControl::drawChar(4, 0, 'C', currentWeathersUpdated ? GREEN : RED);
	DisplayControl::drawChar(20, 0, 'F', forecastWeathersUpdated ? GREEN : RED);
    sprintf(m_lastTimeUpdated, "%02d/%02d/%04d -- %02d:%02d:%02d", 
            month(timeUpdated), day(timeUpdated), year(timeUpdated), 
            hour(timeUpdated), minute(timeUpdated), second(timeUpdated));
	DisplayControl::drawString(m_lastTimeUpdated, 480, 0, TEXT_RIGHT);	
}

void DisplayWeather::drawFooter(OpenWeatherMapCurrentData *currentWeather)
{
	char time[10];
	sprintf_P(time, PSTR("%02d:%02d"), hour(), minute());

	DisplayControl::getDisplay()->fillRect(0, 280, 480, 320, CHARCOAL);
	DisplayControl::getDisplay()->drawFastHLine(0, 278, 480, CYAN);
	DisplayControl::getDisplay()->drawFastHLine(0, 279, 480, CYAN);
	drawWiFiSignal(450, 286, 2);
	DisplayControl::setFont(&CalibriBold16pt7b);
	DisplayControl::drawString(time, 120, 300, TEXT_CENTER, ORANGE);
	//Serial.println(String(currentWeather->temp));
	drawTemperature(currentWeather->temp, m_isMetric, 360, 300, TEXT_CENTER, ORANGE);
}
