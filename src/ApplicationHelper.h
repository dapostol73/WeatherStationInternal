#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <Arduino.h>
#include <limits.h>
#include <TimeLib.h>
#include <WiFiEspAT.h>
#include <NTPClient.h>

#define LCDWIKITOUCH
#ifdef LCDWIKITOUCH
	#include <LCDWIKI_TOUCH.h>
#else
	#include <TFT_Touch.h>
#endif

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

extern int availableMemory();

extern void printMemory();

#endif