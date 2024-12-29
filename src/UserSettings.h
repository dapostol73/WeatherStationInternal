#ifndef _USER_SETTINGS_H_
#define _USER_SETTINGS_H_

#include "ApplicationSettings.h"

ApplicationSettings Home(WiFiConnection("homessid", "homepw123"),
                         OpenWeatherInfo("homeAppID", "homeLocationID"),
                         ThingSpeakInfo("homeWriteAPIKey", "homeReadAPIKey", 1234567));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"),
                           OpenWeatherInfo("officeAppID", "officeLocationID"),
                           ThingSpeakInfo("officeWriteAPIKey", "officeReadAPIKey", 1234567));

ApplicationSettings AppSettings[] = { Home, Office };
uint8_t AppSettingsCount = sizeof(AppSettings) / sizeof(AppSettings[0]);

#endif