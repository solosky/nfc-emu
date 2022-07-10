#include "storage.h"

#include <Arduino.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#include "Flash.h"

// uint8_t tagStorage[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
// 0x00, 0x00, 0x00, 0xE1, 0x10, 0x80, 0x00, //Byte  0 to 15:
//                         0x03, 0xFF, 0x00, 0xB4, 0xC2, 0x09, 0x00, 0x00, 0x00,
//                         0xA5, 0x74, 0x65, 0x78, 0x74, 0x2F, 0x68, //Byte 16
//                         to 31: 0x74, 0x6D, 0x6C, 0xEF, 0xBB, 0xBF, 0x3C,
//                         0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x3C, 0x62, 0x6F, 0x64,
//                         //... 0x79, 0x20, 0x73, 0x74, 0x79, 0x6C, 0x65, 0x3D,
//                         0x22, 0x62, 0x61, 0x63, 0x6B, 0x67, 0x72, 0x6F, 0x75,
//                         0x6E, 0x64, 0x2D, 0x63, 0x6F, 0x6C, 0x6F, 0x72, 0x3A,
//                         0x62, 0x6C, 0x61, 0x63, 0x6B, 0x3B, 0x63, 0x6F, 0x6C,
//                         0x6F, 0x72, 0x3A, 0x77, 0x68, 0x69, 0x74, 0x65, 0x3B,
//                         0x66, 0x6F, 0x6E, 0x74, 0x3A, 0x41, 0x72, 0x69, 0x61,
//                         0x6C, 0x3B, 0x22, 0x3E, 0x3C, 0x68, 0x31, 0x3E, 0x53,
//                         0x69, 0x6D, 0x70, 0x6C, 0x65, 0x20, 0x4E, 0x46, 0x43,
//                         0x3C, 0x2F, 0x68, 0x31, 0x3E, 0x3C, 0x70, 0x3E, 0x46,
//                         0x72, 0x6F, 0x6D, 0x20, 0x3C, 0x61, 0x20, 0x68, 0x72,
//                         0x65, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A,
//                         0x2F, 0x2F, 0x79, 0x6F, 0x68, 0x72, 0x62, 0x6C, 0x6F,
//                         0x67, 0x2E, 0x62, 0x6C, 0x6F, 0x67, 0x73, 0x70, 0x6F,
//                         0x74, 0x2E, 0x64, 0x65, 0x22, 0x3E, 0x79, 0x6F, 0x68,
//                         0x72, 0x62, 0x6C, 0x6F, 0x67, 0x3C, 0x2F, 0x61, 0x3E,
//                         0x3C, 0x2F, 0x70, 0x3E, 0x3C, 0x2F, 0x62, 0x6F, 0x64,
//                         0x79, 0x3E, 0x3C, 0x2F, 0x68, 0x74, 0x6D, 0x6C, 0x3E,
//                         0xFE};

// Byte 14 (value=0x80=1024/8) specifies tag size in bytes divided by 8
// Byte 18, 19 specify NDEF-Message size, Byte 20..35 is NDEF-header, content
// starts at Byte 36

