// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54

#include <Arduino.h>
#include <WiFiEsp.h>

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

#define NAME_OF_SSID "SHAW-74C6"
#define PASSWORD_OF_SSID "cellar0933beard"

void printConnectInfo();

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  Serial3.begin(115200);
  WiFi.init(&Serial3);

  if (WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  WiFi.begin(NAME_OF_SSID, PASSWORD_OF_SSID);
  Serial.println("Waiting for connection to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print('.');
  }
  Serial.println();

  printConnectInfo();
}

void loop()
{
}

void printConnectInfo()
{
  // get your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  
  // print MAC address
  char buf[20];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.print("MAC address: ");
  Serial.println(buf);
  Serial.print("IP address: ");
  Serial.println(ip);
}