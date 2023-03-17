#ifndef _APP_SETTINGS_
#define _APP_SETTINGS_

struct WiFiConnection
{
    const char* SSID = "Invalid";
    const char* Password = "Unknown";
    int16_t Strength = -100;
    bool Avialable = 0;

    WiFiConnection() = default;

    WiFiConnection(const char* ssid, const char* password)
    {
        SSID = ssid;
        Password = password;
    }
};

struct ThingSpeakInfo
{
    int16_t Port = 80;
    const char *Host = "api.thingspeak.com"; //IP address of the thingspeak server
    const char *APIKeyWrite = ""; //Your own thingspeak write api_key
    const char *APIKeyRead = "";	 //Your own thingspeak read api_key

    ThingSpeakInfo() = default;

    ThingSpeakInfo(const char* apiKeyWrite, const char* apiKeyRead)
    {
        APIKeyWrite = apiKeyWrite;
        APIKeyRead = apiKeyRead;
    }
};

struct ApplicationSettings
{
    WiFiConnection WifiSettings;
    ThingSpeakInfo ThingSpeakSettings;

    ApplicationSettings() = default;

    ApplicationSettings(WiFiConnection wifiSettings, ThingSpeakInfo thingSpeakSettings)
    {
        WifiSettings = wifiSettings;
        ThingSpeakSettings = thingSpeakSettings;
    }
};

ApplicationSettings Home(WiFiConnection("homessid", "homepw123"), ThingSpeakInfo("homeWriteAPIKey", "homeReadAPIKey"));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"), ThingSpeakInfo("officeWriteAPIKey", "officeReadAPIKey"));

ApplicationSettings AppSettings[] = { Home, Office };
uint8_t AppSettingsCount = 2;

#endif