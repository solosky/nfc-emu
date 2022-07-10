#ifndef BUTTON_H
#define BUTTON_H

typedef enum {
  BTN_EVENT_PRESSED,
  BTN_EVENT_LONG_PRESSED,
  BTN_EVENT_RELEASED
} btn_event_t;

typedef enum {
    BTN_KEY_NEXT,
    BTN_KEY_ENTER,
    BTN_KEY_PREV,
} btn_key_t;

typedef void (*btn_callback_t)(btn_key_t, btn_event_t);


void button_init();
void button_set_callback(btn_callback_t callback);
void button_tick();

#endif