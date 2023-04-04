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
    unsigned long ChannelID;

    ThingSpeakInfo() = default;

    ThingSpeakInfo(const char* apiKeyWrite, const char* apiKeyRead, unsigned long channelID)
    {
        APIKeyWrite = apiKeyWrite;
        APIKeyRead = apiKeyRead;
        ChannelID = channelID;
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

ApplicationSettings Home(WiFiConnection("a12studiosinc", "apollo1973"),
                         OpenWeatherInfo("6ef72f6a0b2b7849e74ac530ce47d067", "6090785"),
                         ThingSpeakInfo("EMCNAORN3ZXKCFW1", "3MVBTFB81C5DGD27", 1980518));
ApplicationSettings Office(WiFiConnection("a12studiosOG", "apollo1973"),
                           OpenWeatherInfo("6ef72f6a0b2b7849e74ac530ce47d067", "6090785"),
                           ThingSpeakInfo("EMCNAORN3ZXKCFW1", "3MVBTFB81C5DGD27", 1980518));
ApplicationSettings Whistler(WiFiConnection("SHAW-74C6", "cellar0933beard"),
                             OpenWeatherInfo("6ef72f6a0b2b7849e74ac530ce47d067", "6180144"),
                             ThingSpeakInfo("97KZEPIAGEOWQ9DG", "9RE75XTU78B17IXV", 2047687));

ApplicationSettings AppSettings[] = { Home, Office, Whistler };
uint8_t AppSettingsCount = 3;

#endif