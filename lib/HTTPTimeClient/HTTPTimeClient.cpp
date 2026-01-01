#include "HTTPTimeClient.h"

HTTPTimeClient::HTTPTimeClient(const char* server, uint16_t port) {
    this->server = server;
    this->port = port;
    this->epochTime = 0;
    this->lastUpdate = 0;
    this->updateInterval = 60000; // default 60s
}

bool HTTPTimeClient::update() {
    // Prevent too frequent updates
    if (millis() - lastUpdate < updateInterval && this->lastUpdate != 0) {
        return isTimeSet();  // Return true if time is already set
    }

    return forceUpdate();
}

bool HTTPTimeClient::forceUpdate() {

    WiFiClient client;
    if (!client.connect(server, port)) {
        Serial.println(F("Connection failed"));
        return false;
    }

    char connectInfo[72] = "";
    sprintf(connectInfo, "[HTTP] Requesting resource at https://%s:%u/api/ip\n", server, port);
	Serial.println(connectInfo);

    // Send HTTP GET request
    Serial.println(F("[HTTP] connected, now GETting data"));
    // TODO: Figure out why Connection: close truncates client.read()
    client.println(F("GET /api/ip HTTP/1.0"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Connection: close"));
    client.println();

    // Wait for data with timeout
    unsigned long start = millis();
    while (!client.available() && (millis() - start < 5000)) delay(10);
    if (!client.available()) {
        Serial.println(F("No response from server"));
        client.stop();
        return false;
    }

    // Read all response
    String response = "";
    while (client.available()) {
        response += (char)client.read();
    }
    //Serial.println(F("Response received:"));
    //Serial.println(response);
    client.stop();

    // Parse UNIX time
    if (parseUnixTime(response)) {
        lastUpdate = millis();
        return true;
    } else {
        Serial.println(F("Failed to parse time"));
        return false;
    }
}

// Extract "unixtime" from JSON response
bool HTTPTimeClient::parseUnixTime(String response) {
    int idx = response.indexOf("\"unixtime\":");
    if (idx < 0) return false;
    int start = idx + 11;
    int end = response.indexOf(",", start);
    if (end < 0) return false;
    String unixtimeStr = response.substring(start, end);
    epochTime = unixtimeStr.toInt();
    return true;
}
