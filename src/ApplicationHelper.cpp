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

#define HRES 800 /* Default screen resulution for X axis */
#define VRES 480 /* Default screen resulution for Y axis */

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

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

// North America Pacific Time Zone (Las Vegas, Los Angeles, Vancouver)
TimeChangeRule naPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule naPST = {"PST", First, Sun, Nov, 2, -480};
Timezone naPT(naPDT, naPST);

void initHelpers()
{
#ifdef LCDWIKITOUCH
	touch.TP_Init(1, HRES, VRES);
	touch.TP_Set_Rotation(1);
#else
	touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP);
	touch.setRotation(1);
#endif
	timeClient.begin();
}

TouchResult touchTest()
{
	bool touched = false;
	uint16_t xValue;
	uint16_t yValue;
#ifdef LCDWIKITOUCH
	touch.TP_Scan(0);
	if (touch.TP_Get_State()&TP_PRES_DOWN) 
	{
		xValue = touch.x;
		yValue = touch.y;
		touched = true;
	}
#else
	if (touch.Pressed())
	{
		xValue = touch.X();
		yValue = touch.Y();
		touched = true;
	}
#endif

    if (touched && millis() - lastTouchTime > 100)
	{
		#ifdef SERIAL_LOGGING
		Serial.println("Touch at X,Y: (" + String(xValue) + "," + String(yValue) +")" );
		#endif

		if (xValue > 300 && xValue < 500 && yValue < 20)
		{
			return UPDATE;
		}
		else if (xValue > 500)
		{
			return FORWARD;
		}
		else if (xValue < 300)
		{
			return BACKWARD;		
		}
		lastTouchTime = millis();
	}

    return NONE;
}

// TODO: Figure out bug on switch over.
void updateSystemTime()
{
	timeClient.update();
	time_t utc = (time_t)timeClient.getEpochTime();
	setTime(naPT.toLocal(utc));
}
