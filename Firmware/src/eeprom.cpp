#include "eeprom.h"

#include <CRC.h>
#include <EEPROM.h>

#define NAME_DEFAULT "OpenFan-Micro"
#define NAME_LEN_MAX 32
#define NAME_LEN_MIN 3
#define EEPROM_ADR 0
#define ENABLE_12V_DEFAULT false
#define ENABLE_LED_DEFAULT true
#define PWM_DEFAULT 100

typedef struct {
  char name[NAME_LEN_MAX];
  bool enable_12v;
  bool enable_led;
  uint8_t pwm;
  uint8_t crc;
} data_t;

static data_t data;

static bool is_name_valid(const char *devicename) {
  if (!devicename) {
    return false;
  }

  const size_t len = strlen(devicename);

  if ((len < NAME_LEN_MIN) || (len >= NAME_LEN_MAX)) {
    return false;
  }

  if ((devicename[0] == '-') || devicename[len - 1] == '-') {
    return false;
  }

  for (uint8_t i = 0; i < len; i++) {
    if ((devicename[i] != '-') &&
        ((devicename[i] < '0') || (devicename[i] > '9')) &&
        ((devicename[i] < 'A') || (devicename[i] > 'Z')) &&
        ((devicename[i] < 'a') || (devicename[i] > 'z'))) {
      return false;
    }
  }

  return true;
}

bool eeprom_get_12v() { return data.enable_12v; }
bool eeprom_get_led() { return data.enable_led; }
const char *eeprom_get_name() { return data.name; }
uint8_t eeprom_get_pwm() { return data.pwm; }

void eeprom_save() {
  const uint8_t crc = calcCRC8((const uint8_t *)&data, sizeof(data_t) - 1);
  data.crc = crc;
  EEPROM.writeBytes(EEPROM_ADR, &data, sizeof(data_t));
  EEPROM.commit();
}

void eeprom_set_12v(bool enable) { data.enable_12v = enable; }
void eeprom_set_led(bool enable) { data.enable_led = enable; }

bool eeprom_set_name(const char *devicename) {
  if (!is_name_valid(devicename)) {
    return false;
  }

  snprintf(data.name, NAME_LEN_MAX, "%s", devicename);
  return true;
}

void eeprom_set_pwm(uint8_t percent) { data.pwm = percent; }

void eeprom_setup() {
  EEPROM.begin(sizeof(data_t));
  EEPROM.readBytes(EEPROM_ADR, &data, sizeof(data_t));
  const uint8_t crc = calcCRC8((const uint8_t *)&data, sizeof(data_t) - 1);

  if (data.crc != crc) {
    eeprom_set_name(NAME_DEFAULT);
    data.enable_12v = ENABLE_12V_DEFAULT;
    data.enable_led = ENABLE_LED_DEFAULT;
    data.pwm = PWM_DEFAULT;
    eeprom_save();
    return;
  }
}
