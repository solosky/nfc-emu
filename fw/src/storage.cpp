#include "storage.h"

#include <Arduino.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "Flash.h"
#include "timer2.h"

#define EMPTY_TAG                                        \
  {                                                      \
    .magic = 0, .type = NTAG_215, .size = 540, .data = { \
      0x04, 0x68, 0x95, 0x71,                            \
      0xfa, 0x5c, 0x64, 0x80,                            \
      0x42, 0x48, 0x00, 0x00,                            \
      0xe1, 0x10, 0x3e, 0x00,                            \
      0x03, 0x00, 0xfe, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0xf1, 0x10, 0xff, 0xee,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0xf1, 0x10, 0xff, 0xee,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0xbd,                            \
      0x04, 0x00, 0x00, 0xff,                            \
      0x00, 0x05, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00,                            \
      0x00, 0x00, 0x00, 0x00                             \
    }                                                    \
  }

PROGMEM __attribute__((aligned(SPM_PAGESIZE)))
const tag_t tags[MAX_TAG_CNT + 1] = {EMPTY_TAG, EMPTY_TAG, EMPTY_TAG, EMPTY_TAG,
                                     EMPTY_TAG, EMPTY_TAG, EMPTY_TAG, EMPTY_TAG,
                                     EMPTY_TAG, EMPTY_TAG, EMPTY_TAG};

// RAM buffer needed by the Flash library. Use flash[] to access the buffer
uint8_t ram_buffer[SPM_PAGESIZE];
// Flash constructor
Flash flash((uint8_t*)(tags), sizeof(tags), ram_buffer, sizeof(ram_buffer));

static void storage_rand_uuid(uint8_t* uuid) {
  randomSeed(millis2());
  for (int i = 0; i < 6; i++) {
    uuid[i] = (uint8_t)rand();
  }
}

static void storage_init_tag(uint8_t index, tag_t* tag) {
  uint8_t uuid[6];

  storage_rand_uuid(uuid);

  tag->magic = TAG_MAGIC;

  tag->data[0] = 04;  // fixed
  tag->data[1] = uuid[0];
  tag->data[2] = index;
  // BCC 0 is always equal to UID0 ⊕ UID 1 ⊕ UID 2 ⊕ 0x88
  tag->data[3] = tag->data[0] ^ tag->data[1] ^ tag->data[2] ^ 0x88;
  tag->data[4] = uuid[2];
  tag->data[5] = uuid[3];
  tag->data[6] = uuid[4];
  tag->data[7] = uuid[5];

  // BCC 1 is always equal to UID3 ⊕ UID 4 ⊕ UID 5 ⊕ UID6
  tag->data[8] = tag->data[4] ^ tag->data[5] ^ tag->data[6] ^ tag->data[7];
}

void storage_init() {
  if (!flash.check_writable()) {
    Serial.println(
        F("Incompatible or no bootloader present! Please burn correct "
          "bootloader"));
    while (1)
      ;
  }
}

void storage_read_tag(uint8_t index, tag_t* tag) {
  if (index < 0 || index > MAX_TAG_CNT) {
    return;
  }
  memcpy_P(tag, &(tags[index]), sizeof(tag_t));

  // empty tag, reset uid
  if (!tag->magic) {
    storage_init_tag(index, tag);
  }
}

void storage_save_tag(uint8_t index, tag_t* tag) {
  uint16_t page_begin = index * TAG_STO_PAGES;
  if (index < 0 || index > MAX_TAG_CNT) {
    return;
  }
  for (uint8_t i = 0; i < TAG_STO_PAGES; i++) {
    flash.clear_buffer();
    memcpy(ram_buffer, ((uint8_t*)tag) + i * SPM_PAGESIZE, SPM_PAGESIZE);
    flash.write_page(page_begin + i);
  }
}

void storage_reset_tag(uint8_t index, tag_t* tag) {
  memcpy_P(tag, &(tags[MAX_TAG_CNT]), sizeof(tag_t));
  storage_init_tag(index, tag);
  storage_save_tag(index, tag);
}

void storage_read_conf(conf_t* conf) {
  uint8_t* dst = (uint8_t*)conf;
  for (uint32_t i = 0; i < sizeof(conf_t); i++) {
    dst[i] = eeprom_read_byte((uint8_t*)i);
  }
}

void storage_save_conf(conf_t* conf) {
  conf_t old_conf;
  storage_read_conf(&old_conf);
  if (memcmp(&old_conf, conf, sizeof(conf_t)) != 0) {
    uint8_t* dst = (uint8_t*)conf;
    for (uint32_t i = 0; i < sizeof(conf_t); i++) {
      eeprom_write_byte((uint8_t*)i, dst[i]);
      eeprom_busy_wait();
    }
  }
}
