#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <WiFiEspAT.h>

#include "ApplicationSettings.h"

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

//#define RESET_ESPAT_MODULE

class NetworkManager
{
    private:

    public:
        NetworkManager();
        bool init();
        bool isConnected();
        int scanSettingsID(ApplicationSettings* aSettings, uint16_t nSettings);
        bool connectWiFi(WiFiConnection wiFiConnection, uint16_t retryAttempts = 5, uint16_t retryDelay = 60);
        String sendATcommand(const char *atCommand, unsigned long milliseconds = 1000UL);
        String getLocalIP();
};

#endif