// PROGMEM const unsigned char __20_Heart_Wolf_Link_bin[] = {
// 	0x04, 0xca, 0xec, 0xaa, 0x22, 0x75, 0x4c, 0x80, 0x9b, 0x48, 0x0f, 0xe0,
// 	0xf1, 0x10, 0xff, 0xee, 0xa5, 0x00, 0x06, 0x00, 0x1e, 0x21, 0xcc, 0x05,
// 	0x89, 0x9c, 0x8e, 0xe6, 0xe1, 0x31, 0x21, 0x25, 0xd5, 0xfd, 0x0e, 0x00,
// 	0xd3, 0xd4, 0x63, 0x05, 0xb0, 0x58, 0x12, 0x73, 0xa0, 0xc3, 0xe0, 0x43,
// 	0xd7, 0x7c, 0x05, 0x6a, 0x4b, 0x4f, 0x40, 0xee, 0x49, 0x54, 0x04, 0x9a,
// 	0x77, 0xa3, 0xe4, 0xc7, 0xb8, 0x26, 0x43, 0x5b, 0xe1, 0x5d, 0x37, 0xd5,
// 	0xf9, 0x99, 0x5d, 0x54, 0x05, 0xbe, 0xf0, 0x8f, 0xc6, 0x6e, 0xe9, 0xb1,
// 	0x01, 0x03, 0x00, 0x00, 0x02, 0x4f, 0x09, 0x02, 0x0d, 0x12, 0x36, 0x04,
// 	0xf5, 0x9b, 0xce, 0x70, 0xbc, 0xaf, 0x32, 0x8b, 0xb4, 0x59, 0x2b, 0xf0,
// 	0xe3, 0x39, 0xb2, 0x7d, 0xf0, 0xac, 0x84, 0x17, 0xee, 0x91, 0xf6, 0xd9,
// 	0x9e, 0xa4, 0x25, 0xd1, 0x72, 0x54, 0x53, 0xd3, 0x8e, 0xde, 0x5e, 0x3f,
// 	0xf1, 0xb3, 0x1d, 0xd2, 0x3a, 0xea, 0x87, 0xf6, 0x9b, 0xfd, 0x1f, 0x87,
// 	0xd8, 0x3b, 0xb5, 0x0b, 0xd9, 0xcd, 0x38, 0xa0, 0x61, 0xf7, 0xb2, 0x59,
// 	0x95, 0xc8, 0x64, 0xa3, 0xf2, 0x9c, 0x6f, 0x04, 0x43, 0xa6, 0x23, 0x06,
// 	0xb0, 0xab, 0x36, 0x9c, 0xcc, 0x9b, 0xd7, 0x60, 0xc1, 0x82, 0xb8, 0x71,
// 	0xe4, 0xcd, 0xb9, 0x39, 0xcf, 0xb3, 0xa3, 0x64, 0x02, 0x3a, 0xdf, 0xed,
// 	0x5a, 0xea, 0x62, 0x61, 0xbf, 0x55, 0x83, 0x8a, 0xe1, 0x34, 0x6f, 0x6a,
// 	0x04, 0xd3, 0xf6, 0x8c, 0xd1, 0x0a, 0xdc, 0x60, 0x30, 0x4b, 0x2e, 0x24,
// 	0x54, 0x2b, 0x32, 0x69, 0x26, 0x9e, 0x50, 0xe4, 0x04, 0xc0, 0x92, 0x13,
// 	0x40, 0x05, 0x6c, 0x84, 0x8f, 0xf6, 0x89, 0x59, 0xbc, 0xef, 0x07, 0x0c,
// 	0xd6, 0x2c, 0x5b, 0x0a, 0xd9, 0x6d, 0x8c, 0xc5, 0xcd, 0x70, 0x6b, 0x46,
// 	0xfe, 0x21, 0xa3, 0x42, 0x4d, 0x37, 0x01, 0x3b, 0x3c, 0x78, 0xb3, 0x60,
// 	0xd4, 0x14, 0xd9, 0x3e, 0xc4, 0x68, 0x55, 0x3e, 0xa7, 0x0e, 0x49, 0xb5,
// 	0x33, 0x3e, 0xbe, 0xdf, 0x74, 0x42, 0xe5, 0xcc, 0xa5, 0x82, 0x96, 0x99,
// 	0x2e, 0xcd, 0x16, 0xb7, 0x39, 0x31, 0x92, 0xeb, 0xe8, 0x76, 0x8f, 0x25,
// 	0xb2, 0xb6, 0xf3, 0x62, 0xe0, 0xcb, 0xb4, 0x98, 0xd6, 0x7d, 0x57, 0x5e,
// 	0x22, 0xce, 0xcc, 0xd4, 0x90, 0x71, 0xd0, 0xcd, 0x63, 0x9b, 0xeb, 0x2e,
// 	0xa0, 0x69, 0xf2, 0x4d, 0x31, 0x32, 0xc6, 0xb5, 0x96, 0x63, 0x59, 0x4c,
// 	0x99, 0x1c, 0xbb, 0xa4, 0xca, 0x3b, 0xde, 0x65, 0x8c, 0x4d, 0x3f, 0x4e,
// 	0x9a, 0x1a, 0x10, 0xf2, 0x75, 0xd6, 0x8a, 0xe7, 0x4f, 0xc2, 0x29, 0x7b,
// 	0x7d, 0xbb, 0x13, 0x4a, 0xce, 0x7a, 0xf0, 0xe7, 0x56, 0xaa, 0x85, 0x0f,
// 	0xd4, 0x70, 0xdb, 0xd4, 0x88, 0x86, 0xa6, 0x43, 0xff, 0x1f, 0x01, 0x21,
// 	0x8d, 0x1b, 0xce, 0x02, 0xa0, 0x10, 0x3d, 0x90, 0xeb, 0xf4, 0xe8, 0xa1,
// 	0x96, 0x2c, 0x5d, 0xa5, 0x74, 0x3e, 0x90, 0xe0, 0xb0, 0x21, 0xc1, 0x97,
// 	0x01, 0xa6, 0x7e, 0xf1, 0x07, 0x2a, 0x88, 0x1a, 0xbb, 0xe8, 0xe5, 0xcf,
// 	0x9a, 0xf7, 0x5d, 0xff, 0xd8, 0xeb, 0xa5, 0x15, 0x5b, 0x75, 0x40, 0x1c,
// 	0x4f, 0xd5, 0x11, 0x29, 0xa6, 0x22, 0x59, 0x8d, 0xf5, 0xa1, 0xbf, 0x5a,
// 	0xd4, 0x36, 0x8f, 0xbd, 0xea, 0x8d, 0xe9, 0x68, 0x72, 0x5b, 0xd8, 0xd4,
// 	0x6a, 0x72, 0x7f, 0xd9, 0x80, 0x3d, 0xac, 0x3a, 0x41, 0x83, 0xff, 0x86,
// 	0xdf, 0xcb, 0xca, 0x4f, 0x7e, 0x16, 0xfc, 0x1e, 0x3c, 0x58, 0x3f, 0x8c,
// 	0xe1, 0xfd, 0x40, 0x5f, 0xdb, 0xaa, 0xa4, 0xa6, 0x31, 0x6e, 0x31, 0xd8,
// 	0xd1, 0xb9, 0x6d, 0x52, 0x6b, 0x96, 0xa8, 0xf8, 0x52, 0xd3, 0xcb, 0x2b,
// 	0xa3, 0x30, 0xee, 0x08, 0x53, 0x2e, 0x1f, 0xef, 0x1d, 0xdc, 0xc1, 0x42,
// 	0x2b, 0xee, 0x28, 0x58, 0x01, 0x00, 0x0f, 0xbd, 0x00, 0x00, 0x00, 0x04,
// 	0x5f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };
// const unsigned int __20_Heart_Wolf_Link_bin_len = 540;

