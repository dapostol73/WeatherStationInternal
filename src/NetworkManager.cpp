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
	delay(1000);
	WiFi.sleepMode(WIFI_NONE_SLEEP);
	WiFi.setAutoConnect(true);

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
	Serial.println("Scanning for WiFi SSID.");
	#endif
	uint8_t id = 0;

	if (numOfSettings < 2)
	{

		if (numOfSettings == 1)
		{
			appSettings[id].WifiSettings.Avialable = true;
			#ifdef SERIAL_LOGGING
			Serial.println("Only one WiFi settings, assume it is avialable.");
			#endif
		}
		else
		{
			#ifdef SERIAL_LOGGING
			Serial.println("No WiFi settings found, skipping network setup.");
			#endif
		}
		return id;
	}

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

/// @brief Used for connecting to WiFi first time, by default will try 15 times every 20 seonds
/// @param wiFiConnection connection to use
/// @param retryAttempts attempts to connect 15
/// @param retryDelay how many seconds to delay between attempts (min 5 sec)
/// @return true if connection was successful
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

	WiFi.begin(wiFiConnection.SSID, wiFiConnection.Password);
	#ifdef SERIAL_LOGGING
	char infoMsg[] = "Waiting for connection to WiFi";
	Serial.println(infoMsg);
	#endif

	// Give it 5 seconds to establish connection.
	uint8_t attempts = 0;
	uint8_t attemptMax = max(retryDelay*2, 10);
	while (WiFi.status() != WL_CONNECTED && attempts < attemptMax)
	{
		delay(500);
		#ifdef SERIAL_LOGGING
		Serial.print('.');
		#endif
		++attempts;
	}
	#ifdef SERIAL_LOGGING
	Serial.println();
	#endif

	uint8_t retry = 0;
	while(WiFi.status() != WL_CONNECTED && retry < retryAttempts)
	{
		WiFi.disconnect();
		delay(500);
		connectWiFi(wiFiConnection, 0, retryDelay);
		retry++;
		#ifdef SERIAL_LOGGING
		char connectInfo[96] = "";
		sprintf(connectInfo, "Reconnect attempt %i of %i returned status %i for %s!", retry, retryAttempts, WiFi.status(), wiFiConnection.SSID);
		Serial.println(connectInfo);
		#endif
	}

	return isConnected();
}

String NetworkManager::sendATcommand(const char *atCommand, unsigned long milliseconds)
{
	String result;
	#ifdef SERIAL_LOGGING
	Serial.print("Sending: ");
	Serial.println(atCommand);
	#endif
	Serial3.println(atCommand);
	unsigned long startTime = millis();
	#ifdef SERIAL_LOGGING
	Serial.print("Received: ");
	#endif

	while (millis() - startTime < milliseconds)
	{
		if (Serial3.available())
		{
			char c = Serial3.read();
			#ifdef SERIAL_LOGGING
			Serial.write(c);
			#endif
			result += c;  // append to the result string
		}
	}

	#ifdef SERIAL_LOGGING
	Serial.println();  // new line after timeout.
	#endif
	return result;
}

String NetworkManager::getLocalIP()
{
	IPAddress ip = WiFi.localIP();
	char ipInfo[20] = "";
	sprintf(ipInfo, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
	return ipInfo;
}
