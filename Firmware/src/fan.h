#ifndef FAN_H
#define FAN_H

#include <stdint.h>

bool fan_get_12V();
uint8_t fan_get_pwm();
uint32_t fan_get_rpm();
void fan_loop();
void fan_set_12V(bool enable);
void fan_set_pwm(uint8_t val);
void fan_setup();

#endif /* FAN_H */