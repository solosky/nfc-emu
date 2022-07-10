#include <Arduino.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <util/delay.h>

#include "button.h"
#include "led.h"
#include "nfcemulator.h"
#include "storage.h"

tag_t tag;
conf_t conf;

static void nfc_tag_setup() {
  led_tag_update(conf.tag_active);
  storage_read_tag(conf.tag_active, &tag);
  setupNfcEmulator(tag.data, tag.size);
}

static void nfc_btn_callback(btn_key_t btn, btn_event_t event) {
  if (event == BTN_EVENT_PRESSED) {
    if (btn == BTN_KEY_PREV) {
      // select prev
      if (conf.tag_active == 0) {
        conf.tag_active = 9;
      } else {
        conf.tag_active--;
      }
      nfc_tag_setup();
    } else if (btn == BTN_KEY_NEXT) {
      // select next
      if ((++conf.tag_active) >= MAX_TAG_CNT) {
        conf.tag_active = 0;
      }
      nfc_tag_setup();
    }
  }
}

static void emu_event_callback(emu_event_type_t event) {
  if (event == EMU_EVENT_STORGE_PERSIST) {
    Serial.print("persist:");
    Serial.println(conf.tag_active);
    storage_save_tag(conf.tag_active, &tag);
  }
}

void nfc_init() {
  storage_read_conf(&conf);
  if (conf.magic != CONF_MAGIC) {
    conf.magic = CONF_MAGIC;
    conf.tag_active = 0;
    storage_save_conf(&conf);
  }

  initNfcEmulator();
  emu_set_event_callback(emu_event_callback);
  nfc_tag_setup();
  button_set_callback(nfc_btn_callback);
}

void nfc_tick() { checkForNfcReader(); }

void nfc_save() { storage_save_conf(&conf); }