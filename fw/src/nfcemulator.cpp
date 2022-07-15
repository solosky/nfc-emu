/*****************************************************************************
Written and Copyright (C) by Nicolas Kruse

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*****************************************************************************/

#include "nfcemulator.h"

#include <Arduino.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>
#include <util/delay.h>

static uint8_t RX_MASK[18] = {0,  1, 0,  2, 0,  4, 0,   8, 0,
                              16, 0, 32, 0, 64, 0, 128, 0, 0};
static uint8_t TX_MASK[8] = {1, 2, 4, 8, 16, 32, 64, 128};
static uint16_t FDT_DELAY[2] = {FDT_DELAY_BIT0, FDT_DELAY_BIT1};

static uint8_t REQA[1] = {0x26};
static uint8_t WUPA[1] = {0x52};
static uint8_t HLTA[4] = {0x50, 0x00, 0x57, 0};  //'50' '00' CRC_A
static uint8_t ATQA[2] = {0x44, 0x00};  // Anticollision with udi size = double
static uint8_t SEL_CL1[2] = {0x93, 0x20};
static uint8_t SEL_CL2[2] = {0x95, 0x20};
static uint8_t CT_UID1[5] = {0x88, 0x04, 0xE3, 0xEF,
                             0};  // uid0 uid1 uid2 uid3 BCC
static uint8_t UID2[5] = {0xA2, 0xEF, 0x20, 0x80, 0};  // uid3 uid4 uid5 uid6
                                                       // BCC
static uint8_t SAK_NC[3] = {0x04, 0xDA,
                            0x17};  // Select acknowledge uid not complete
static uint8_t SAK_C[3] = {0x00, 0xFE,
                           0x51};  // Select acknowledge uid complete, Type 2
                                   // (PICC not compliant to ISO/IEC 14443-4)
static uint8_t comp_write_pages = -1;
static bool comp_write_pending = false;
static bool storage_dirty = false;

#define CMD_GET_VERSION 0x60
#define CMD_AUTH 0x1b
#define CMD_GET_SIG 0x3c
#define CMD_FAST_READ 0x3a
#define CMD_READ 0x30
#define CMD_WRITE 0xA2
#define CMD_COMPATIBILITY_WRITE 0xa0

#define AIN1_PORT PORTD
#define AIN1_BIT PORTD7

#define PAGE_LOCKS 2
#define PAGE_PWD 133
#define PAGE_PACK 134

uint8_t *sto;      // Pointer to tag content
uint16_t stoSize;  // Number of avalible bytes on the tag

uint8_t buffer[250];
uint8_t rCount = 0;

emu_event_callback_t emu_event_callback = NULL;

#define LED_OFF (PORTB |= _BV(PORTB1))
#define LED_ON (PORTB &= ~_BV(PORTB1))

#define OC_OUTPUT DDRD |= (1 << 5)
#define OC_INPUT DDRD &= ~(1 << 5)

void addCrc16(uint8_t *Data, uint8_t Length);
void addBcc(uint8_t *Data);

void emu_set_event_callback(emu_event_callback_t callback) {
  emu_event_callback = callback;
}

void initNfcEmulator() {
  // clock divider for 8 bit timer0: clk/1 -> 13.5225 MHz
  TCCR0B |= (1 << CS00);

  // 8 bit timer0: Toggle OC0A on Compare Match and CTC-Mode
  // for 847.5 kHz subcarrier
  TCCR0A |= (1 << COM0B0) | (1 << WGM01);

  // set up 847.5 kHz subcarrier for sending (8 bit timer0)
  OCR0A = SUBC_OVF;

  // CTC-Mode and no clock divider for 16 bit timer1: clk/1
  TCCR1B = (1 << WGM12) | (1 << CS10);

  // Setup Analog Comparator, Enable (ACD), Set Analog Comparator
  // Interrupt Flag on Rising Output Edge (ACIS0, ACIS1)
  ACSR = (0 << ACD) | (1 << ACIS0) | (1 << ACIS1);

  AIN1_PORT |= (1 << AIN1_BIT);
}

void setupNfcEmulator(uint8_t *storage, uint16_t storageSize) {
  CT_UID1[1] = storage[0];
  CT_UID1[2] = storage[1];
  CT_UID1[3] = storage[2];

  UID2[0] = storage[4];
  UID2[1] = storage[5];
  UID2[2] = storage[6];
  UID2[3] = storage[7];

  addCrc16(HLTA, 2);
  addBcc(CT_UID1);
  addBcc(UID2);

  stoSize = storageSize;
  sto = storage;
}

