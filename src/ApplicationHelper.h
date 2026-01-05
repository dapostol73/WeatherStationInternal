#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <Arduino.h>
#include <limits.h>
#include <TimeLib.h>
#include <WiFiEspAT.h>
#include <Timezone.h> 
#ifdef HTTP_TIME_CLIENT
	#include <HTTPTimeClient.h>
#else
	#include <NTPClient.h>
#endif

#define LCDWIKITOUCH
#ifdef LCDWIKITOUCH
	#include <LCDWIKI_TOUCH.h>
#else
	#include <TFT_Touch.h>
#endif

enum TouchState
{
	NONE,
	UPDATE,
	FORWARD,
	BACKWARD
};

struct TouchResult
{
	bool IsTouched;
	uint16_t X;
	uint16_t Y;
	TouchState State;
};

extern void initHelpers();

extern TouchResult touchTest();

extern void updateSystemTime();

extern void printCurrentTime();

extern String formatTimeISO8601UTC(time_t time);

#endif