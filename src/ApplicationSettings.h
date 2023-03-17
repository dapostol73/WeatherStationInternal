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

// OpenWeatherMap Settings
// Sign up here to get an API key:
// https://docs.thingpulse.com/how-tos/openweathermap-key/
// Language code from this list:
// Arabic - ar, Bulgarian - bg, Catalan - ca, Czech - cz, German - de, Greek - el,
// English - en, Persian (Farsi) - fa, Finnish - fi, French - fr, Galician - gl,
// Croatian - hr, Hungarian - hu, Italian - it, Japanese - ja, Korean - kr,
// Latvian - la, Lithuanian - lt, Macedonian - mk, Dutch - nl, Polish - pl,
// Portuguese - pt, Romanian - ro, Russian - ru, Swedish - se, Slovak - sk,
// Slovenian - sl, Spanish - es, Turkish - tr, Ukrainian - ua, Vietnamese - vi,
// Chinese Simplified - zh_cn, Chinese Traditional - zh_tw.
struct OpenWeatherInfo
{
    bool IsMetric = true;
    const char *AppID = "";
    const char *Location = "";
    const char *Language = "en";

    OpenWeatherInfo() = default;

    OpenWeatherInfo(const char* appID, const char* location, bool isMetric = true, const char* language = "en")
    {
        AppID = appID;
        Location = location;
        IsMetric = isMetric;
        Language = language;
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

ApplicationSettings Home(WiFiConnection("homessid", "homepw123"),
                         OpenWeatherInfo("homeAppID", "homeLocationID"),
                         ThingSpeakInfo("homeWriteAPIKey", "homeReadAPIKey"));
ApplicationSettings Office(WiFiConnection("officessid", "officepw123"),
                           OpenWeatherInfo("officeAppID", "officeLocationID"),
                           ThingSpeakInfo("officeWriteAPIKey", "officeReadAPIKey"));

ApplicationSettings AppSettings[] = { Home, Office };
uint8_t AppSettingsCount = 2;

#endif