void addCrc16(uint8_t *Data, uint8_t Length) {
  uint8_t ch;
  uint16_t wCrc = 0x6363;  // ITU-V.41

  do {
    ch = *Data++;

    ch = (ch ^ (uint8_t)((wCrc)&0x00FF));
    ch = (ch ^ (ch << 4));
    wCrc = (wCrc >> 8) ^ ((uint16_t)ch << 8) ^ ((uint16_t)ch << 3) ^
           ((uint16_t)ch >> 4);
  } while (--Length);

  *Data = (uint8_t)(wCrc & 0xFF);
  Data++;
  *Data = (uint8_t)((wCrc >> 8) & 0xFF);
}

void addBcc(uint8_t *Data)  // add exclusive-OR of 4 bytes
{
  Data[4] = Data[0] ^ Data[1] ^ Data[2] ^ Data[3];
}

void waitForBitend() {
  while (!(TIFR1 & (1 << OCF1A)))
    ;  // Wait till end of bit-time
  TIFR1 |= (1 << OCF1A);
}

#if (F_CPU == RFID_FREQU)
void waitForOneBitTime() { waitForBitend(); }
#elif (F_CPU == 22000000UL)
// Skip every 17 bit times 1 cycle
void waitForOneBitTime() {
  if (rCount < 7) {
    OCR1AL = CLC_PBIT / 2 - 1;
    rCount++;
  } else {
    OCR1AL = CLC_PBIT / 2 - 2;
    rCount = 0;
  }
  waitForBitend();
}
#elif (F_CPU == 13592500UL)
// Add every 6 bit times 1 cycle
void waitForOneBitTime() {
  if (rCount < 6) {
    OCR1AL = CLC_PBIT / 2 - 1;
    rCount++;
  } else {
    OCR1AL = CLC_PBIT / 2;
    rCount = 0;
  }
  waitForBitend();
}
#else
//#error "Not supported frequency, please add support if possible"
void waitForOneBitTime() {}

#endif

void txManchester(uint8_t *data, uint8_t length) {
  uint8_t txBytePos = 0;
  uint8_t txbitPos = 0;
  uint8_t parity = 0;

  TIFR1 |= (1 << OCF1A);

  // Send SOC
  waitForBitend();
  OC_OUTPUT;
  OCR1A = CLC_PBIT / 2 - 1;  // Set Hi- and Low-Bit
  waitForOneBitTime();
  OC_INPUT;

  do {
    if (TX_MASK[txbitPos] & data[txBytePos]) {
      waitForOneBitTime();
      OC_OUTPUT;
      parity ^= 1;
      waitForOneBitTime();
      OC_INPUT;
    } else {
      waitForOneBitTime();
      OC_INPUT;
      waitForOneBitTime();
      OC_OUTPUT;
    }

    txbitPos++;

    if (txbitPos > 7) {
      if (parity) {
        waitForOneBitTime();
        OC_INPUT;
        waitForOneBitTime();
        OC_OUTPUT;
      } else {
        waitForOneBitTime();
        OC_OUTPUT;
        waitForOneBitTime();
        OC_INPUT;
      }

      txBytePos++;
      txbitPos = 0;
      parity = 0;
    }
  } while (txBytePos < length);

  // Send EOC
  waitForOneBitTime();
  OC_INPUT;
}

void txManchesterEx(uint8_t *data, uint8_t length, uint8_t bits) {
  uint8_t txBytePos = 0;
  uint8_t txbitPos = 0;
  uint8_t parity = 0;

  TIFR1 |= (1 << OCF1A);

  // Send SOC
  waitForBitend();
  OC_OUTPUT;
  OCR1A = CLC_PBIT / 2 - 1;  // Set Hi- and Low-Bit
  waitForOneBitTime();
  OC_INPUT;

  if (length > 0) {
    do {
      if (TX_MASK[txbitPos] & data[txBytePos]) {
        waitForOneBitTime();
        OC_OUTPUT;
        parity ^= 1;
        waitForOneBitTime();
        OC_INPUT;
      } else {
        waitForOneBitTime();
        OC_INPUT;
        waitForOneBitTime();
        OC_OUTPUT;
      }

      txbitPos++;

      if (txbitPos > 7) {
        if (parity) {
          waitForOneBitTime();
          OC_INPUT;
          waitForOneBitTime();
          OC_OUTPUT;
        } else {
          waitForOneBitTime();
          OC_OUTPUT;
          waitForOneBitTime();
          OC_INPUT;
        }

        txBytePos++;
        txbitPos = 0;
        parity = 0;
      }
    } while (txBytePos < length);
  }

  if (bits > 0) {
    txbitPos = 0;
    do {
      if (TX_MASK[txbitPos] & data[txBytePos]) {
        waitForOneBitTime();
        OC_OUTPUT;
        waitForOneBitTime();
        OC_INPUT;
      } else {
        waitForOneBitTime();
        OC_INPUT;
        waitForOneBitTime();
        OC_OUTPUT;
      }
      txbitPos++;
    } while (txbitPos < bits);
  }

  // Send EOC
  waitForOneBitTime();
  OC_INPUT;
}

