# WeatherStationInternal
Weather Station for Internal base

## Firmware

Firmware for ESP8266
* Flash to this version: https://github.com/espressif/ESP8266_NONOS_SDK/
* This on how to: https://github.com/espressif/ESP8266_NONOS_SDK/issues/179#issuecomment-461602640

* To add support for tides, had to change AT Firmawre to ESP_ATMod version
*   Fork version: https://github.com/dapostol73/ESP_ATMod
* Since ESP_ATMod does not support UDP, created HTTPTimeClient Library use instead of NTPClient

## Building 
If Serial Logging support is required add this build flag to platformio.ini
* build_flags = -D SERIAL_LOGGING
