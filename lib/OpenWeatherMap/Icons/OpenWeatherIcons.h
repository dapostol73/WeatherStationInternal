#ifndef _OPEN_WEATHER_ICONS_
#define _OPEN_WEATHER_ICONS_

// Generate Icons using https://www.mischianti.org/images-to-byte-array-online-converter-cpp-arduino/
// Using Horizantal - 2 bytes per pixel
#include "01d.h"
#include "02d.h"
#include "03d.h"
#include "04d.h"
#include "09d.h"
#include "10d.h"
#include "11d.h"
#include "13d.h"
#include "50d.h"

// Helper function, should be part of the weather station library and should disappear soon
const OpenWeatherIcon getOpenWeatherIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return icon_01d_2x;
  if (iconText == "02d" || iconText == "02n") return icon_02d_2x;
  if (iconText == "03d" || iconText == "03n") return icon_03d_2x;
  if (iconText == "04d" || iconText == "04n") return icon_04d_2x;
  if (iconText == "09d" || iconText == "09n") return icon_09d_2x;
  if (iconText == "10d" || iconText == "10n") return icon_10d_2x;
  if (iconText == "11d" || iconText == "11n") return icon_11d_2x;
  if (iconText == "13d" || iconText == "13n") return icon_13d_2x;
  if (iconText == "50d" || iconText == "50n") return icon_50d_2x;
  return icon_01d_2x;
}

const OpenWeatherIcon getOpenWeatherMiniIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return icon_01d_1x;
  if (iconText == "02d" || iconText == "02n") return icon_02d_1x;
  if (iconText == "03d" || iconText == "03n") return icon_03d_1x;
  if (iconText == "04d" || iconText == "04n") return icon_04d_1x;
  if (iconText == "09d" || iconText == "09n") return icon_09d_1x;
  if (iconText == "10d" || iconText == "10n") return icon_10d_1x;
  if (iconText == "11d" || iconText == "11n") return icon_11d_1x;
  if (iconText == "13d" || iconText == "13n") return icon_13d_1x;
  if (iconText == "50d" || iconText == "50n") return icon_50d_1x;
  return icon_01d_1x;
}

const OpenWeatherIcon getOpenWeatherCropIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return icon_01d_crop;
  if (iconText == "02d" || iconText == "02n") return icon_02d_crop;
  if (iconText == "03d" || iconText == "03n") return icon_03d_crop;
  if (iconText == "04d" || iconText == "04n") return icon_04d_crop;
  if (iconText == "09d" || iconText == "09n") return icon_09d_crop;
  if (iconText == "10d" || iconText == "10n") return icon_10d_crop;
  if (iconText == "11d" || iconText == "11n") return icon_11d_crop;
  if (iconText == "13d" || iconText == "13n") return icon_13d_crop;
  if (iconText == "50d" || iconText == "50n") return icon_50d_crop;
  return icon_01d_crop;
}

// Helper function, should be part of the weather station library and should disappear soon
const unsigned char* getOpenWeatherPaletteIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return palette_01d_2x;
  if (iconText == "02d" || iconText == "02n") return palette_02d_2x;
  if (iconText == "03d" || iconText == "03n") return palette_03d_2x;
  if (iconText == "04d" || iconText == "04n") return palette_04d_2x;
  if (iconText == "09d" || iconText == "09n") return palette_09d_2x;
  if (iconText == "10d" || iconText == "10n") return palette_10d_2x;
  if (iconText == "11d" || iconText == "11n") return palette_11d_2x;
  if (iconText == "13d" || iconText == "13n") return palette_13d_2x;
  if (iconText == "50d" || iconText == "50n") return palette_50d_2x;
  return palette_01d_2x;
}

const unsigned char* getOpenWeatherMiniPaletteIconFromProgmem(String iconText) {
  if (iconText == "01d" || iconText == "01n") return palette_01d_1x;
  if (iconText == "02d" || iconText == "02n") return palette_02d_1x;
  if (iconText == "03d" || iconText == "03n") return palette_03d_1x;
  if (iconText == "04d" || iconText == "04n") return palette_04d_1x;
  if (iconText == "09d" || iconText == "09n") return palette_09d_1x;
  if (iconText == "10d" || iconText == "10n") return palette_10d_1x;
  if (iconText == "11d" || iconText == "11n") return palette_11d_1x;
  if (iconText == "13d" || iconText == "13n") return palette_13d_1x;
  if (iconText == "50d" || iconText == "50n") return palette_50d_1x;
  return palette_50d_1x;
}

#endif