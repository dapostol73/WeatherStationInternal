#ifndef _APPLICATION_SETTINGS_H_
#define _APPLICATION_SETTINGS_H_

#include "WiFiConnection.h"
#include "OpenWeatherInfo.h"
#include "ThingSpeakInfo.h"

struct ApplicationSettings
{
    WiFiConnection WifiSettings;
    OpenWeatherInfo OpenWeatherSettings;
    ThingSpeakInfo ThingSpeakSettings;

    ApplicationSettings() = default;

    ApplicationSettings(WiFiConnection wifiSettings, OpenWeatherInfo openWeatherSettings, ThingSpeakInfo thingSpeakSettings)
    {
        WifiSettings = wifiSettings;
        OpenWeatherSettings = openWeatherSettings;
        ThingSpeakSettings = thingSpeakSettings;
    }
};

#endif
