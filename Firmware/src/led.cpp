#include "led.h"

#include <Arduino.h>

#include "eeprom.h"

#define PERIOD 500
#define PIN 10

bool led_get_blinking() { return eeprom_get_led(); }
bool led_get_state() { return !digitalRead(PIN); }

void led_loop() {
  const uint32_t now = millis();
  static uint32_t toggle_time = now;

  if (led_get_blinking() && (now - toggle_time) >= PERIOD) {
    led_set_state(!led_get_state());
    toggle_time = now;
  }
}

void led_set_blinking(bool blinking) {
  eeprom_set_led(blinking);
  eeprom_save();
  led_set_state(false);
}

void led_set_state(bool state) { digitalWrite(PIN, state ? LOW : HIGH); }

void led_setup() {
  pinMode(PIN, OUTPUT);
  led_set_state(false);
}
