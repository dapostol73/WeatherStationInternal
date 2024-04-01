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
#define HMIN 0
#define HMAX 4095
#define VMIN 0
#define VMAX 4095
#define HRES 800 /* Default screen resulution for X axis */
#define VRES 480 /* Default screen resulution for Y axis */
#define XYSWAP 1 // 0 or 1 (true/false)
#ifdef LCDWIKITOUCH
	LCDWIKI_TOUCH touch(TP_CS, TP_CLK, TP_OUT, TP_IN, TP_IRQ); //tcs,tclk,tdout,tdin,tirq
#else
	TFT_Touch touch(TP_CS, TP_CLK, TP_IN, TP_OUT);
#endif


long lastTouchTime = LONG_MIN;

//******************************//
// Time Client Settings         //
//******************************//
const int TIME_INTERVAL_SECS = 30 * 60; // Check time every 30 minutes

#define TZ              -8     // (utc+) TZ in hours
#define DST_MN          0      // use 60mn for summer time in some countries

#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", TZ_SEC, TIME_INTERVAL_SECS);

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
	setTime((time_t)timeClient.getEpochTime());
	uint8_t m = month();
	uint8_t d = day();
	uint8_t w = weekday();
	uint8_t h = hour();

	// Check to see if we are in Daylight Saving Time (DST)
	bool inDST = false;
	// Between April to October 
	if (m > 3 && m < 11) 
	{
		inDST = true;
  	}
	// After second Sunday in March
	if (m == 3 && (h + 24UL * d) >= (3 + 24UL * (14 - w))) 
	{
		inDST = true;
	}
	// After first Sunday in November
	if (m == 11 && (h + 24UL * d) < (1 + 24UL * (7 - w))) {
		inDST = true;
	}

	if (inDST)
	{
		adjustTime(3600);
	}
}
