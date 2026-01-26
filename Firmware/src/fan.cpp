#include "fan.h"

#include <Arduino.h>

#include "eeprom.h"

#define LDO_ENA_PIN 0
#define LDO_LED_PIN 3
#define PWM_CHN 0
#define PWM_FRQ 25000
#define PWM_PIN 5
#define PWM_RES 8
#define PWM_SAVE_PERIOD 10000
#define RPM_MEASURE_PERIOD 500
#define TACH_IMPULSES_PER_REV 2
#define TACH_PIN 4

#ifdef STARTUP_BOOST
#define STARTUP_BOOST_PERIOD 5000
#define STARTUP_BOOST_PWM 0xFF
#endif /* STARUP_BOOST */

static uint32_t pwm_save_time = 0;
static uint8_t pwm_val = 0;
static uint32_t rpm = 0;
static volatile uint32_t tach_impulses = 0;

static void IRAM_ATTR TACH_ISR() { tach_impulses++; }

static void start_rpm_measurement() {
  tach_impulses = 0;
  attachInterrupt(digitalPinToInterrupt(TACH_PIN), TACH_ISR, FALLING);
}

static void stop_rpm_measurement(uint32_t now, uint32_t start) {
  const uint32_t period = now - start;
  detachInterrupt(digitalPinToInterrupt(TACH_PIN));

  if (period == 0) {
    return;
  }

  const uint32_t revs = tach_impulses / TACH_IMPULSES_PER_REV;
  rpm = revs * ((float)60000 / (float)period);
}

bool fan_get_12V() { return eeprom_get_12v(); }
uint8_t fan_get_pwm() { return pwm_val; }
uint32_t fan_get_rpm() { return rpm; }

void fan_loop() {
  const uint32_t now = millis();

#ifdef STARTUP_BOOST
  static bool startup_boost = true;
  static const uint32_t startup_time = now;

  if (startup_boost && (now - startup_time) >= STARTUP_BOOST_PERIOD) {
    fan_set_pwm(eeprom_get_pwm());
    startup_boost = false;
  }
#endif /* STARTUP_BOOST */

  static bool rpm_measurement_locked = false;
  static uint32_t rpm_measurement_start = 0;
  static uint32_t rpm_measurement_time = 0;

  if ((now - rpm_measurement_time) >= RPM_MEASURE_PERIOD) {
    if (!rpm_measurement_locked) {
      rpm_measurement_locked = true;
      rpm_measurement_start = now;
      start_rpm_measurement();
    } else {
      rpm_measurement_locked = false;
      stop_rpm_measurement(now, rpm_measurement_start);
    }

    rpm_measurement_time = now;
  }

  if ((now - pwm_save_time) >= PWM_SAVE_PERIOD) {
    if (pwm_val != eeprom_get_pwm()) {
      eeprom_set_pwm(pwm_val);
      eeprom_save();
    }

    pwm_save_time = now;
  }
}

void fan_set_12V(bool enable) {
  const uint8_t state = enable ? HIGH : LOW;
  digitalWrite(LDO_ENA_PIN, state);
  digitalWrite(LDO_LED_PIN, state);
  eeprom_set_12v(enable);
  eeprom_save();
}

void fan_set_pwm(uint8_t pwm) {
  ledcWrite(PWM_CHN, pwm);
  pwm_save_time = millis();
  pwm_val = pwm;
}

void fan_setup() {
  pinMode(LDO_ENA_PIN, OUTPUT);
  pinMode(LDO_LED_PIN, OUTPUT);
  fan_set_12V(eeprom_get_12v());

  pinMode(TACH_PIN, INPUT);
  digitalWrite(TACH_PIN, HIGH);

  ledcSetup(PWM_CHN, PWM_FRQ, PWM_RES);
  ledcAttachPin(PWM_PIN, PWM_CHN);

#ifdef STARTUP_BOOST
  fan_set_pwm(STARTUP_BOOST_PWM);
#else
  fan_set_pwm(eeprom_get_pwm());
#endif /* STARTUP_BOOST */
}
