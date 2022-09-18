#ifndef STORAGE_H
#define STORAGE_H
#include <stdint.h>


#define MAX_TAG_CNT 10
#define TAG_MAGIC 0xAA
#define CONF_MAGIC 0xBB

typedef enum { NTAG_215, MIFARE_CLASSIC_1K } tag_type_t;

# pragma pack(1)
typedef struct {
  uint8_t magic;
  uint8_t type;
  uint16_t size;
  uint8_t data[540];
  uint8_t dummy[96]; //padding to 5 pages
} tag_t;

#define TAG_STO_PAGES  sizeof(tag_t)/SPM_PAGESIZE

typedef struct {
  uint8_t magic;
  uint8_t tag_active;
} conf_t;

# pragma pack()

void storage_init();
void storage_read_tag(uint8_t index, tag_t* tag);
void storage_save_tag(uint8_t index, tag_t* tag);
void storage_reset_tag(uint8_t index, tag_t* tag);
void storage_read_conf(conf_t* conf);
void storage_save_conf(conf_t* conf);

#endif