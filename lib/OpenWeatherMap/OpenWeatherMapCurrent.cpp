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
#include "OpenWeatherMapCurrent.h"

OpenWeatherMapCurrent::OpenWeatherMapCurrent() {
}

bool OpenWeatherMapCurrent::updateCurrent(OpenWeatherMapCurrentData *data, String appId, String location) {
  return doUpdate(data, buildPath(appId, "q=" + location));
}

bool OpenWeatherMapCurrent::updateCurrentById(OpenWeatherMapCurrentData *data, String appId, String locationId) {
  return doUpdate(data, buildPath(appId, "id=" + locationId));
}

String OpenWeatherMapCurrent::buildPath(String appId, String locationParameter) {
  char path[128] = "";
  sprintf(path, 
          "/data/2.5/weather?%s&appid=%s&units=%s&lang=%s",
          locationParameter.c_str(),
          appId.c_str(),
          metric ? "metric" : "imperial",
          language.c_str());
  return path;
  //return "/data/2.5/weather?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language;
}

bool OpenWeatherMapCurrent::doUpdate(OpenWeatherMapCurrentData *data, String path) {
  bool success = true;
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->weatherItemCounter = 0;
  this->data = data;
  this->data->isMetric = isMetric();
  JsonStreamingParser parser;
  parser.setListener(this);
  char connectInfo[196] = "";
  sprintf(connectInfo, "[HTTP] Requesting resource at http://%s:%u%s\n", host, port, path.c_str());
  Serial.println(connectInfo);

  WiFiClient client;
  if(client.connect(host, port)) {
    bool isBody = false;
    uint8_t eof = 0;
    char c;
    Serial.println(F("[HTTP] connected, now GETting data"));
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
        Serial.println(F("[HTTP] lost in client with a timeout"));
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
    Serial.println(F("[HTTP] failed to connect to host"));
    success = false;
  }
  this->data = nullptr;
  return success;
}

String OpenWeatherMapCurrent::toPascalCase(String value) {
  bool upper = true;
  char c;
	for (uint16_t i = 0; i < value.length(); i++) {
    if (upper) {
		  c = toupper(value.charAt(i));
      value.setCharAt(i, c);
    }
    upper = false;
    if (value.charAt(i) == ' ')
      upper = true;
	}
  return value;
}

void OpenWeatherMapCurrent::whitespace(char c) {
  Serial.println(F("whitespace"));
}

void OpenWeatherMapCurrent::startDocument() {
  Serial.println(F("start of document"));
}

void OpenWeatherMapCurrent::key(String key) {
  currentKey = String(key);
}

void OpenWeatherMapCurrent::value(String value) {
  // "lon": 8.54, float lon;
  if (currentKey == F("lon")) {
    this->data->lon = value.toFloat();
  }
  // "lat": 47.37 float lat;
  if (currentKey == F("lat")) {
    this->data->lat = value.toFloat();
  }
  // weatherItemCounter: only get the first item if more than one is available
  if (currentParent == F("weather") && weatherItemCounter == 0) {
    // "id": 521, weatherId weatherId;
    if (currentKey == F("id")) {
      this->data->weatherId = value.toInt();
    }
    // "main": "Rain", String main;
    if (currentKey == F("main")) {
      strcpy(this->data->main, value.c_str());
    }
    // "description": "shower rain", String description;
    if (currentKey == F("description")) {
      strcpy(this->data->description, toPascalCase(value).c_str());
    }
    // "icon": "09d" String icon;
   //String iconMeteoCon;
    if (currentKey == F("icon")) {
      strcpy(this->data->icon, value.c_str());
    }

  }

  // "temp": 290.56, float temp;
  if (currentKey == F("temp")) {
    this->data->temp = value.toFloat();
  }
  // "pressure": 1013, uint16_t pressure;
  if (currentKey == F("pressure")) {
    this->data->pressure = value.toFloat();
  }
  // "humidity": 87, uint8_t humidity;
  if (currentKey == F("humidity")) {
    this->data->humidity = value.toFloat();
  }
  // "temp_min": 289.15, float tempMin;
  if (currentKey == F("temp_min")) {
    this->data->tempMin = value.toFloat();
  }
  // "temp_max": 292.15 float tempMax;
  if (currentKey == F("temp_max")) {
    this->data->tempMax = value.toFloat();
  }
  // visibility: 10000, uint16_t visibility;
  if (currentKey == F("visibility")) {
    this->data->visibility = value.toInt();
  }
  // "wind": {"speed": 1.5}, float windSpeed;
  if (currentKey == F("speed")) {
    this->data->windSpeed = value.toFloat();
  }
  // "wind": {deg: 226.505}, float windDeg;
  if (currentKey == F("deg")) {
    this->data->windDeg = value.toFloat();
  }
  // "clouds": {"all": 90}, uint8_t clouds;
  if (currentKey == F("all")) {
    this->data->clouds = value.toInt();
  }
  // "dt": 1527015000, uint64_t observationTime;
  if (currentKey == F("dt")) {
    this->data->observationTime = value.toInt();
  }
  // "country": "CH", String country;
  if (currentKey == F("country")) {
    strcpy(this->data->country, value.c_str());
  }
  // "sunrise": 1526960448, uint32_t sunrise;
  if (currentKey == F("sunrise")) {
    this->data->sunrise = value.toInt();
  }
  // "sunset": 1527015901 uint32_t sunset;
  if (currentKey == F("sunset")) {
    this->data->sunset = value.toInt();
  }
  // "name": "Zurich", String cityName;
  if (currentKey == F("name")) {
    strcpy(this->data->cityName, value.c_str());
  }
}

void OpenWeatherMapCurrent::endArray() {

}


void OpenWeatherMapCurrent::startObject() {
  currentParent = currentKey;
}

void OpenWeatherMapCurrent::endObject() {
  if (currentParent == F("weather")) {
    weatherItemCounter++;
  }
  currentParent = "";
}

void OpenWeatherMapCurrent::endDocument() {
  Serial.println(F("end of document"));
}

void OpenWeatherMapCurrent::startArray() {

}
