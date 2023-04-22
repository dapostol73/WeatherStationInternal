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
#include "OpenWeatherMapForecast.h"

OpenWeatherMapForecast::OpenWeatherMapForecast() {
}

bool OpenWeatherMapForecast::updateForecasts(OpenWeatherMapForecastData *data, String appId, String location, uint8_t maxForecasts) {
  this->maxForecasts = maxForecasts;
  return doUpdate(data, buildPath(appId, "q=" + location));
}

bool OpenWeatherMapForecast::updateForecastsById(OpenWeatherMapForecastData *data, String appId, String locationId, uint8_t maxForecasts) {
  this->maxForecasts = maxForecasts;
  return doUpdate(data, buildPath(appId, "id=" + locationId));
}

String OpenWeatherMapForecast::buildPath(String appId, String locationParameter) {
  String units = metric ? "metric" : "imperial";
  return "/data/2.5/forecast?" + locationParameter + "&appid=" + appId + "&units=" + units + "&lang=" + language + "&cnt=" + String(this->maxForecasts*8);
}

bool OpenWeatherMapForecast::doUpdate(OpenWeatherMapForecastData *data, String path) {
  bool success = true;
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->weatherItemCounter = 0;
  this->currentForecast = 0;
  this->data = data;
  this->data->isMetric = isMetric();
  JsonStreamingParser parser;
  parser.setListener(this);
  char connectInfo[256] = "";
	sprintf(connectInfo, "[HTTP] Requesting resource at http://%s:%u%s\n", host.c_str(), port, path.c_str());
	Serial.println(connectInfo);

  WiFiClient client;
  if(client.connect(host.c_str(), port)) {
    bool isBody = false;
    uint8_t eof = 0;
    char c;
    Serial.println("[HTTP] connected, now GETting data");
    // TODO: Figure out why Connection: close truncates client.read()
    client.println("GET " + path + " HTTP/1.1");
    client.println("Host: " + host);
    client.println("Connection: keep-alive");
    client.println();
    
    while (client.connected() || client.available()) {
      if ((millis() - lost_do) > lostTest) {
        Serial.println("[HTTP] lost in client with a timeout");
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
    Serial.println("[HTTP] failed to connect to host");
    success = false;
  }
  this->data = nullptr;
  return success;
}

String OpenWeatherMapForecast::toPascalCase(String value) {
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

void OpenWeatherMapForecast::whitespace(char c) {
  Serial.println("whitespace");
}

void OpenWeatherMapForecast::startDocument() {
  Serial.println("start of document");
}

void OpenWeatherMapForecast::key(String key) {
  currentKey = String(key);
}

void OpenWeatherMapForecast::value(String value) {
  if (currentForecast >= maxForecasts) {
    return;
  }
  // {"dt":1527066000,  uint32_t observationTime;
  if (currentKey == "dt") {
    data[currentForecast].observationTime = value.toInt();

    if (allowedHoursCount > 0) {
      time_t time = data[currentForecast].observationTime;
      struct tm* timeInfo;
      timeInfo = gmtime(&time);
      uint8_t currentHour = timeInfo->tm_hour;
      for (uint8_t i = 0; i < allowedHoursCount; i++) {
        if (currentHour == allowedHours[i]) {
          isCurrentForecastAllowed = true;
          return;
        }
      }
      isCurrentForecastAllowed = false;
      return;
    }
  }
  if (!isCurrentForecastAllowed) {
    return;
  }
  // "main":{
  //   "temp":17.35, float temp;
  if (currentKey == "temp") {
    data[currentForecast].temp = value.toFloat();
    // initialize potentially empty values:
    //data[currentForecast].rain = 0;;
  }
  //   "pressure":970.8,float pressure;
  if (currentKey == "pressure") {
    data[currentForecast].pressure = value.toFloat();
  }
  //   "":97,uint8_t humidity;
  if (currentKey == "humidity") {
    data[currentForecast].humidity = value.toFloat();
  }
  //   "temp_kf":0.46
  // },"weather":[{

  if (currentParent == "weather") {
    //   "id":802,uint16_t weatherId;
    if (currentKey == "id") {
      data[currentForecast].weatherId = value.toInt();
    }

    //   "main":"Clouds",String main;
    if (currentKey == "main") {
      data[currentForecast].main = value;
    }
    //   "description":"scattered clouds",String description;
    if (currentKey == "description") {
      data[currentForecast].description = toPascalCase(value);
    }
    //   "icon":"03d" String icon; String iconMeteoCon;
    if (currentKey == "icon") {
      data[currentForecast].icon = value;
    }
  }
  /*
  //   "feels_like": 16.99, float feelsLike;
  if (currentKey == "feels_like") {
    data[currentForecast].feelsLike = value.toFloat();
  }
  //   "temp_min":16.89, float tempMin;
  if (currentKey == "temp_min") {
    data[currentForecast].tempMin = value.toFloat();
  }
  //   "temp_max":17.35,float tempMax;
  if (currentKey == "temp_max") {
    data[currentForecast].tempMax = value.toFloat();
  }
  //   "sea_level":1030.62,float pressureSeaLevel;
  if (currentKey == "sea_level") {
    data[currentForecast].pressureSeaLevel = value.toFloat();
  }
  //   "grnd_level":970.8,float pressureGroundLevel;
  if (currentKey == "grnd_level") {
    data[currentForecast].pressureGroundLevel = value.toFloat();
  }
  // }],"clouds":{"all":44},uint8_t clouds;
  if (currentKey == "all") {
    data[currentForecast].clouds = value.toInt();
  }
  // "wind":{
  //   "speed":1.77, float windSpeed;
  if (currentKey == "speed") {
    data[currentForecast].windSpeed = value.toFloat();
  }
  //   "deg":207.501 float windDeg;
  if (currentKey == "deg") {
    data[currentForecast].windDeg = value.toFloat();
  }
  // rain: {3h: 0.055}, float rain;
  if (currentKey == "3h") {
    data[currentForecast].rain = value.toFloat();
  }
  */
  // },"sys":{"pod":"d"}
  // dt_txt: "2018-05-23 09:00:00"   String observationTimeText;
  if (currentKey == "dt_txt") {
    data[currentForecast].observationTimeText = value;
    // this is not super save, if there is no dt_txt item we'll never get all forecasts;
    currentForecast++;
  }
}

void OpenWeatherMapForecast::endArray() {

}

void OpenWeatherMapForecast::startObject() {
  currentParent = currentKey;
}

void OpenWeatherMapForecast::endObject() {
  if (currentParent == "weather") {
    weatherItemCounter++;
  }
  currentParent = "";
}

void OpenWeatherMapForecast::endDocument() {
  Serial.println("end of document");
}

void OpenWeatherMapForecast::startArray() {

}
