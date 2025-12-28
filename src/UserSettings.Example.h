//*****************************************************************************
// Rename to UserSettings.h and modify for your local settings.
// Add as many WiFi and ThingSpeak configs as needed.
//*****************************************************************************

#ifndef _USER_SETTINGS_H_
#define _USER_SETTINGS_H_

#include "ApplicationSettings.h"

#ifdef THINGSPEAK_SENSOR
ApplicationSettings Home(WiFiConnection("homessid", "homepw123"),
                         OpenWeatherInfo("homeAppID", "homeLocationID"),
                         ThingSpeakInfo("homeWriteAPIKey", "homeReadAPIKey", 1234567));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"),
                           OpenWeatherInfo("officeAppID", "officeLocationID"),
                           ThingSpeakInfo("officeWriteAPIKey", "officeReadAPIKey", 1234567));
#elif CANADIAN_TIDES
ApplicationSettings Home(WiFiConnection("homessid", "homepw123"),
                         OpenWeatherInfo("homeAppID", "homeLocationID"),
                         CanadianHyrdographicInfo("homeStationID12345567890"));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"),
                           OpenWeatherInfo("officeAppID", "officeLocationID"),
                           CanadianHyrdographicInfo("officeStationID123455678"));
#else
ApplicationSettings Home(WiFiConnection("homessid", "homepw123"),
                         OpenWeatherInfo("homeAppID", "homeLocationID"));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"),
                           OpenWeatherInfo("officeAppID", "officeLocationID"));
#endif


ApplicationSettings AppSettings[] = { Home, Office };
uint8_t AppSettingsCount = sizeof(AppSettings) / sizeof(AppSettings[0]);

#endif