// Firmware for ESP8266
// https://github.com/Edragon/esp_firmware/tree/master/Firmware/AT-other/AI-THINKER/At_firmware_bin1.54
// Libraries for Screen
// http://www.lcdwiki.com/3.95inch_Arduino_Display-Mega2560_ST7796

#include <Arduino.h>
#include <WiFiEsp.h>
#include <SPI.h>
#include <Wire.h>

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <LCDWIKI_TOUCH.h> //touch screen library

#ifdef HAVE_SERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7) //RX, TX
#endif

#define NAME_OF_SSID "SHAW-74C6"
#define PASSWORD_OF_SSID "cellar0933beard"

LCDWIKI_KBV mylcd(ILI9488,40,38,39,43,41); //model,cs,cd,wr,rd,reset
LCDWIKI_TOUCH mytouch(53,52,50,51,44); //tcs,tclk,tdout,tdin,tirq
//LCDWIKI_KBV mylcd(ST7796S,A3,A2,A1,A0,A4);
//LCDWIKI_KBV mylcd(ST7796S,40,38,39,44,41);
//LCDWIKI_KBV mylcd(320,480,A3,A2,A1,A0,A4);
//LCDWIKI_KBV mylcd(320,480,40,38,39,44,41);

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define COLORBOXSIZE mylcd.Get_Display_Width()/6
#define PENBOXSIZE mylcd.Get_Display_Width()/4

void showMainMenu();
void testDisplay();
void printConnectInfo();

void setup()
{

  Serial.begin(115200);
  while (!Serial)
    ;

  mylcd.Init_LCD(); //initialize lcd 
  testDisplay();

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
  showMainMenu();
}

void loop()
{
  //testDisplay();
}

void showString(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    mylcd.Set_Text_Mode(mode);
    mylcd.Set_Text_Size(csize);
    mylcd.Set_Text_colour(fc);
    mylcd.Set_Text_Back_colour(bc);
    mylcd.Print_String(str,x,y);
}

void showMainMenu(void)
{
   mylcd.Set_Draw_color(YELLOW);
   mylcd.Fill_Round_Rectangle(5, 0, (mylcd.Get_Display_Width()-20)/3+5,COLORBOXSIZE/2+20, 5);
   mylcd.Fill_Round_Rectangle((mylcd.Get_Display_Width()-20)/3*2+15, 0, (mylcd.Get_Display_Width()-20)/3*3+15,COLORBOXSIZE/2+20, 5);
   mylcd.Set_Draw_color(MAGENTA);
   mylcd.Fill_Round_Rectangle((mylcd.Get_Display_Width()-20)/3+10, 0, (mylcd.Get_Display_Width()-20)/3*2+10,COLORBOXSIZE/2+20, 5);
   showString("COLOUR",5+((mylcd.Get_Display_Width()-20)/3-72)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
   showString("CLEAR",(mylcd.Get_Display_Width()-20)/3+10+((mylcd.Get_Display_Width()-20)/3-60)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,WHITE, BLACK,1);
   showString("PENSIZE",(mylcd.Get_Display_Width()-20)/3*2+15+((mylcd.Get_Display_Width()-20)/3-84)/2-1,((COLORBOXSIZE/2+20)-16)/2,2,BLUE, BLACK,1);
 }

void testDisplay()
{
  //Sequential display black,white,red,green,blue
  mylcd.Fill_Screen(0,0,0);  
  mylcd.Fill_Screen(255,255,255); 
  mylcd.Fill_Screen(255,0,0); 
  mylcd.Fill_Screen(0,255,0);
  mylcd.Fill_Screen(0,0,255);
  delay(3000);
  mylcd.Fill_Screen(0x0000);
  delay(1000);
  mylcd.Fill_Screen(0xFFFF);
  delay(1000);
  mylcd.Fill_Screen(0xF800);
  delay(1000);
  mylcd.Fill_Screen(0x07E0);
  delay(1000);
  mylcd.Fill_Screen(0x001F);
  delay(1000);
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