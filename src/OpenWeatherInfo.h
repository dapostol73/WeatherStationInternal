#ifndef _OPEN_WEATHER_INFO_H_
#define _OPEN_WEATHER_INFO_H_

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

#endif