inline void resetRxFlags() {
  TCNT1 = 0;
  TIFR1 |= (1 << OCF1A);  // Clear Timer Overflow Flag
  ACSR |= (1 << ACI);     // Clear Analog Comparator Interrupt Flag
}

uint8_t rxMiller() {
#if (F_CPU > 16000000)
  uint16_t t;  // For hi cpu clock a 8 bit variable will overflow (CLCM > 0xFF)
#else
  uint8_t t;  // For low cpu clock computing time is to low for 16bit a variable
#endif

  uint16_t cDown = 0x0FFF;
  uint8_t bytePos = 0;
  uint8_t hbitPos = 0;

  OCR1A = CLCL - 1;
  buffer[0] = 0;

  // Wait for transmission end if there is data arriving
  do {
    if (ACSR & (1 << ACI)) resetRxFlags();
  } while (~TIFR1 & (1 << OCF1A));

  // Wait for transmission end if there is data arriving
  do {
    if (TIFR1 & (1 << OCF1A)) {
      TCNT1 = 0;
      TIFR1 |= (1 << OCF1A);
      cDown--;
      if (!cDown) break;
    }
  } while (~ACSR & (1 << ACI));

  if (cDown) {
    resetRxFlags();
    do {
      if ((ACSR & (1 << ACI)) && (TCNT1 > 1)) {
        t = TCNT1;
        resetRxFlags();

        hbitPos += (t > CLCS) + (t > CLCM);

        if (hbitPos > 17) {
          bytePos++;
          hbitPos -= 18;
          buffer[bytePos] = 0;
        }

        buffer[bytePos] |= RX_MASK[hbitPos];

        hbitPos += 2;
      }  // 34 or 41 (hbitPos > 17) click cycles
    } while (~TIFR1 & (1 << OCF1A));
  }

  OCR1A = FDT_DELAY[hbitPos & 1];  // Set delay for answer
  TIFR1 |= (1 << OCF1A);

  if (hbitPos > 7) bytePos++;

  return bytePos;
}

void sendData(uint8_t block) {
  uint8_t i = 0;
  uint16_t pos = (uint16_t)block * 4;

  for (i = 0; i < 16; i++) {
    if (pos >= 540) {
      buffer[i] = 0;
    } else {
      buffer[i] = sto[pos];
    }

    pos++;
  }

  addCrc16(buffer, 16);
  txManchester(buffer, 18);
}

void sendDataFast(uint8_t begin, uint8_t end) {
  uint8_t i = 0;
  uint16_t pos = (uint16_t)begin * 4;
  uint8_t size = (end - begin + 1) * 4;

  for (i = 0; i < size; i++) {
    if (pos >= 540) {
      buffer[i] = 0;
    } else {
      buffer[i] = sto[pos];
    }

    pos++;
  }

  addCrc16(buffer, size);
  txManchester(buffer, size + 2);
}

void receiveData(uint8_t block) {
  uint8_t i = 0;
  uint16_t pos = (uint16_t)block * 4;
  uint8_t crc1 = buffer[6];
  uint8_t crc2 = buffer[7];

  storage_dirty = true;

  //   Serial.print(">>");
  //   for (int i = 0; i < 6; i++) {
  //     Serial.print(buffer[i], HEX);
  //     Serial.print(" ");
  //   }
  //   Serial.println();

  addCrc16(buffer, 6);

  if (buffer[6] == crc1 && buffer[7] == crc2) {
    if (block == PAGE_PACK || block == PAGE_PWD) {
      // skip
    } else if (block == PAGE_LOCKS) {
      // only write 2 lock bytes
      sto[pos + 2] = buffer[4];
      sto[pos + 3] = buffer[5];
    } else {
      for (i = 2; i < 6; i++)  // byte 2-5 contains Data
      {
        if (pos < stoSize) {
          sto[pos] = buffer[i];
        }
        pos++;
      }
    }

    buffer[0] = 0x0A;  // ACK
    txManchesterEx(buffer, 0, 4);
  } else {
    buffer[0] = 0x01;  // NAK for CRC error
    txManchesterEx(buffer, 0, 4);
  }
}

