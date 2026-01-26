#include <Arduino.h>

#include "eeprom.h"
#include "fan.h"
#include "led.h"
#include "version.h"

#ifdef SERIAL_ENABLE
#include "serial.h"
#endif /* SERIAL_ENABLE */

#ifdef WIFI_ENABLE
#include "web.h"
#include "wifi.h"
#endif /* WIFI_ENABLE */

#define SERIAL_BAUD 115200
#define SERIAL_DBG false

void setup() {
  Serial.begin(SERIAL_BAUD);
  Serial.setDebugOutput(SERIAL_DBG);
  delay(1000);

  eeprom_setup();
  fan_setup();
  led_setup();

#ifdef WIFI_ENABLE
  wifi_setup();
  web_setup();
#endif /* WIFI_ENABLE */

  Serial.println("OpenFAN - Micro");
  Serial.println("Version: " + String(VERSION_MAJOR) + "-" +
                 String(VERSION_MINOR) + "-" + String(VERSION_PATCH));
}

void loop() {
  fan_loop();
  led_loop();

#ifdef SERIAL_ENABLE
  serial_loop();
#endif /* SERIAL_ENABLE */

#ifdef WIFI_ENABLE
  wifi_loop();
#endif /* WIFI_ENABLE */
}
