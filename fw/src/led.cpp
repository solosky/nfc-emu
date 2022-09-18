#include "led.h"

#include <avr/io.h>

#include "timer2.h"

#define BLINK_COUNT 2
#define BLINK_INTERVAL_MS 200

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

void led_tag_off() {
  PORTC |= 0x1F;
  PORTB |= _BV(PORTB0);
}

void led_tag_blink(uint8_t i) {
  for (uint8_t j = 0; j < BLINK_COUNT; j++) {
    led_tag_off();
    delay_ms(BLINK_INTERVAL_MS);
    led_tag_update(i);
    delay_ms(BLINK_INTERVAL_MS);
  }
}