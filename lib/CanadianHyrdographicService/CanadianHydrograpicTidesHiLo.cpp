/**The MIT License (MIT)
 
 Copyright (c) 2018 by ThingPulse Ltd., https://thingpulse.com
 
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


#include <WiFiEspAT.h>
#include <WiFiClient.h>
#include "CanadianHydrograpicTidesHiLo.h"

CanadianHydrograpicTidesHiLo::CanadianHydrograpicTidesHiLo() {
}

bool CanadianHydrograpicTidesHiLo::updateTides(CanadianHydrograpicTidesHiLoData *data, String stationId, String startTime, String endTime, uint8_t maxTides) {
  this->maxTides = max(maxTides, 1);
  return doUpdate(data, buildPath(stationId, startTime, endTime));
}

bool CanadianHydrograpicTidesHiLo::updateTidesById(CanadianHydrograpicTidesHiLoData *data, String stationId, String startTime, String endTime, uint8_t maxTides) {
  this->maxTides = max(maxTides, 1);
  return doUpdate(data, buildPath(stationId, startTime, endTime));
}

/// @brief example /api/v1/stations/5cebf1e33d0f4a073c4bc2a7/data?time-series-code=wlp-hilo&from=2025-12-27T12%3A00%3A00Z&to=2025-12-29T00%3A00%3A00Z&resolution=ALL
/// @param stationId 
/// @param startTime 
/// @param endTime 
/// @return 
String CanadianHydrograpicTidesHiLo::buildPath(String stationId, String startTime, String endTime) {
  char path[128] = "";
  sprintf(path,
          "/api/v1/stations/%s/data?time-series-code=%s&from=%s&to=%s",
          stationId.c_str(),
          TimeSeriesCode,
          startTime.c_str(),
          endTime.c_str());
  return path;
}

bool CanadianHydrograpicTidesHiLo::doUpdate(CanadianHydrograpicTidesHiLoData *data, String path) {
  bool success = true;
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->currentTide = 0;
  this->data = data;
  JsonStreamingParser parser;
  parser.setListener(this);
  char connectInfo[232] = "";
  sprintf(connectInfo, "[HTTPS] Requesting resource at https://%s:%u%s\n", host, port, path.c_str());
	Serial.println(connectInfo);

  WiFiClient client;
  if(client.connectSSL(host, port)) {
    bool isBody = false;
    uint8_t eof = 0;
    char c;
    Serial.println(F("[HTTPS] connected, now GETting data"));
    // TODO: Figure out why Connection: close truncates client.read()
    client.print(F("GET "));
    client.print(path);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(host);
    client.println(F("Connection: keep-alive"));
    client.println();
    
    while (client.connected() || client.available()) {
      if ((millis() - lost_do) > lostTest) {
        Serial.println(F("[HTTPS] lost in client with a timeout"));
        success = false;
        break;
      }
      if (client.available() == 0 && eof < 100) {
        eof++;
        delay(25);
        if (eof >= 100) break;//HACK: assume if you get 100+ zero, we are done
        continue;
      }
      c = client.read();
      //Serial.write(c);
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
      // give WiFi and TCP/IP libraries a chance to handle pending events
      yield();
    }
    
    client.stop();
    client.flush();
    parser.reset();
  } else {
    Serial.println(F("[HTTPS] failed to connect to host"));
    success = false;
  }
  this->data = nullptr;
  return success;
}

void CanadianHydrograpicTidesHiLo::whitespace(char c) {
  Serial.println(F("whitespace"));
}

void CanadianHydrograpicTidesHiLo::startDocument() {
  Serial.println(F("start of document"));
}

void CanadianHydrograpicTidesHiLo::key(String key) {
  currentKey = String(key);
}

void CanadianHydrograpicTidesHiLo::value(String value) {
  if (currentTide >= maxTides) {
    return;
  }
  // { "eventDate": "string",
  if (currentKey == F("eventDate")) {
    strcpy(data[currentTide].eventDate, value.c_str());
  }
  // "qcFlagCode": "1",
  // "value": 0.1,
  if (currentKey == F("value")) {
    data[currentTide].value = value.toFloat();
  }
  // "uncertainty": 0.1,
  // "timeSeriesId": "string",
  if (currentKey == F("timeSeriesId")) {
    strcpy(data[currentTide].timeSeriesId, value.c_str());
    // this is not super safe, if there is no timeSeriesId item we'll never get all tides;
    currentTide++;
  }
  // "reviewed": true,
  // "qualifier": "SLACK" }
}

void CanadianHydrograpicTidesHiLo::endArray() {

}

void CanadianHydrograpicTidesHiLo::startObject() {
  currentParent = currentKey;
}

void CanadianHydrograpicTidesHiLo::endObject() {
  currentParent = "";
}

void CanadianHydrograpicTidesHiLo::endDocument() {
  Serial.println(F("end of document"));
}

void CanadianHydrograpicTidesHiLo::startArray() {

}