void recieveDataComp(uint8_t bytes) {
  uint8_t i = 0;
  uint8_t crc1 = buffer[16];
  uint8_t crc2 = buffer[17];
  uint16_t pos = (uint16_t)comp_write_pages * 4;
  comp_write_pages = 0;
  comp_write_pending = false;
  storage_dirty = true;

  addCrc16(buffer, 16);

  if (buffer[16] == crc1 && buffer[17] == crc2) {
    for (i = 0; i < 4; i++)  // byte 2-5 contains Data
    {
      if (pos < stoSize) {
        sto[pos] = buffer[i];
      }
      pos++;
    }

    buffer[0] = 0x0A;  // ACK
    txManchesterEx(buffer, 0, 4);
  } else {
    buffer[0] = 0x01;  // NAK for CRC error
    txManchesterEx(buffer, 0, 4);
  }
}

void checkForNfcReader() {
  uint8_t bytes = 1;
  uint8_t state = 0;
  uint8_t cdow = 8;

  if (ACSR & (1 << ACI))  // 13.56 MHz carrier available?
  {
    LED_ON;
    wdt_reset();
    cli();                          // time critial, disable interupt
    AIN1_PORT &= ~(1 << AIN1_BIT);  // Deactivate pull up to increase
                                    // sensitivity
    comp_write_pages = 0;
    comp_write_pending = false;

    while (cdow > 0) {
      bytes = rxMiller();

      if ((state & 7) == S_READY) {
        if (buffer[0] == SEL_CL1[0] && buffer[1] == SEL_CL1[1]) {
          txManchester(CT_UID1, sizeof(CT_UID1));
        } else if (buffer[0] == SEL_CL2[0] && buffer[1] == SEL_CL2[1]) {
          txManchester(UID2, sizeof(UID2));
        } else if (buffer[0] == SEL_CL1[0] && buffer[1] == 0x70) {
          txManchester(SAK_NC, sizeof(SAK_NC));
        } else if (buffer[0] == SEL_CL2[0] && buffer[1] == 0x70) {
          txManchester(SAK_C, sizeof(SAK_C));
          state++;  // Set state to ACTIVE
        } else {
          state &= 8;  // Set state to IDLE/HALT
        }
      } else if ((state & 7) == S_ACTIVE) {
        // Serial.print(">>>");
        // for (int i = 0; i < bytes; i++) {
        //   Serial.print(buffer[i], HEX);
        //   Serial.print(" ");
        // }
        // Serial.println();

        if (comp_write_pending) {
          recieveDataComp(bytes);
        } else if (buffer[0] == CMD_READ) {
          sendData(buffer[1]);
        } else if (buffer[0] == CMD_WRITE) {
          receiveData(buffer[1]);
        } else if (buffer[0] == CMD_COMPATIBILITY_WRITE) {
          if (buffer[1] * 4 < stoSize) {
            comp_write_pending = true;
            comp_write_pages = buffer[1];
            buffer[0] = 0x0A;  // ACK
            txManchesterEx(buffer, 0, 4);
          } else {
            buffer[0] = 0x01;  // NAK for CRC error
            txManchesterEx(buffer, 0, 4);
          }
        } else if (buffer[0] == HLTA[0] && buffer[2] == HLTA[2]) {
          state = S_HALT;
          comp_write_pending = false;
        } else if (buffer[0] == CMD_GET_VERSION) {
          buffer[0] = 0;
          buffer[1] = 4;
          buffer[2] = 4;
          buffer[3] = 2;
          buffer[4] = 1;
          buffer[5] = 0;
          buffer[6] = 0x11;
          buffer[7] = 3;
          addCrc16(buffer, 8);
          txManchester(buffer, 10);
        } else if (buffer[0] == CMD_AUTH) {
          buffer[0] = 0x80;
          buffer[1] = 0x80;
          addCrc16(buffer, 2);
          txManchester(buffer, 4);
        } else if (buffer[0] == CMD_GET_SIG) {
          memset(buffer, 0, 32);
          addCrc16(buffer, 32);
          txManchester(buffer, 34);
        } else if (buffer[0] == CMD_FAST_READ) {
          sendDataFast(buffer[1], buffer[2]);
        } else if (bytes) {
          state &= 8;  // Set state to IDLE/HALT
          comp_write_pending = false;
        }
      } else if (bytes == 1 && (buffer[0] == REQA[0] ||
                                buffer[0] == WUPA[0]))  // state == S_IDLE
      {
        txManchester(ATQA, sizeof(ATQA));
        state = (state & 8) + S_READY;  // Set state to READY
      }

      cdow -= (bytes == 0);
    }
    AIN1_PORT |= (1 << AIN1_BIT);  // Activate pull up to prevent noise from
                                   // toggling the comparator
    LED_OFF;
    sei();

    if (storage_dirty && emu_event_callback) {
      setupNfcEmulator(sto, stoSize);  // reset UID
      emu_event_callback(EMU_EVENT_STORGE_PERSIST);
      storage_dirty = false;
    }
  }
  ACSR |= (1 << ACI);  // Clear comparator interrupt flag
}