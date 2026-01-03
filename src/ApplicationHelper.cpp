#include "ApplicationHelper.h"

// LCDWIKI_TOUCH my_touch(53,52,50,51,44); // tcs, tclk, tdout, tdin, tirq
//Set the pins to the correct ones for your development shield or breakout board.
//This demo use the BREAKOUT BOARD only and use these 8bit data lines to the LCD,
//pin usage as follow:
//             CS  CD  WR  RD  RST  D0  D1  D2  D3  D4  D5  D6  D7  D8  D9  D10  D11  D12  D13  D14  D15 
//Arduino Mega 40  38  39  43  41   /   /   /   /   /   /   /   /   22  23  24   25   26   27   28   29
//             TP_IRQ  MOSI  MISO  TP_CS  EX_CLK
//Arduino Mega   44     51    50    53      52
#define TP_CS  53  /* Chip select pin (T_CS) of touch screen */
#define TP_CLK 52  /* Clock pin (T_CLK) of touch screen */
#define TP_IN  51  /* Data in pin (T_DIN) of touch screen */
#define TP_OUT 50  /* Data out pin (T_DO) of touch screen */
#define TP_IRQ 44  /* Interupt pin (T_IRQ) of touch screen */

#ifdef DISPLAY_ILI9488
	#define HRES 480 /* Default screen resulution for X axis */
	#define VRES 320 /* Default screen resulution for Y axis */
	#define TTOP 40 /* Touch screen top in pixels */
	#define TBCK 180 /* Touch screen back in pixels */
	#define TFWD 300 /* Touch screen foward in pixels */
	#define DROT 1 /* Touch display rotaion */
	#define SROT 3 /* Touch screen rotaion */
#else
	#define HRES 800 /* Default screen resulution for X axis */
	#define VRES 480 /* Default screen resulution for Y axis */
	#define TTOP 60 /* Touch screen top in pixels */
	#define TBCK 300 /* Touch screen back in pixels */
	#define TFWD 500 /* Touch screen foward in pixels */
	#define DROT 1 /* Touch display rotaion */
	#define SROT 1 /* Touch screen rotaion */
#endif

#ifndef LCDWIKITOUCH
	#define HMIN 0
	#define HMAX 4095
	#define VMIN 0
	#define VMAX 4095
	#define XYSWAP 1 // 0 or 1 (true/false)
#endif

#ifdef LCDWIKITOUCH
	LCDWIKI_TOUCH touch(TP_CS, TP_CLK, TP_OUT, TP_IN, TP_IRQ); //tcs,tclk,tdout,tdin,tirq
#else
	TFT_Touch touch(TP_CS, TP_CLK, TP_IN, TP_OUT);
#endif


long lastTouchTime = LONG_MIN;

//******************************//
// Time Client Settings         //
//******************************//
const unsigned long TIME_INTERVAL_SECS = 30 * 60 * 1000UL; // Check time every 30 minutes
const long TZ_SEC = 0 * 60 * 60; // First digit is time zone, 0 = UTC time 

HTTPTimeClient httpTimeClient;

#ifdef TIMEZONE_ATLANTIC
// North America Atlantic Time Zone (Moncton, Halifax, Charlottetown)
TimeChangeRule naADT = {"ADT", Second, Sun, Mar, 2, -180};
TimeChangeRule naAST = {"AST", First, Sun, Nov, 2, -240};
Timezone naAT(naADT, naAST);
#endif

#ifdef TIMEZONE_PACIFIC
// North America Pacific Time Zone (Las Vegas, Los Angeles, Vancouver)
TimeChangeRule naPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule naPST = {"PST", First, Sun, Nov, 2, -480};
Timezone naPT(naPDT, naPST);
#endif

void initHelpers()
{
#ifdef SERIAL_LOGGING
	Serial.println(F("Intializing Touch Screen..."));
#endif

#ifdef LCDWIKITOUCH
	touch.TP_Init(DROT, HRES, VRES);
	touch.TP_Set_Rotation(SROT);
#else
	touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP);
	touch.setRotation(SROT);
#endif

#ifdef SERIAL_LOGGING
	Serial.println(F("Intializing Time Client..."));
#endif
	httpTimeClient.setUpdateInterval(TIME_INTERVAL_SECS);
	httpTimeClient.forceUpdate();
}

TouchResult touchTest()
{
	TouchResult result = { false, 0, 0, NONE };
#ifdef LCDWIKITOUCH
	touch.TP_Scan(0);
	if (touch.TP_Get_State()&TP_PRES_DOWN) 
	{
		result.X = touch.x;
		result.Y = touch.y;
		result.IsTouched = true;
	}
#else
	if (touch.Pressed())
	{
		result.X = touch.X();
		result.Y = touch.Y();
		result.IsTouched = true;
	}
#endif

    if (result.IsTouched && millis() - lastTouchTime > 100)
	{
		#ifdef SERIAL_LOGGING
		Serial.print(F("Touch at X,Y: ("));
		Serial.print(result.X);
		Serial.print(F(","));
		Serial.print(result.Y);
		Serial.println(F(")"));
		#endif

		if (result.X > TFWD)
		{
			result.State = FORWARD;
		}
		else if (result.X < TBCK)
		{
			result.State = BACKWARD;		
		}
		else if (result.Y < TTOP)
		{
			result.State = UPDATE;		
		}
		lastTouchTime = millis();

		#ifdef SERIAL_LOGGING
		Serial.print(F("Touch at State: "));
		Serial.println(result.State);
		#endif
	}

    return result;
}

// TODO: Figure out bug on switch over.
void updateSystemTime()
{
	if (!httpTimeClient.isTimeSet())
	{
		#ifdef SERIAL_LOGGING
		Serial.println(F("Time not set, forcing update."));
		#endif
		httpTimeClient.forceUpdate();
	}
	else
	{
		httpTimeClient.update();
	}	
	time_t utc = (time_t)httpTimeClient.getEpochTime();
#ifdef TIMEZONE_ATLANTIC
	setTime(naAT.toLocal(utc));	
#endif
#ifdef TIMEZONE_PACIFIC
	setTime(naPT.toLocal(utc));
#endif
}

void printCurrentTime()
{
	#ifdef SERIAL_LOGGING
	char datetime[32];
	sprintf_P(datetime,
			  PSTR("Time: %d %d, %d  %d:%02d %s"),
			  month(),
			  day(),
			  year(),
			  hourFormat12(),
			  minute(),
			  (isAM() ? "AM" : "PM"));
	Serial.println(datetime);
	#endif
}

String formatTimeISO8601UTC(time_t time)
{
	tmElements_t timeinfo;
	breakTime(time, timeinfo);

	char buf[25];
		snprintf(
		buf,
		sizeof(buf),
		"%04d-%02d-%02dT%02d:%02d:%02dZ",
		tmYearToCalendar(timeinfo.Year), // converts offset → full year
		timeinfo.Month,
		timeinfo.Day,
		timeinfo.Hour,
		timeinfo.Minute,
		timeinfo.Second
	);
	return String(buf);
}
