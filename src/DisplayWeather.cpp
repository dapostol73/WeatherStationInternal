#include "DisplayWeather.h"
#include "Icons/OpenWeatherIcons.h"

// 'snowflake_mask_sml', 19x21px
const uint8_t snowflake_mask_sml [] PROGMEM = {
	0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 
	0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 
	0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 
	0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xe0
};

// 'snowflake_mask_lrg', 38x41px
const uint8_t snowflake_mask_lrg [] PROGMEM = {
	0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 
	0x0e, 0x78, 0xe0, 0x00, 0x00, 0x1f, 0x79, 0xe0, 0x00, 0x00, 0x0f, 0x7b, 0xe0, 0x00, 0x01, 0xc7, 
	0xff, 0xc7, 0x00, 0x03, 0xc3, 0xff, 0x8f, 0x00, 0x01, 0xc1, 0xff, 0x0f, 0x00, 0x21, 0xc0, 0xfe, 
	0x0e, 0x10, 0x79, 0xe0, 0x7c, 0x0e, 0x78, 0x7f, 0xe0, 0x78, 0x1f, 0xf8, 0x3f, 0xe0, 0x78, 0x1f, 
	0xf8, 0x1f, 0xf0, 0x78, 0x1f, 0xe0, 0x07, 0xf0, 0x78, 0x3f, 0x80, 0x1f, 0xfd, 0xfe, 0x7f, 0xe0, 
	0x7f, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xbf, 0xff, 0xf7, 0xfc, 0xfc, 0x0f, 0x87, 0xc0, 0x7c, 0x20, 
	0x07, 0x03, 0x80, 0x08, 0x00, 0x07, 0x03, 0x80, 0x00, 0x00, 0x07, 0x03, 0x80, 0x00, 0x78, 0x0f, 
	0x87, 0xc0, 0x7c, 0xff, 0xbf, 0xef, 0xf7, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0xfc, 0x3f, 0xfd, 0xfe, 
	0xff, 0xf0, 0x07, 0xf0, 0x78, 0x3f, 0x80, 0x0f, 0xf0, 0x78, 0x1f, 0xe0, 0x3f, 0xe0, 0x78, 0x1f, 
	0xf0, 0x7f, 0xe0, 0x78, 0x1f, 0xf8, 0x79, 0xe0, 0x78, 0x1e, 0x78, 0x31, 0xc0, 0xfc, 0x0e, 0x10, 
	0x01, 0xc1, 0xfe, 0x0f, 0x00, 0x03, 0xc3, 0xff, 0x0f, 0x00, 0x03, 0xc7, 0xff, 0x87, 0x00, 0x01, 
	0x8f, 0xff, 0xc2, 0x00, 0x00, 0x1f, 0x7b, 0xe0, 0x00, 0x00, 0x0e, 0x79, 0xe0, 0x00, 0x00, 0x00, 
	0x78, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00
};


DisplayWeather::DisplayWeather()
{

}

void DisplayWeather::init()
{
    DisplayControl::init(1, &CalibriRegular8pt7b);
}

/// @brief Sun 24*24
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawSun(int16_t x, int16_t y, int16_t size, int16_t radius, uint16_t color)
{
	size = max(size, 1);
	radius *= size;
	DisplayControl::getDisplay()->fillCircle(x+radius, y+radius, radius, color);
}

/// @brief Cloud is 58*36 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawCloud(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	DisplayControl::getDisplay()->fillCircle(10*size+x, 25*size+y, 11*size, color);
	DisplayControl::getDisplay()->fillCircle(24*size+x, 12*size+y, 12*size, color);
	DisplayControl::getDisplay()->fillCircle(41*size+x, 16*size+y, 7*size,  color);
	DisplayControl::getDisplay()->fillCircle(50*size+x, 28*size+y, 8*size,  color);
	DisplayControl::getDisplay()->fillRoundRect(30*size+x, 10*size+y, 19*size, 16*size, 7*size, color);
	DisplayControl::getDisplay()->fillRoundRect(10*size+x, 26*size+y, 40*size, 11*size, 2*size, color);
	DisplayControl::getDisplay()->fillRect(20*size+x, 20*size+y, 20*size, 6*size, color);
}

