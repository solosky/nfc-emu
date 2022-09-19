#include "button.h"

#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <string.h>

#include "led.h"
#include "timer2.h"

typedef enum { BTN_STATE_IDLE, BTN_STATE_ARMED, BTN_STATE_PRESSED } btn_state_t;

typedef struct {
  btn_state_t state;
  uint32_t pressed_time;
} button_t;

#define BOUNCE_DELAY_MS 100
#define LONG_PRESS_MS 2000

#define BTN_CNT 3
button_t buttons[BTN_CNT];
btn_callback_t btn_callback = 0;

void button_init() {
  DDRD &= (~_BV(DDD2)) & (~_BV(DDD3)) & (~_BV(DDD4));  // pin input
  // enable pull up, D3 have external pull up
  PORTD |= _BV(PORTD2) | _BV(PORTD4);

  memset(buttons, 0, sizeof(buttons));
}

void button_set_callback(btn_callback_t callback) { btn_callback = callback; }

void button_event(uint8_t i, btn_event_t event) {
  wdt_reset();
  if (btn_callback) {
    btn_callback((btn_key_t)i, event);
  }
}

void button_trigger(uint8_t i, bool pressed) {
  btn_state_t state = buttons[i].state;
  if (pressed) {
    wdt_reset();
    switch (state) {
      case BTN_STATE_IDLE:
        buttons[i].state = BTN_STATE_ARMED;
        buttons[i].pressed_time = millis2();
        break;

      case BTN_STATE_ARMED:
        if (millis2() - buttons[i].pressed_time > BOUNCE_DELAY_MS) {
          buttons[i].state = BTN_STATE_PRESSED;
        }
        break;

      case BTN_STATE_PRESSED: {
        if (millis2() - buttons[i].pressed_time > LONG_PRESS_MS) {
          button_event(i, BTN_EVENT_LONG_PRESSED);
        }
        break;
      }
    }
  } else {
    switch (state) {
      case BTN_STATE_IDLE:
        break;

      case BTN_STATE_ARMED:
        if (millis2() - buttons[i].pressed_time > BOUNCE_DELAY_MS) {
          buttons[i].state = BTN_STATE_IDLE;
          buttons[i].pressed_time = 0;
        }
        break;

      case BTN_STATE_PRESSED: {
        unsigned long pressed_time = millis2() - buttons[i].pressed_time;
        if (pressed_time > BOUNCE_DELAY_MS && pressed_time <= LONG_PRESS_MS) {
          button_event(i, BTN_EVENT_PRESSED);
        }

        buttons[i].state = BTN_STATE_IDLE;
        buttons[i].pressed_time = 0;
        button_event(i, BTN_EVENT_RELEASED);
        break;
      }
    }
  }
}

void button_tick() {
  for (uint8_t i = 0; i < BTN_CNT; i++) {
    button_trigger(i, !(PIND & _BV(2 + i)));  // pressed when low
  }
}