#include "NetworkManager.h"


NetworkManager::NetworkManager()
{
}

bool NetworkManager::init()
{
	#ifdef SERIAL_LOGGING
	String intialMsg = "Intializing WiFi module.";
	Serial.println(intialMsg);
	#endif

	WiFi.init(&Serial3);

	delay(1000);;
	WiFi.setAutoConnect(true);
	WiFi.setPersistent(false);

	if (WiFi.status() == WL_NO_SHIELD)
	{
		#ifdef SERIAL_LOGGING
        String initialErr = "Communication with WiFi module failed!";
		Serial.println(initialErr);
		#endif
		// don't continue
		return false;
	}

	return true;
}

bool NetworkManager::isConnected()
{
	return (WiFi.status() == WL_CONNECTED);
}

int NetworkManager::scanSettingsID(ApplicationSettings* appSettings, uint16_t numOfSettings)
{
	#ifdef SERIAL_LOGGING
	String scanMsg = "Scanning for WiFi SSID.";
	Serial.println(scanMsg);
	#endif
	uint8_t id = 0;
	int8_t numNetworks = WiFi.scanNetworks();
	#ifdef SERIAL_LOGGING
	Serial.println("Number of networks found: " + String(numNetworks));
	#endif

	if (numNetworks == 0)
	{
		delay(2500);
		numNetworks = WiFi.scanNetworks();
	}

	for (uint8_t i=0; i<numNetworks; i++)
	{
		String ssid = WiFi.SSID(i);
		#ifdef SERIAL_LOGGING
		Serial.println(ssid + " (" + String(WiFi.RSSI(i)) + ")");
		#endif
		for (uint8_t j=0; j < numOfSettings; j++)
		{
			const char* appSsid = appSettings[j].WifiSettings.SSID;
			#ifdef SERIAL_LOGGING
			Serial.println("Checking: " + String(appSsid));
			#endif
			if (strcasecmp(appSsid, ssid.c_str()) == 0)
			{
				#ifdef SERIAL_LOGGING
				Serial.println("Found: " + String(ssid));
				#endif
				appSettings[j].WifiSettings.Avialable = true;
				appSettings[j].WifiSettings.Strength = WiFi.RSSI(i);

				if (appSettings[j].WifiSettings.Strength > appSettings[id].WifiSettings.Strength)
				{
					id = j;
				}
			}
		}
	}

	#ifdef SERIAL_LOGGING
	Serial.println("Using WiFi " + String(appSettings[id].WifiSettings.SSID));
	#endif
	WiFi.disconnect();
	return id;
}

/// @brief 
/// @param retryAttempts number of reconnect attempts if failed
/// @param retryDelay in seconds
/// @return 
bool NetworkManager::connectWiFi(WiFiConnection wiFiConnection, uint16_t retryAttempts, uint16_t retryDelay)
{
	if (!wiFiConnection.Avialable)
	{
		#ifdef SERIAL_LOGGING
		char connectErr[48] = "";
		sprintf(connectErr, "No WiFi connections found for %s!", wiFiConnection.SSID);
		Serial.println(connectErr);
		#endif
		return false;
	}

	WiFi.disconnect();
	WiFi.begin(wiFiConnection.SSID, wiFiConnection.Password);
	WiFi.sleepMode(WIFI_NONE_SLEEP);
	#ifdef SERIAL_LOGGING
	char infoMsg[] = "Waiting for connection to WiFi";
	Serial.println(infoMsg);
	#endif

	uint8_t attempts = 0;
	uint8_t attemptMax = 3;
	while (WiFi.status() != WL_CONNECTED && attempts < attemptMax)
	{
		delay(1000);

		#ifdef SERIAL_LOGGING
		Serial.print('.');
		#endif
		++attempts;
	}
	#ifdef SERIAL_LOGGING
	Serial.println();
	#endif

	uint8_t retry = 0;
	while(WiFi.status() != WL_CONNECTED)
	{
		if (retry < retryAttempts)
		{
			delay(1000L*retryDelay);
			connectWiFi(wiFiConnection, 0, retryDelay);
			++retry;
		}
	}

	return isConnected();
}

String NetworkManager::getLocalIP()
{
	IPAddress ip = WiFi.localIP();
	char ipInfo[20] = "";
	sprintf(ipInfo, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	return ipInfo;
}
