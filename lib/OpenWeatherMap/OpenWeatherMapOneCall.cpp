/**The MIT License (MIT)
 
 Copyright (c) 2020 by Chris Klinger
 
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
#include "OpenWeatherMapOneCall.h"

String PATH_SEPERATOR = "/";

OpenWeatherMapOneCall::OpenWeatherMapOneCall() {
}

bool OpenWeatherMapOneCall::update(OpenWeatherMapOneCallData *data, String appId, float lat, float lon) {
  return doUpdate(data, buildPath(appId, lat, lon));
}

String OpenWeatherMapOneCall::buildPath(String appId, float lat, float lon) {
  String units = metric ? "metric" : "imperial";
  return "/data/2.5/onecall?appid=" + appId + "&lat=" + lat + "&lon=" + lon + "&units=" + units + "&lang=" + language;
}

bool OpenWeatherMapOneCall::doUpdate(OpenWeatherMapOneCallData *data, String path) {
  bool success = true;
  unsigned long lostTest = 10000UL;
  unsigned long lost_do = millis();
  this->weatherItemCounter = 0;
  this->hourlyItemCounter = 0;
  this->dailyItemCounter = 0;
  this->data = data;
  this->data->isMetric = isMetric();
  JsonStreamingParser parser;
  parser.setListener(this);
  char connectInfo[256] = "";
	sprintf(connectInfo, "[HTTP] Requesting resource at http://%s:%u%s\n", host.c_str(), port, path.c_str());
	Serial.println(connectInfo);

  WiFiClient client;
  if (client.connect(host.c_str(), port)) {
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

String OpenWeatherMapOneCall::toPascalCase(String value) {
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

void OpenWeatherMapOneCall::whitespace(char c) {
  Serial.println("whitespace");
}

void OpenWeatherMapOneCall::startDocument() {
  Serial.println("start of document");
}

void OpenWeatherMapOneCall::key(String key) {
  currentKey = key;
}

void OpenWeatherMapOneCall::value(String value) {
  // "lon": 8.54, float lon;
  if (currentKey == "lon") {
    this->data->lon = value.toFloat();
  }
  // "lat": 47.37 float lat;
  if (currentKey == "lat") {
    this->data->lat = value.toFloat();
  }
  // "timezone": "Europe/Berlin"
  if (currentKey == "timezone") {
    this->data->timezone = value;
  }

  // "current": {..}
  if (currentParent.startsWith("/ROOT/current")) {
    // "current": {.. "dt": 1587216739, .. }
    if (currentKey == "dt") {
      this->data->current.dt = value.toInt();
    }
    if (currentKey == "sunrise") {
      this->data->current.sunrise = value.toInt();
    }
    if (currentKey == "sunset") {
      this->data->current.sunset = value.toInt();
    }
    if (currentKey == "temp") {
      this->data->current.temp = value.toFloat();
    }
    if (currentKey == "feels_like") {
      this->data->current.feels_like = value.toFloat();
    }
    if (currentKey == "pressure") {
      this->data->current.pressure = value.toFloat();
    }
    if (currentKey == "humidity") {
      this->data->current.humidity = value.toFloat();
    }
    if (currentKey == "dew_point") {
      this->data->current.dew_point = value.toFloat();
    }
    if (currentKey == "uvi") {
      this->data->current.uvi = value.toFloat();
    }
    if (currentKey == "clouds") {
      this->data->current.clouds = value.toInt();
    }
    if (currentKey == "visibility") {
      this->data->current.visibility = value.toInt();
    }
    if (currentKey == "wind_speed") {
      this->data->current.windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg") {
      this->data->current.windDeg = value.toFloat();
    }
    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/current/weather[]") && weatherItemCounter == 0) {
      if (currentKey == "id") {
        this->data->current.weatherId = value.toInt();
      }
      if (currentKey == "main") {
        this->data->current.weatherMain = value;
      }
      if (currentKey == "description") {
        this->data->current.weatherDescription = toPascalCase(value);
      }
      if (currentKey == "icon") {
        this->data->current.weatherIcon = value;
      }
    }
  }

  // "hourly": [..]
  if(currentParent.startsWith("/ROOT/hourly[]")) {
    if (currentKey == "dt") {
      this->data->hourly[hourlyItemCounter].dt = value.toInt();
    }
    if (currentKey == "temp") {
      this->data->hourly[hourlyItemCounter].temp = value.toFloat();
    }
    if (currentKey == "feels_like") {
      this->data->hourly[hourlyItemCounter].feels_like = value.toFloat();
    }
    if (currentKey == "pressure") {
      this->data->hourly[hourlyItemCounter].pressure = value.toInt();
    }
    if (currentKey == "humidity") {
      this->data->hourly[hourlyItemCounter].humidity = value.toInt();
    }
    if (currentKey == "dew_point") {
      this->data->hourly[hourlyItemCounter].dew_point = value.toFloat();
    }
    if (currentKey == "clouds") {
      this->data->hourly[hourlyItemCounter].clouds = value.toInt();
    }
    if (currentKey == "wind_speed") {
      this->data->hourly[hourlyItemCounter].windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg") {
      this->data->hourly[hourlyItemCounter].windDeg = value.toFloat();
    }

    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/hourly[]/_obj/weather[]") && weatherItemCounter == 0) {
      if (currentKey == "id") {
        this->data->hourly[hourlyItemCounter].weatherId = value.toInt();
      }
      if (currentKey == "main") {
        this->data->hourly[hourlyItemCounter].weatherMain = value;
      }
      if (currentKey == "description") {
        this->data->hourly[hourlyItemCounter].weatherDescription = value;
      }
      if (currentKey == "icon") {
        this->data->hourly[hourlyItemCounter].weatherIcon = value;
      }
    }
  }

  // "daily": [..]
  if(currentParent.startsWith("/ROOT/daily[]")) {
    if (currentKey == "dt") {
      this->data->daily[dailyItemCounter].dt = value.toInt();
    }
    if (currentKey == "sunrise") {
      this->data->daily[dailyItemCounter].sunrise = value.toInt();
    }
    if (currentKey == "sunset") {
      this->data->daily[dailyItemCounter].sunset = value.toInt();
    }
    if (currentKey == "pressure") {
      this->data->daily[dailyItemCounter].pressure = value.toInt();
    }
    if (currentKey == "humidity") {
      this->data->daily[dailyItemCounter].humidity = value.toInt();
    }
    if (currentKey == "dew_point") {
      this->data->daily[dailyItemCounter].dew_point = value.toFloat();
    }
    if (currentKey == "wind_speed") {
      this->data->daily[dailyItemCounter].windSpeed = value.toFloat();
    }
    if (currentKey == "wind_deg") {
      this->data->daily[dailyItemCounter].windDeg = value.toFloat();
    }
    if (currentKey == "clouds") {
      this->data->daily[dailyItemCounter].clouds = value.toInt();
    }
    if (currentKey == "rain") {
      this->data->daily[dailyItemCounter].rain = value.toFloat();
    }
    if (currentKey == "snow") {
      this->data->daily[dailyItemCounter].snow = value.toFloat();
    }
    if (currentKey == "uvi") {
      this->data->daily[dailyItemCounter].uvi = value.toFloat();
    }

    if (currentParent.startsWith("/ROOT/daily[]/_obj/temp")) {
      if (currentKey == "day") {
        this->data->daily[dailyItemCounter].tempDay = value.toFloat();
      }
      if (currentKey == "min") {
        this->data->daily[dailyItemCounter].tempMin = value.toFloat();
      }
      if (currentKey == "max") {
        this->data->daily[dailyItemCounter].tempMax = value.toFloat();
      }
      if (currentKey == "night") {
        this->data->daily[dailyItemCounter].tempNight = value.toFloat();
      }
      if (currentKey == "eve") {
        this->data->daily[dailyItemCounter].tempEve = value.toFloat();
      }
      if (currentKey == "morn") {
        this->data->daily[dailyItemCounter].tempMorn = value.toFloat();
      }
    }
  
    if (currentParent.startsWith("/ROOT/daily[]/_obj/feels_like")) {
      if (currentKey == "day") {
        this->data->daily[dailyItemCounter].feels_likeDay = value.toFloat();
      }
      if (currentKey == "night") {
        this->data->daily[dailyItemCounter].feels_likeNight = value.toFloat();
      }
      if (currentKey == "eve") {
        this->data->daily[dailyItemCounter].feels_likeEve = value.toFloat();
      }
      if (currentKey == "morn") {
        this->data->daily[dailyItemCounter].feels_likeMorn = value.toFloat();
      }
    }
    // weatherItemCounter: only get the first item if more than one is available
    if (currentParent.startsWith("/ROOT/daily[]/_obj/weather[]") && weatherItemCounter == 0) {
      if (currentKey == "id") {
        this->data->daily[dailyItemCounter].weatherId = value.toInt();
      }
      if (currentKey == "main") {
        this->data->daily[dailyItemCounter].weatherMain = value;
      }
      if (currentKey == "description") {
        this->data->daily[dailyItemCounter].weatherDescription = value;
      }
      if (currentKey == "icon") {
        this->data->daily[dailyItemCounter].weatherIcon = value;
      }
    }
  }
}

void OpenWeatherMapOneCall::endArray() {
  currentKey = "";
  currentParent= currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR));
}

void OpenWeatherMapOneCall::startObject() {
  if(currentKey == "") {
    currentKey = "_obj";
  }
  currentParent += PATH_SEPERATOR + currentKey;
}

void OpenWeatherMapOneCall::endObject() {
  if (currentParent == "/ROOT/current/weather[]/_obj" || currentParent == "/ROOT/daily[]/_obj/weather[]/_obj" || currentParent == "/ROOT/daily[]/_obj/weather[]/_obj"  ) {
    weatherItemCounter++;
  }
  if (currentParent == "/ROOT/hourly[]/_obj") {
    hourlyItemCounter++;
  }
  if (currentParent == "/ROOT/daily[]/_obj") {
    dailyItemCounter++;
  }  
  currentKey = "";
  currentParent= currentParent.substring(0, currentParent.lastIndexOf(PATH_SEPERATOR));
}

void OpenWeatherMapOneCall::endDocument() {
  Serial.println("end of document");
}

void OpenWeatherMapOneCall::startArray() {
  weatherItemCounter = 0;
  
  currentParent += PATH_SEPERATOR + currentKey + "[]";
  currentKey = "";
}
