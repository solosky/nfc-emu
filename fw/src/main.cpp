#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "button.h"
#include "led.h"
#include "nfc.h"
#include "nfcemulator.h"
#include "storage.h"
#include "timer2.h"

void power_hold() {
  DDRB |= _BV(DDB2);
  PORTB |= _BV(PORTB2);
}

void power_off() { PORTB &= ~_BV(PORTB2); }

void power_disable_unused() { PRR = _BV(PRTWI) | _BV(PRSPI); }

void wdt_init() {
  cli();
  wdt_reset();
  MCUSR &= ~(1 << WDRF);
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDP3);  // 4s timeout
  WDTCSR |= (1 << WDIE);
  sei();
}

ISR(WDT_vect) {
  wdt_reset();
  nfc_save();
  power_off();
}

int main() {

  sei();

  Serial.begin(115200);
  Serial.print("NFC EMU v1.0");

  power_hold();

  wdt_init();
  timer2_init();
  button_init();
  led_init();
  storage_init();

  led_sta_off();

  power_disable_unused();

  nfc_init();
  while (1) {
    nfc_tick();
    button_tick();
  }
}
