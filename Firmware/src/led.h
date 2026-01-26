#ifndef LED_H
#define LED_H

bool led_get_blinking();
bool led_get_state();
void led_loop();
void led_set_blinking(bool blinking);
void led_set_state(bool state);
void led_setup();

#endif /* LED_H */