#ifndef _WIFI_INFO_
#define _WIFI_INFO_

struct WiFiConnection
{
    const char* SSID;
    const char* Password;
    int16_t Strength = -100;
    bool Avialable = 0;

    WiFiConnection(const char* ssid, const char* password)
    {
        SSID = ssid;
        Password = password;
    }
};


WiFiConnection Home("homeSSID", "homePS123");
WiFiConnection Office("officeSSID", "officePS123");

WiFiConnection WiFiConnections[] = { Home, Office };
uint8_t WiFiConnectionsCount = 2;

#endif