#define DEFAULT_TAG                                              \
  {                                                              \
    .magic = TAG_MAGIC, .type = NTAG_215, .size = 540, .data = { \
      0x04,                                                      \
      0xca,                                                      \
      0xec,                                                      \
      0xaa,                                                      \
      0x22,                                                      \
      0x75,                                                      \
      0x4c,                                                      \
      0x80,                                                      \
      0x9b,                                                      \
      0x48,                                                      \
      0x0f,                                                      \
      0xe0,                                                      \
      0xf1,                                                      \
      0x10,                                                      \
      0xff,                                                      \
      0xee,                                                      \
      0xa5,                                                      \
      0x00,                                                      \
      0x06,                                                      \
      0x00,                                                      \
      0x1e,                                                      \
      0x21,                                                      \
      0xcc,                                                      \
      0x05,                                                      \
      0x89,                                                      \
      0x9c,                                                      \
      0x8e,                                                      \
      0xe6,                                                      \
      0xe1,                                                      \
      0x31,                                                      \
      0x21,                                                      \
      0x25,                                                      \
      0xd5,                                                      \
      0xfd,                                                      \
      0x0e,                                                      \
      0x00,                                                      \
      0xd3,                                                      \
      0xd4,                                                      \
      0x63,                                                      \
      0x05,                                                      \
      0xb0,                                                      \
      0x58,                                                      \
      0x12,                                                      \
      0x73,                                                      \
      0xa0,                                                      \
      0xc3,                                                      \
      0xe0,                                                      \
      0x43,                                                      \
      0xd7,                                                      \
      0x7c,                                                      \
      0x05,                                                      \
      0x6a,                                                      \
      0x4b,                                                      \
      0x4f,                                                      \
      0x40,                                                      \
      0xee,                                                      \
      0x49,                                                      \
      0x54,                                                      \
      0x04,                                                      \
      0x9a,                                                      \
      0x77,                                                      \
      0xa3,                                                      \
      0xe4,                                                      \
      0xc7,                                                      \
      0xb8,                                                      \
      0x26,                                                      \
      0x43,                                                      \
      0x5b,                                                      \
      0xe1,                                                      \
      0x5d,                                                      \
      0x37,                                                      \
      0xd5,                                                      \
      0xf9,                                                      \
      0x99,                                                      \
      0x5d,                                                      \
      0x54,                                                      \
      0x05,                                                      \
      0xbe,                                                      \
      0xf0,                                                      \
      0x8f,                                                      \
      0xc6,                                                      \
      0x6e,                                                      \
      0xe9,                                                      \
      0xb1,                                                      \
      0x01,                                                      \
      0x03,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x02,                                                      \
      0x4f,                                                      \
      0x09,                                                      \
      0x02,                                                      \
      0x0d,                                                      \
      0x12,                                                      \
      0x36,                                                      \
      0x04,                                                      \
      0xf5,                                                      \
      0x9b,                                                      \
      0xce,                                                      \
      0x70,                                                      \
      0xbc,                                                      \
      0xaf,                                                      \
      0x32,                                                      \
      0x8b,                                                      \
      0xb4,                                                      \
      0x59,                                                      \
      0x2b,                                                      \
      0xf0,                                                      \
      0xe3,                                                      \
      0x39,                                                      \
      0xb2,                                                      \
      0x7d,                                                      \
      0xf0,                                                      \
      0xac,                                                      \
      0x84,                                                      \
      0x17,                                                      \
      0xee,                                                      \
      0x91,                                                      \
      0xf6,                                                      \
      0xd9,                                                      \
      0x9e,                                                      \
      0xa4,                                                      \
      0x25,                                                      \
      0xd1,                                                      \
      0x72,                                                      \
      0x54,                                                      \
      0x53,                                                      \
      0xd3,                                                      \
      0x8e,                                                      \
      0xde,                                                      \
      0x5e,                                                      \
      0x3f,                                                      \
      0xf1,                                                      \
      0xb3,                                                      \
      0x1d,                                                      \
      0xd2,                                                      \
      0x3a,                                                      \
      0xea,                                                      \
      0x87,                                                      \
      0xf6,                                                      \
      0x9b,                                                      \
      0xfd,                                                      \
      0x1f,                                                      \
      0x87,                                                      \
      0xd8,                                                      \
      0x3b,                                                      \
      0xb5,                                                      \
      0x0b,                                                      \
      0xd9,                                                      \
      0xcd,                                                      \
      0x38,                                                      \
      0xa0,                                                      \
      0x61,                                                      \
      0xf7,                                                      \
      0xb2,                                                      \
      0x59,                                                      \
      0x95,                                                      \
      0xc8,                                                      \
      0x64,                                                      \
      0xa3,                                                      \
      0xf2,                                                      \
      0x9c,                                                      \
      0x6f,                                                      \
      0x04,                                                      \
      0x43,                                                      \
      0xa6,                                                      \
      0x23,                                                      \
      0x06,                                                      \
      0xb0,                                                      \
      0xab,                                                      \
      0x36,                                                      \
      0x9c,                                                      \
      0xcc,                                                      \
      0x9b,                                                      \
      0xd7,                                                      \
      0x60,                                                      \
      0xc1,                                                      \
      0x82,                                                      \
      0xb8,                                                      \
      0x71,                                                      \
      0xe4,                                                      \
      0xcd,                                                      \
      0xb9,                                                      \
      0x39,                                                      \
      0xcf,                                                      \
      0xb3,                                                      \
      0xa3,                                                      \
      0x64,                                                      \
      0x02,                                                      \
      0x3a,                                                      \
      0xdf,                                                      \
      0xed,                                                      \
      0x5a,                                                      \
      0xea,                                                      \
      0x62,                                                      \
      0x61,                                                      \
      0xbf,                                                      \
      0x55,                                                      \
      0x83,                                                      \
      0x8a,                                                      \
      0xe1,                                                      \
      0x34,                                                      \
      0x6f,                                                      \
      0x6a,                                                      \
      0x04,                                                      \
      0xd3,                                                      \
      0xf6,                                                      \
      0x8c,                                                      \
      0xd1,                                                      \
      0x0a,                                                      \
      0xdc,                                                      \
      0x60,                                                      \
      0x30,                                                      \
      0x4b,                                                      \
      0x2e,                                                      \
      0x24,                                                      \
      0x54,                                                      \
      0x2b,                                                      \
      0x32,                                                      \
      0x69,                                                      \
      0x26,                                                      \
      0x9e,                                                      \
      0x50,                                                      \
      0xe4,                                                      \
      0x04,                                                      \
      0xc0,                                                      \
      0x92,                                                      \
      0x13,                                                      \
      0x40,                                                      \
      0x05,                                                      \
      0x6c,                                                      \
      0x84,                                                      \
      0x8f,                                                      \
      0xf6,                                                      \
      0x89,                                                      \
      0x59,                                                      \
      0xbc,                                                      \
      0xef,                                                      \
      0x07,                                                      \
      0x0c,                                                      \
      0xd6,                                                      \
      0x2c,                                                      \
      0x5b,                                                      \
      0x0a,                                                      \
      0xd9,                                                      \
      0x6d,                                                      \
      0x8c,                                                      \
      0xc5,                                                      \
      0xcd,                                                      \
      0x70,                                                      \
      0x6b,                                                      \
      0x46,                                                      \
      0xfe,                                                      \
      0x21,                                                      \
      0xa3,                                                      \
      0x42,                                                      \
      0x4d,                                                      \
      0x37,                                                      \
      0x01,                                                      \
      0x3b,                                                      \
      0x3c,                                                      \
      0x78,                                                      \
      0xb3,                                                      \
      0x60,                                                      \
      0xd4,                                                      \
      0x14,                                                      \
      0xd9,                                                      \
      0x3e,                                                      \
      0xc4,                                                      \
      0x68,                                                      \
      0x55,                                                      \
      0x3e,                                                      \
      0xa7,                                                      \
      0x0e,                                                      \
      0x49,                                                      \
      0xb5,                                                      \
      0x33,                                                      \
      0x3e,                                                      \
      0xbe,                                                      \
      0xdf,                                                      \
      0x74,                                                      \
      0x42,                                                      \
      0xe5,                                                      \
      0xcc,                                                      \
      0xa5,                                                      \
      0x82,                                                      \
      0x96,                                                      \
      0x99,                                                      \
      0x2e,                                                      \
      0xcd,                                                      \
      0x16,                                                      \
      0xb7,                                                      \
      0x39,                                                      \
      0x31,                                                      \
      0x92,                                                      \
      0xeb,                                                      \
      0xe8,                                                      \
      0x76,                                                      \
      0x8f,                                                      \
      0x25,                                                      \
      0xb2,                                                      \
      0xb6,                                                      \
      0xf3,                                                      \
      0x62,                                                      \
      0xe0,                                                      \
      0xcb,                                                      \
      0xb4,                                                      \
      0x98,                                                      \
      0xd6,                                                      \
      0x7d,                                                      \
      0x57,                                                      \
      0x5e,                                                      \
      0x22,                                                      \
      0xce,                                                      \
      0xcc,                                                      \
      0xd4,                                                      \
      0x90,                                                      \
      0x71,                                                      \
      0xd0,                                                      \
      0xcd,                                                      \
      0x63,                                                      \
      0x9b,                                                      \
      0xeb,                                                      \
      0x2e,                                                      \
      0xa0,                                                      \
      0x69,                                                      \
      0xf2,                                                      \
      0x4d,                                                      \
      0x31,                                                      \
      0x32,                                                      \
      0xc6,                                                      \
      0xb5,                                                      \
      0x96,                                                      \
      0x63,                                                      \
      0x59,                                                      \
      0x4c,                                                      \
      0x99,                                                      \
      0x1c,                                                      \
      0xbb,                                                      \
      0xa4,                                                      \
      0xca,                                                      \
      0x3b,                                                      \
      0xde,                                                      \
      0x65,                                                      \
      0x8c,                                                      \
      0x4d,                                                      \
      0x3f,                                                      \
      0x4e,                                                      \
      0x9a,                                                      \
      0x1a,                                                      \
      0x10,                                                      \
      0xf2,                                                      \
      0x75,                                                      \
      0xd6,                                                      \
      0x8a,                                                      \
      0xe7,                                                      \
      0x4f,                                                      \
      0xc2,                                                      \
      0x29,                                                      \
      0x7b,                                                      \
      0x7d,                                                      \
      0xbb,                                                      \
      0x13,                                                      \
      0x4a,                                                      \
      0xce,                                                      \
      0x7a,                                                      \
      0xf0,                                                      \
      0xe7,                                                      \
      0x56,                                                      \
      0xaa,                                                      \
      0x85,                                                      \
      0x0f,                                                      \
      0xd4,                                                      \
      0x70,                                                      \
      0xdb,                                                      \
      0xd4,                                                      \
      0x88,                                                      \
      0x86,                                                      \
      0xa6,                                                      \
      0x43,                                                      \
      0xff,                                                      \
      0x1f,                                                      \
      0x01,                                                      \
      0x21,                                                      \
      0x8d,                                                      \
      0x1b,                                                      \
      0xce,                                                      \
      0x02,                                                      \
      0xa0,                                                      \
      0x10,                                                      \
      0x3d,                                                      \
      0x90,                                                      \
      0xeb,                                                      \
      0xf4,                                                      \
      0xe8,                                                      \
      0xa1,                                                      \
      0x96,                                                      \
      0x2c,                                                      \
      0x5d,                                                      \
      0xa5,                                                      \
      0x74,                                                      \
      0x3e,                                                      \
      0x90,                                                      \
      0xe0,                                                      \
      0xb0,                                                      \
      0x21,                                                      \
      0xc1,                                                      \
      0x97,                                                      \
      0x01,                                                      \
      0xa6,                                                      \
      0x7e,                                                      \
      0xf1,                                                      \
      0x07,                                                      \
      0x2a,                                                      \
      0x88,                                                      \
      0x1a,                                                      \
      0xbb,                                                      \
      0xe8,                                                      \
      0xe5,                                                      \
      0xcf,                                                      \
      0x9a,                                                      \
      0xf7,                                                      \
      0x5d,                                                      \
      0xff,                                                      \
      0xd8,                                                      \
      0xeb,                                                      \
      0xa5,                                                      \
      0x15,                                                      \
      0x5b,                                                      \
      0x75,                                                      \
      0x40,                                                      \
      0x1c,                                                      \
      0x4f,                                                      \
      0xd5,                                                      \
      0x11,                                                      \
      0x29,                                                      \
      0xa6,                                                      \
      0x22,                                                      \
      0x59,                                                      \
      0x8d,                                                      \
      0xf5,                                                      \
      0xa1,                                                      \
      0xbf,                                                      \
      0x5a,                                                      \
      0xd4,                                                      \
      0x36,                                                      \
      0x8f,                                                      \
      0xbd,                                                      \
      0xea,                                                      \
      0x8d,                                                      \
      0xe9,                                                      \
      0x68,                                                      \
      0x72,                                                      \
      0x5b,                                                      \
      0xd8,                                                      \
      0xd4,                                                      \
      0x6a,                                                      \
      0x72,                                                      \
      0x7f,                                                      \
      0xd9,                                                      \
      0x80,                                                      \
      0x3d,                                                      \
      0xac,                                                      \
      0x3a,                                                      \
      0x41,                                                      \
      0x83,                                                      \
      0xff,                                                      \
      0x86,                                                      \
      0xdf,                                                      \
      0xcb,                                                      \
      0xca,                                                      \
      0x4f,                                                      \
      0x7e,                                                      \
      0x16,                                                      \
      0xfc,                                                      \
      0x1e,                                                      \
      0x3c,                                                      \
      0x58,                                                      \
      0x3f,                                                      \
      0x8c,                                                      \
      0xe1,                                                      \
      0xfd,                                                      \
      0x40,                                                      \
      0x5f,                                                      \
      0xdb,                                                      \
      0xaa,                                                      \
      0xa4,                                                      \
      0xa6,                                                      \
      0x31,                                                      \
      0x6e,                                                      \
      0x31,                                                      \
      0xd8,                                                      \
      0xd1,                                                      \
      0xb9,                                                      \
      0x6d,                                                      \
      0x52,                                                      \
      0x6b,                                                      \
      0x96,                                                      \
      0xa8,                                                      \
      0xf8,                                                      \
      0x52,                                                      \
      0xd3,                                                      \
      0xcb,                                                      \
      0x2b,                                                      \
      0xa3,                                                      \
      0x30,                                                      \
      0xee,                                                      \
      0x08,                                                      \
      0x53,                                                      \
      0x2e,                                                      \
      0x1f,                                                      \
      0xef,                                                      \
      0x1d,                                                      \
      0xdc,                                                      \
      0xc1,                                                      \
      0x42,                                                      \
      0x2b,                                                      \
      0xee,                                                      \
      0x28,                                                      \
      0x58,                                                      \
      0x01,                                                      \
      0x00,                                                      \
      0x0f,                                                      \
      0xbd,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x04,                                                      \
      0x5f,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00,                                                      \
      0x00                                                       \
    }                                                            \
  }

