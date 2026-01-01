/**The MIT License (MIT)
 
 Copyright (c) 2020 by Demetrius Apostolopoulos
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */
// Canadian Hydrographic Service API information
// https://tides.gc.ca/en/web-services-offered-canadian-hydrographic-service
// https://api-sine.dfo-mpo.gc.ca/swagger-ui/index.html

#pragma once
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <time.h>

typedef struct CanadianHydrograpicTidesHiLoData {
    bool isMetric = true;
    char eventDate[24] = "Unknown";
    float value = 0.0;
    char timeSeriesId[26] = "";
} CanadianHydrograpicTidesHiLoData;

class CanadianHydrograpicTidesHiLo : public JsonListener {
  private:
    const char host[25] = "api-sine.dfo-mpo.gc.ca";
    const char TimeSeriesCode[10] = "wlp-hilo";
    const uint16_t port = 443;
    String currentKey;
    String currentParent;
    CanadianHydrograpicTidesHiLoData *data;
    uint8_t maxTides;
    uint8_t currentTide;

    bool doUpdate(CanadianHydrograpicTidesHiLoData *data, String path);
    String buildPath(String stationId, String startTime, String endTime);

  public:
    CanadianHydrograpicTidesHiLo();
    bool updateTides(CanadianHydrograpicTidesHiLoData *data, String stationId, String startTime, String endTime, uint8_t maxTides = 6);
    bool updateTidesById(CanadianHydrograpicTidesHiLoData *data, String stationId, String startTime, String endTime, uint8_t maxTides = 6);
    
    virtual void whitespace(char c);
    virtual void startDocument();
    virtual void key(String key);
    virtual void value(String value);
    virtual void endArray();
    virtual void endObject();
    virtual void endDocument();
    virtual void startArray();
    virtual void startObject();
};