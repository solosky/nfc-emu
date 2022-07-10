#include "timer2.h"

#include <avr/interrupt.h>
#include <avr/io.h>

// use timer2 as rtc
// timer2 will be setup as 1/64 prescaler and overflow after 256 ticks
// timer2 overflow will be 1 / 13.56M * 64 * 256  = 1208.25959 microseconds,
// every timer2 overflow the timer2_millis +=1 and time2_fract will be add 26 (
// 26*8=208 us)

#define OVF_8US_CNT 26
#define FRACT_MAX_CNT 128
#define FRACT_MIRCO_TIME 8  // 8us (0.076125)

volatile unsigned long timer2_millis = 0;
volatile unsigned char timer2_fract = 0;

// micros = timer2_millis * 1000 + timer2_fract * 8

void timer2_init() {
  TCCR2A = 0;           // normal mode, TOP
  TCCR2B = _BV(CS22);   // 1/64 prescaler
  TIMSK2 = _BV(TOIE2);  // enable overflow interuppt
}

ISR(TIMER2_OVF_vect) {
  unsigned long t = timer2_millis;
  unsigned char f = timer2_fract;

  t++;
  f += OVF_8US_CNT;
  if (f >= FRACT_MAX_CNT) {
    t++;
    f = f - FRACT_MAX_CNT;
  }

  timer2_millis = t;
  timer2_fract = f;
}

unsigned long millis2() {
  unsigned long m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read timer0_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to timer0_millis)
  cli();
  m = timer2_millis;
  SREG = oldSREG;

  sei();

  return m;
}
