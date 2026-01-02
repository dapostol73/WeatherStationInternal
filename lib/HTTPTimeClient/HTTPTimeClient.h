#ifndef HTTPTimeClient_h
#define HTTPTimeClient_h

#include <Arduino.h>
#include <WiFiClient.h>

class HTTPTimeClient {
  public:
    HTTPTimeClient(const char* server = "worldtimeapi.org", uint16_t port = 80);
    
    /**
     * This should be called in the main loop of your application. By default an update from the HTTP Server is only
     * made every 60 seconds. This can be configured in the HTTPTimeClient constructor.
     *
     * @return true on success, false on failure
     */
    bool update();

    /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
    bool forceUpdate();

    /**
     * This allows to check if the HTTPTimeClient successfully received a time packet and set the time.
     *
     * @return true if time has been set, else false
     */
    bool isTimeSet() const { return lastUpdate != 0; }

    // Return last fetched UNIX timestamp
    unsigned long getEpochTime() const { return epochTime; }

    // Optional: set update interval (ms)
    void setUpdateInterval(unsigned long interval) { updateInterval = interval; }

  private:
    const char* server;
    uint16_t port;
    unsigned long epochTime;
    unsigned long lastUpdate;
    unsigned long updateInterval;

    bool parseUnixTime(String response);
};

#endif
