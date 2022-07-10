#include "led.h"

#include <avr/io.h>

void led_init() {
  // STA led
  DDRB |= _BV(DDB0) | _BV(DDB1);
  DDRC |= _BV(DDC0) | _BV(DDC1) | _BV(DDC2) | _BV(DDC3) | _BV(DDC4);
  PORTB |= _BV(PORTB0) | _BV(PORTB1);
  PORTC |= _BV(PORTC0) | _BV(PORTC1) | _BV(PORTC2) | _BV(PORTC3) | _BV(PORTC4);
}


void led_tag_update(uint8_t i) {
  if (i < 5) {
    PORTB |= _BV(PORTB0);
    PORTC |= 0x1F;
    PORTC &= ~_BV(i);
  } else {
    PORTB &= ~_BV(PORTB0);
    PORTC |= 0x1F;
    PORTC &= ~_BV(i - 5);
  }
}