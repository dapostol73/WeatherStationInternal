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
    sprintf(connectInfo, "[HTTP] Requesting resource at http://%s:%u/generate_204\n", server, port);
	Serial.println(connectInfo);

    // Send HTTP GET request
    Serial.println(F("[HTTP] connected, now GETting data"));
    // TODO: Figure out why Connection: close truncates client.read()
    client.println(F("GET /generate_204 HTTP/1.1"));
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
bool HTTPTimeClient::parseFileTime(String response)
{
    int idx = response.indexOf("Date:");
    if (idx < 0)
        return false;

    int end = response.indexOf("\r\n", idx);
    if (end < 0)
        return false;

    String date = response.substring(idx + 5, end);
    date.trim();

    char weekday[4];
    char month[4];
    int day, year, hour, minute, second;

    if (sscanf(date.c_str(),
               "%3s, %d %3s %d %d:%d:%d GMT",
               weekday,
               &day,
               month,
               &year,
               &hour,
               &minute,
               &second) != 7)
    {
        return false;
    }

    epochTime = dateToEpoch(year, month, day, hour, minute, second);
    return true;
}

uint8_t HTTPTimeClient::monthToNumber(const char* month)
{
    static const char* months[] =
    {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };

    for (uint8_t i = 0; i < 12; i++)
    {
        if (strcmp(month, months[i]) == 0)
            return i + 1;
    }

    return 0;
}

bool HTTPTimeClient::isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) ||
            (year % 400 == 0));
}

unsigned long HTTPTimeClient::dateToEpoch(
    int year,
    const char* month,
    int day,
    int hour,
    int minute,
    int second)
{
    static const uint8_t daysInMonth[] =
    {
        31,28,31,30,31,30,
        31,31,30,31,30,31
    };

    uint8_t monthNum = monthToNumber(month);
    if (monthNum == 0)
        return 0;

    unsigned long days = 0;

    // Days from 1970 up to this year
    for (int y = 1970; y < year; y++)
        days += isLeapYear(y) ? 366 : 365;

    // Days from previous months this year
    for (uint8_t m = 1; m < monthNum; m++)
    {
        days += daysInMonth[m - 1];

        if (m == 2 && isLeapYear(year))
            days++;
    }

    // Days this month
    days += day - 1;

    return days * 86400UL +
           hour * 3600UL +
           minute * 60UL +
           second;
}
