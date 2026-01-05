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

    char connectInfo[96] = "";
    sprintf(connectInfo, "[HTTP] Requesting resource at http://%s:%u/api/json/utc/now\n", server, port);
	Serial.println(connectInfo);

    // Send HTTP GET request
    Serial.println(F("[HTTP] connected, now GETting data"));
    // TODO: Figure out why Connection: close truncates client.read()
    client.println(F("GET /api/json/utc/now HTTP/1.0"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Connection: keep-alive"));
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
    if (parseFileTime(response)) {
        lastUpdate = millis();
        return true;
    } else {
        Serial.println(F("Failed to parse time"));
        lastUpdate = 0;
        return false;
    }
}

// Extract "currentFileTime" from JSON response
bool HTTPTimeClient::parseFileTime(String response) {
    int idx = response.indexOf(F("\"currentFileTime\":"));
    if (idx < 0) return false;
    int start = idx + 18;
    int end = response.indexOf(",", start);
    if (end < 0) return false;
    String filetimeStr = response.substring(start, end);
    //Serial.print("FileTime: ");
    //Serial.println(filetimeStr);
    //FileTime to EpcohTime:
    // 133814487212461578 / 10000000 = 13381448721.2461578 seconds since Jan 1, 1601.
    // 13381448721.2461578 - 11644473600 = 1737000000 (approximately) seconds since Jan 1, 1970 (Unix epoch). 
    uint64_t fileTime = parseUint64(filetimeStr.c_str());
    epochTime = (fileTime / 10000000) - 11644473600;
    return true;
}

uint64_t HTTPTimeClient::parseUint64(const char* s) {
    uint64_t value = 0;

    while (*s) {
        char c = *s++;
        if (c < '0' || c > '9') break;
        value = value * 10 + (c - '0');
    }
    return value;
}
