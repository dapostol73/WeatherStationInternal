#ifndef _WIFI_CONNECTION_H_
#define _WIFI_CONNECTION_H_

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

#endif