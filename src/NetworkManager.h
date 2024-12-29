#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <WiFiEspAT.h>

#include "ApplicationSettings.h"

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

class NetworkManager
{
    private:

    public:
        NetworkManager();
        bool init();
        bool isConnected();
        int scanSettingsID(ApplicationSettings* aSettings, uint16_t nSettings);
        bool connectWiFi(WiFiConnection wiFiConnection, uint16_t retryAttempts = 2, uint16_t retryDelay = 20);
        String getLocalIP();
};

#endif