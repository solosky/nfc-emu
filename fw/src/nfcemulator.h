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

//#define F_CPU 13560000UL
//#define F_CPU 22000000UL
#include <stdint.h>

#define RFID_FREQU 13560000UL
#define CLC_PBIT (uint16_t)(128.0 * F_CPU / RFID_FREQU + 0.5)
#define CLCS CLC_PBIT * 5 / 4
#define CLCM CLC_PBIT * 7 / 4
#define CLCL CLC_PBIT * 9 / 4

#define FDT_DELAY_BD9 (uint16_t)(9.0 * 128 * F_CPU / RFID_FREQU - 1) //Nr of cycles-1 for 9 bit durations
#define FDT_DELAY_BIT0 (FDT_DELAY_BD9 + 20 - CLCL)
#define FDT_DELAY_BIT1 (FDT_DELAY_BD9 + 84 - CLCL)

#define SUBC_OVF (uint8_t)(F_CPU / 847500.0 / 2.0 + 0.5 - 1) //847500 Hz Subcarrier

#define S_IDLE 0
#define S_READY 1
#define S_ACTIVE 2
#define S_HALT 8
#define S_READY_H 9
#define S_ACTIVE_H 10

typedef enum {
    EMU_EVENT_STORGE_PERSIST
}emu_event_type_t;

typedef void (*emu_event_callback_t)(emu_event_type_t);

void initNfcEmulator();
void setupNfcEmulator(uint8_t *storage, uint16_t storageSize);
void checkForNfcReader();

void emu_set_event_callback(emu_event_callback_t callback);