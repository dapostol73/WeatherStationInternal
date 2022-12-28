// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796

#include <Arduino.h>
#include <WiFiEsp.h>
#include <SPI.h>
#include <Wire.h>

#define USE_8BIT_SHIELD_ON_MEGA 0

#include "DisplayControl.h"

#define OFFICE

#ifdef WHISTLER
#define NAME_OF_SSID "SHAW-74C6"
#define PASSWORD_OF_SSID "cellar0933beard"
#endif

#ifdef OFFICE
#define NAME_OF_SSID "a12studiosOG"
#define PASSWORD_OF_SSID "apollo1973"
#endif

#ifdef HOME
#define NAME_OF_SSID "a12studiosinc"
#define PASSWORD_OF_SSID "apollo1973"
#endif

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

DisplayControl displayControl;

void printConnectInfo();

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  displayControl.initialize();
  displayControl.fillScreen(BLACK);

  char intialMsg[] = "Intializing Wifi module.";
  Serial.println(intialMsg);
  displayControl.printLine(intialMsg);
  Serial3.begin(115200);
  WiFi.init(&Serial3);

  if (WiFi.status() == WL_NO_SHIELD)
  {
    char initialErr[] = "Communication with WiFi module failed!";
    Serial.println(initialErr);
    displayControl.printLine(initialErr, RED);
    // don't continue
    while (true)
      ;
  }

  char infoMsg[] = "Waiting for connection to WiFi";
  WiFi.begin(NAME_OF_SSID, PASSWORD_OF_SSID);
  Serial.println(infoMsg);
  displayControl.printLine(infoMsg, CYAN);

  int timeout = 0;
  int timeoutMax = 30;
  while (WiFi.status() != WL_CONNECTED && timeout < timeoutMax)
  {
    delay(1000);
    Serial.print('.');
    displayControl.print(".", CYAN);
    ++timeout;
  }
  Serial.println();
  displayControl.printLine();
  Serial.println(WiFi.status());

  if (WiFi.status() == WL_DISCONNECTED)
  {
    char connectErr[48] = "";
    sprintf(connectErr, "Wifi failed to connect to %s acces point!", NAME_OF_SSID);
    Serial.println(connectErr);
    displayControl.printLine(connectErr, BLACK, RED);
    // don't continue
    while (true)
      ;
  }

  printConnectInfo();
  displayControl.setRotation(1);
  //showMainMenu();
}

void loop()
{
  //testDisplay();
}

void printConnectInfo()
{
  // get your MAC address
  byte mac[6];
  WiFi.macAddress(mac);
  IPAddress ip = WiFi.localIP();
  
  // print MAC address
  char wifiInfo[34] = "";
  sprintf(wifiInfo, "Wifi: %s", NAME_OF_SSID);
  Serial.println(wifiInfo);
  displayControl.printLine(wifiInfo, GREEN);

  // print MAC address
  char macInfo[34] = "";
  sprintf(macInfo, "MAC address: %02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  Serial.println(macInfo);
  displayControl.printLine(macInfo, YELLOW);

  // print IP address
  char ipInfo[34] = "";
  Serial.println(ipInfo);
  sprintf(ipInfo, "IP address: %u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
  displayControl.printLine(ipInfo, YELLOW);
}