/// @brief Thunder is 12*22 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawThunder(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	fillPolygon(4 * size + x, 0 * size + y,
				0 * size + x, 12 * size + y,
				4 * size + x, 12 * size + y,
				11 * size + x, 0 * size + y,
				color);
	fillPolygon(6 * size + x, 8 * size + y,
				12 * size + x, 8 * size + y,
				0 * size + x, 22 * size + y,
				1 * size + x, 18 * size + y,
				color);
}


void DisplayWeather::drawRainDrop(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	DisplayControl::getDisplay()->fillTriangle(0*size+x, 2*size+y, 2*size+x, 0*size+y, 3*size+x, 3*size+y, color);
	DisplayControl::getDisplay()->fillCircle(1*size+x,  4*size+y, size*2, color);
}

/// @brief Rain Heavy is 26*18 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawRainLight(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	drawRainDrop(0*size+x,  4*size+y,  size, color);//r0.d0
	drawRainDrop(8*size+x,  0*size+y,  size, color);//r1.d0
	drawRainDrop(5*size+x,  11*size+y, size, color);//r1.d1
	drawRainDrop(14*size+x, 4*size+y, size, color);//r2.d2
	drawRainDrop(12*size+x, 15*size+y, size, color);//r2.d1
	drawRainDrop(22*size+x, 0*size+y, size, color);//r3.d0
	drawRainDrop(20*size+x, 11*size+y, size, color);//r3.d1
	drawRainDrop(28*size+x, 4*size+y,  size, color);//r4.d0
}

/// @brief Rain Heavy is 34*24 at size 1
/// @param x 
/// @param y 
/// @param size 
/// @param color 
void DisplayWeather::drawRainHeavy(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	size = max(size, 1);
	drawRainDrop(2*size+x, 4*size+y, size, color);//r0.d0
	drawRainDrop(0*size+x, 12*size+y, size, color);//r0.d1

	drawRainDrop(10*size+x, 0*size+y, size, color);//r1.d0
	drawRainDrop(8*size+x,  8*size+y, size, color);//r1.d1
	drawRainDrop(6*size+x,  16*size+y, size, color);//r1.d2

	drawRainDrop(16*size+x, 4*size+y, size, color);//r2.d0
	drawRainDrop(14*size+x, 12*size+y, size, color);//r2.d1
	drawRainDrop(12*size+x, 20*size+y, size, color);//r2.d2
	
	drawRainDrop(24*size+x, 0*size+y, size, color);//r3.d0
	drawRainDrop(22*size+x, 8*size+y, size, color);//r3.d1
	drawRainDrop(20*size+x, 16*size+y, size, color);//r3.d2

	drawRainDrop(30*size+x, 4*size+y, size, color);//r4.d0
	drawRainDrop(28*size+x, 12*size+y, size, color);//r4.d1
}

void DisplayWeather::drawSnowflake(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	if (size > 1)
	{
		drawMaskBitmap(x, y, 38, 41, snowflake_mask_lrg, color, true, size-1);
	}
	else
	{
		drawMaskBitmap(x, y, 19, 21, snowflake_mask_sml, color, true);
	}	
}

void DisplayWeather::drawSnow(int16_t x, int16_t y, int16_t size, uint16_t color)
{
	drawSnowflake(x, y, size, color);
	drawSnowflake(x+50, y, size+1, color);
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
    DisplayControl::getDisplay()->getTextBounds("8", 0, 0, &x1, &y1, &w, &h);
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
	int16_t radius = w * 0.5;
	x = DisplayControl::getDisplay()->getCursorX() + radius;
	DisplayControl::getDisplay()->drawCircle(x, y + radius-1, radius-1, foregroundColor);
	DisplayControl::getDisplay()->drawCircle(x, y + radius-1, radius-2, foregroundColor);
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
	fillScreen(BLACK);
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
	fillScreen(BLACK);
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
	fillScreen(BLACK);
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
