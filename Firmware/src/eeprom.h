#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

bool eeprom_get_12v();
bool eeprom_get_led();
const char *eeprom_get_name();
uint8_t eeprom_get_pwm();
void eeprom_save();
void eeprom_set_12v(bool enable);
void eeprom_set_led(bool enable);
bool eeprom_set_name(const char *devicename);
void eeprom_set_pwm(uint8_t percent);
void eeprom_setup();

#endif /* EEPROM_H */