#define EMPTY_TAG                                        \
  {                                                      \
    .magic = 0, .type = NTAG_215, .size = 540, .data = { \
      0x04,                                              \
      0x68,                                              \
      0x95,                                              \
      0x71,                                              \
      0xfa,                                              \
      0x5c,                                              \
      0x64,                                              \
      0x80,                                              \
      0x42,                                              \
      0x48,                                              \
      0x00,                                              \
      0x00,                                              \
      0xe1,                                              \
      0x10,                                              \
      0x3e,                                              \
      0x00,                                              \
      0x03,                                              \
      0x00,                                              \
      0xfe,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0xf1,                                              \
      0x10,                                              \
      0xff,                                              \
      0xee,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0xf1,                                              \
      0x10,                                              \
      0xff,                                              \
      0xee,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0xbd,                                              \
      0x04,                                              \
      0x00,                                              \
      0x00,                                              \
      0xff,                                              \
      0x00,                                              \
      0x05,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00,                                              \
      0x00                                               \
    }                                                    \
  }

PROGMEM __attribute__((aligned(SPM_PAGESIZE))) const tag_t tags[MAX_TAG_CNT] = {
    DEFAULT_TAG, EMPTY_TAG, EMPTY_TAG, EMPTY_TAG, EMPTY_TAG,

    EMPTY_TAG,   EMPTY_TAG, EMPTY_TAG, EMPTY_TAG, EMPTY_TAG,
};

// RAM buffer needed by the Flash library. Use flash[] to access the buffer
uint8_t ram_buffer[SPM_PAGESIZE];
// Flash constructor
Flash flash((uint8_t*)(tags), sizeof(tags), ram_buffer, sizeof(ram_buffer));

static void storage_init_tag(uint8_t index, tag_t* tag) {
  uint8_t uuid[6] = {0x04, 0x4b, 0xcb, 0x5d, 0x64, 0x80};

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

void storage_read_conf(conf_t* conf) {
  uint8_t* dst = (uint8_t*)conf;
  for (int i = 0; i < sizeof(conf_t); i++) {
    dst[i] = eeprom_read_byte((uint8_t*)i);
  }
}
void storage_save_conf(conf_t* conf) {
  uint8_t* dst = (uint8_t*)conf;
  for (int i = 0; i < sizeof(conf_t); i++) {
    eeprom_write_byte((uint8_t*)i, dst[i]);
    eeprom_busy_wait();
  }
}