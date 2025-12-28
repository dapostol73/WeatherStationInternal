#ifndef _APPLICATION_SETTINGS_H_
#define _APPLICATION_SETTINGS_H_

#include "WiFiConnection.h"
#include "OpenWeatherInfo.h"
#ifdef THINGSPEAK_SENSOR
    #include "ThingSpeakInfo.h"
#endif
#ifdef CANADIAN_TIDES
    #include "CanadianHyrdographicInfo.h"
#endif

struct ApplicationSettings
{
    WiFiConnection WifiSettings;
    OpenWeatherInfo OpenWeatherSettings;
    #ifdef THINGSPEAK_SENSOR
        ThingSpeakInfo ThingSpeakSettings;
    #endif
    #ifdef CANADIAN_TIDES
        CanadianHyrdographicInfo CanadianHydrographicSettings;
    #endif

    ApplicationSettings() = default;

    #ifdef THINGSPEAK_SENSOR
    ApplicationSettings(WiFiConnection wifiSettings, OpenWeatherInfo openWeatherSettings, ThingSpeakInfo thingSpeakSettings)
    {
        WifiSettings = wifiSettings;
        OpenWeatherSettings = openWeatherSettings;
        ThingSpeakSettings = thingSpeakSettings;
    }
    #elif defined(CANADIAN_TIDES)
    ApplicationSettings(WiFiConnection wifiSettings, OpenWeatherInfo openWeatherSettings, CanadianHyrdographicInfo canadianHydrographicSettings)
    {
        WifiSettings = wifiSettings;
        OpenWeatherSettings = openWeatherSettings;
        CanadianHydrographicSettings = canadianHydrographicSettings;
    }
    #else
    ApplicationSettings(WiFiConnection wifiSettings, OpenWeatherInfo openWeatherSettings)
    {
        WifiSettings = wifiSettings;
        OpenWeatherSettings = openWeatherSettings;
    }
    #endif
};

#endif
