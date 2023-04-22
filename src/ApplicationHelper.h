#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <Arduino.h>
#include <limits.h>
#include <TimeLib.h>
#include <TFT_Touch.h>
#include <WiFiEspAT.h>
#include <NTPClient.h>

enum TouchResult
{
	NONE,
	UPDATE,
	FORWARD,
	BACKWARD
};

extern void initHelpers();

extern TouchResult touchTest();

extern void updateSystemTime();

#endif