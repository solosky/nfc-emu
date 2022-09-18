#ifndef LED_H
#define LED_H

#include <stdint.h>

#define led_sta_off() (PORTB|=_BV(PORTB1))
#define led_sta_on() (PORTB&=~_BV(PORTB1))

void led_init();
void led_tag_update(uint8_t i);
void led_tag_blink(uint8_t i);

#endif